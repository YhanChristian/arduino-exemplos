/* Projeto - Utilização de WiFiManager e NodeMCU
   Utilizado: NodeMCU, sensor de temperatura DS18B20
   Autores: Lucas Barros | Yhan Christian
*/

// -- Bibliotecas Auxiliares --

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>

// -- Hardware --

#define pinoSensor D0

// -- Instância objetos --

OneWire oneWire(pinoSensor);
DallasTemperature tempSensor(&oneWire);

Ultrasonic ultrasonic(D4, D5);

WiFiClient myClient;
PubSubClient MQTT(myClient);

// -- Variáveis Globais --

char ssid[40];
char password[20];
char mqttServer[40];
const char* topicTemp = "topicTemp";
const char* topicVolume = "topicVolume";
const int intervalToPublish = 5000;
unsigned long lastMillis;
int mqttPort, tankHeight, tankLenght, tankWidth, volumeMin, volumeMax;



// -- Funções --


// Inicia  SPIFFS

void openFS() {
  Serial.println("Montando arquivo FS");
  if (SPIFFS.begin())
    Serial.println("Arquivo montado...");
  else Serial.println("Falha ao montar arquivo FS");
}


//Lê config.json e salva nas variaveis credenciais WiFi e MQTT

void readConfig() {
  if (SPIFFS.exists("/config.json")) {
    Serial.println("Lendo arquivo config");
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile) {
      size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      //json.printTo(Serial); //Printa dados do arquivo JSON lido
      if (json.success()) {
        Serial.println("\n Carregado Json");
        strcpy(ssid, json["WiFi_Config"]["ssid"]);
        strcpy(password, json["WiFi_Config"]["password"]);
        strcpy(mqttServer, json["MQTT_Config"]["mqttServer"]);
        mqttPort = json["MQTT_Config"]["mqttPort"];
        configFile.close();
      }
      else Serial.println("Falha em carregar config.json");
    }
  }
}

// Conecta a rede WiFi

void connectToWiFi() {
  Serial.println("Conectando a rede: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}

//Conecta ao Broker MQTT

void connectToBrokerMQTT() {
  MQTT.setServer(mqttServer, mqttPort);
  Serial.println("Conectado ao broker");
  delay(500);
}


//Reconecta ao Broker e verifica mensagens de erro

void reconnectToBrokerMQTT() {
  while (!MQTT.connected()) {
    Serial.println("Conectando ao broker MQTT");
    if (MQTT.connect("NodeMCU Client")) Serial.println("Conectado com sucesso!");
    else {
      Serial.print("Falha estado: ");
      Serial.println(MQTT.state());
      Serial.println("Tentar novamente em 5s");
      delay(5000);
    }
  }
}



// Verificação de Parâmetros do arquivo parametros.json

void readParam() {
  if (SPIFFS.exists("/parametros.json")) {
    File parametros = SPIFFS.open("/parametros.json", "r");
    if (parametros) {
      size_t size = parametros.size();
      std::unique_ptr<char[]> buf(new char[size]);
      parametros.readBytes(buf.get(), size);

      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      json.printTo(Serial);
      if (json.success()) {
        Serial.println("\n Carregado Json");
        json.printTo(Serial);
        tankHeight = json["Dimensoes"]["height"];
        tankLenght = json["Dimensoes"]["lenght"];
        tankWidth = json["Dimensoes"]["width"];
        volumeMin = json["Tanque"]["volume_minimo"];
        volumeMax = json["Tanque"]["volume_maximo"];
        parametros.close();
      }
      else Serial.println("Falha em carregar parametros.json");
    }
  }
}


// Leitura Sensor Ultrasonico

float getVolume() {

  float distance = ultrasonic.read();

  //Calculo de volume 1cm³ = 1mL logo n preciso realizar conversão

  float volume = (tankHeight - distance) * tankLenght * tankWidth;
  if (volume <= 0) volume = 0;
  return volume;
}

// Publica Volume

void publishVolume() {
  float vol = getVolume();
  char msgVol[10];
  dtostrf(vol, 10, 0, msgVol);
  MQTT.publish(topicVolume, msgVol);
}

// Leitura Temperatura

float getTemperature() {
  tempSensor.requestTemperatures();
  float readTemp = tempSensor.getTempCByIndex(0);
  return readTemp;
}

//Publica temperatura

void publishTemp() {
  float temp = getTemperature();
  char msgTemp[10];
  dtostrf(temp, 10, 0, msgTemp);
  MQTT.publish(topicTemp, msgTemp);
}


// Setup com chamada de funções de inicalização

void setup() {
  Serial.begin(115200);
  openFS();
  readConfig();
  connectToWiFi();
  connectToBrokerMQTT();
  readParam();
  tempSensor.begin();
  delay(3000);
}

// Loop

void loop() {
  if (!MQTT.connected()) reconnectToBrokerMQTT();
  if (millis() - lastMillis > intervalToPublish) {
    publishTemp();
    publishVolume();
    lastMillis = millis();
  }
  MQTT.loop();
}
