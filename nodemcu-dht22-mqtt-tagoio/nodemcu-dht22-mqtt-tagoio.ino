/* Leitura de Temperatura e Umidade e publicação de dados via MQTT
   Hardware: NodeMCU e DHT22
   Autor: Yhan Christian Souza Silva - Data: 31/05/2020
*/

//============================ Bibliotecas Auxiliares ===============================

#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <stdio.h>

//============================= Hardware e Defines ==================================

#define DHTPIN D2
#define DHTTYPE DHT22


#define WIFI_SSID       "Seu SSID"
#define WIFI_PASS       "Sua Senha"

#define BAUD_RATE       9600


#define MQTT_SERVER     "mqtt.tago.io"
#define MQTT_PORT       1883

#define MQTT_USER       "Qualquer nome"
#define MQTT_PASS       "Seu token TAGO.IO (verifique ao criar o Device)"

#define MQTT_PUB        "tago/data/post"

//=============================== Instância Objetos =================================

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
PubSubClient MQTT(client);

//============================ Variáveis e Constantes ===============================

const uint16_t tempoEnvio = 30e3;
long ultimoMillisEnvio;


//========================= Protótipo Funções Auxiliares ============================

void conectaWiFi();
void verificaConexaoWiFi();
void conectaBrokerMQTT();
void verificaConexaoBrokerMQTT();
void publicaDadosMQTT();

//================================== Setup ==========================================

void setup() {
  Serial.begin(BAUD_RATE);
  dht.begin();
  conectaWiFi();
  conectaBrokerMQTT();
}

//================================== Loop ===========================================

void loop() {
  verificaConexaoWiFi();
  verificaConexaoBrokerMQTT();

  if (millis() - ultimoMillisEnvio > tempoEnvio) {
    publicaDadosMQTT();
    ultimoMillisEnvio = millis();
  }

  MQTT.loop();

}

//============================ Funções Auxiliares ====================================

// Realiza conexão WiFi

void conectaWiFi() {
  Serial.print("Conectando a rede = ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.print("Conectado IP = ");
  Serial.println(WiFi.localIP());
  delay(1000);
}

//Verifica status conexão

void verificaConexaoWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  else {
    conectaWiFi();
  }
}

//Realiza conexão ao broker MQTT

void conectaBrokerMQTT() {
  uint8_t tentativasConexao = 0;
  char idMQTTRandom[5] = {0};
  MQTT.setServer(MQTT_SERVER, MQTT_PORT);

  randomSeed(random(9999));
  sprintf(idMQTTRandom, "%ld", random(9999));

  while (!MQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT");
    if (MQTT.connect(idMQTTRandom, MQTT_USER, MQTT_PASS)) {
      Serial.println("Conectado ao Broker MQTT com sucesso!");
      tentativasConexao = 0;
    }
    else {
      tentativasConexao++;
      Serial.println("Falha na tentativa de conexão com o Broker MQTT");
      Serial.println("Nova tentativa em 5s");
      if (tentativasConexao == 5) {
        Serial.println("NodeMCU não conectou! o mesmo será resetado");
        delay(1000);
        ESP.restart();
      }
      delay(5000);
    }
  }
}

//Verifica conexão com o broker MQTT

void verificaConexaoBrokerMQTT() {
  if (MQTT.connected()) {
    return;
  }
  else {
    conectaBrokerMQTT();
  }
}

//Realiza leitura do sensor DHT22 - temp e hum
//Formata o JSON conforme padrão TAGO.IO
//Publica dados via MQTT a cada 30s

void publicaDadosMQTT() {
  StaticJsonDocument<250> json_temperatura;
  StaticJsonDocument<250> json_umidade;
  char jsonString[250] = {0};

  float h, t;

  h = dht.readHumidity();
  t = dht.readTemperature();

  //Exibe Serial

  Serial.print("Temperatura ºC = ");
  Serial.println(t);
  Serial.print("Umidade % = ");
  Serial.println(h);

  //Publica dados temperatura

  json_temperatura["variable"] = "temperatura";
  json_temperatura["unit"] = "C";
  json_temperatura["value"] = t;
  memset(jsonString, 0, sizeof(jsonString));
  serializeJson(json_temperatura, jsonString);
  MQTT.publish(MQTT_PUB, jsonString);

  //Publica dados ummidade

  json_umidade["variable"] = "umidade";
  json_umidade["unit"] = "%";
  json_umidade["value"] = h;
  memset(jsonString, 0, sizeof(jsonString));
  serializeJson(json_umidade, jsonString);
  MQTT.publish(MQTT_PUB, jsonString);
}
