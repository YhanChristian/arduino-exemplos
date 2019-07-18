/* Projeto - Utilização de WiFiManager e NodeMCU
   Utilizado: NodeMCU, sensor de temperatura DS18B20
   Autores: Lucas Barros | Yhan Christian
*/

// -- Bibliotecas Auxiliares --

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>

// -- Hardware --

#define pinoSensor D0 // Corresponde ao pino D4
#define pinoRelay D1

// -- Instância objetos --

OneWire oneWire(pinoSensor);
DallasTemperature tempSensor(&oneWire);

Ultrasonic ultrasonic(D4, D5);

WiFiClient myClient;
PubSubClient MQTT(myClient);

// -- Variáveis Globais --

bool shouldSaveConfig = false; //flag para salvar config.
char mqttServer[40];
char mqttPort[6];
unsigned long lastMillis;
const int interval = 2000, tankHeight = 10, tankLenght = 15, tankWidth = 10;
float volumeMin, volumeMax;


//Callback confirmando config. salva com sucesso

void saveConfigCallback () {
  Serial.println("Config. salva com sucesso");
  shouldSaveConfig = true;
}

// LeituraSensor Ultrasonico

float readUltrasonic() {

  float distance = ultrasonic.read();

  //Calculo de volume 1cm³ = 1mL logo n preciso realizar conversão

  float volume = (tankHeight - distance) * tankLenght * tankWidth;
  if (volume <= 0) volume = 0;
  Serial.print("Volume mL = ");
  Serial.println(volume);
  return volume;
}

float readTempSensor() {
  tempSensor.requestTemperatures();
  float readTemp = tempSensor.getTempCByIndex(0);
  Serial.print("Temperatura ºC = ");
  Serial.println(readTemp);
  return readTemp;
}


/* Setup com configuração do WiFiManager com parâmetros adicionais
    MQTTServer e MQTTPort, dados salvos no arquivo config.json
*/

void setup() {
  Serial.begin(115200);
  pinMode(pinoRelay, OUTPUT);
  tempSensor.begin();
  Serial.println("Montando arquivo FS");
  if (SPIFFS.begin()) {
    Serial.println("Arquivo montado...");
    if (SPIFFS.exists("/config.json")) {
      Serial.println("Lendo arquivo config");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, buf.get());
        if (error) {
          Serial.print(F("Falha ao analisar arquivo"));
          Serial.println(error.c_str());
          return;
        }

        JsonObject json = doc.as<JsonObject>();
        serializeJson(json, Serial);
        strcpy(mqttServer, json["mqttServer"]);
        strcpy(mqttPort, json["mqttPort"]);

      }
      else Serial.println("Falha em carregar config.json");
    }
  }

  WiFiManagerParameter customMQTTServer("server", "MQTT Server", mqttServer, 40);
  WiFiManagerParameter customMQTTPort("port", "MQTT Port", mqttPort, 6);

  WiFiManager wifiManager;

  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&customMQTTServer);
  wifiManager.addParameter(&customMQTTPort);
  // wifiManager.resetSettings(); //Descomente esta linha caso queria resetar os dados salvos pelo ESP-8266.

  if (!wifiManager.autoConnect("NODEMCU-AP")) {
    Serial.println("Falha na conexão - timeout");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  // conexão realizada

  Serial.println("Conectado com sucesso");

  //Lê / Copia parâmetros atualizados.
  strcpy(mqttServer, customMQTTServer.getValue());
  strcpy(mqttPort, customMQTTPort.getValue());


  //Salva parâmetros atualizados em config.json

  if (shouldSaveConfig) {
    Serial.println("Salvando config.json");
    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();
    File configFile = SPIFFS.open("/config.json", "w");
    json["mqttServer"] = mqttServer;
    json["mqttPort"] = mqttPort;

    serializeJson(json, Serial);
    serializeJson(json, configFile);
    if (serializeJson(json, configFile) == 0) Serial.println(F("Falha em escrever no arquivo"));
    else Serial.println("Arquivo salvo com sucesso!");
    configFile.close();
  }
  delay(1000);
}


void loop() {
  if (millis() - lastMillis > interval) {
    readUltrasonic();
    readTempSensor();
    lastMillis = millis();
  }

// Verificação de Parâmetros Json  * colocar função no setup e não no loop 

  if (SPIFFS.exists("/parametros.json")) {
    File parametros = SPIFFS.open("/config.json", "r");
    if (parametros) {
      size_t size = parametros.size();
      std::unique_ptr<char[]> buf(new char[size]);
      parametros.readBytes(buf.get(), size);
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, buf.get());
      if (error) {
        Serial.print(F("Falha ao analisar arquivo"));
        Serial.println(error.c_str());
        return;
      }
      JsonObject parametros = doc.as<JsonObject>();
      serializeJson(parametros, Serial);
    }
  }
}
