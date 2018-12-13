
/*
  Envio de dados de sensor PIR  via MQTT
  Hardware Utilizado: Nodemcu v1.0, sensor de presença PIR
  Autor: Yhan Christian Souza Silva - data: 01/12/2018
*/

// -- Bibliotecas Auxiliares --

#include <ESP8266WiFi.h>
#include <PubSubClient.h>


// -- Hardware -- 

#define sensorPIR D4

// -- Definições de Rede e conexão MQTT / Tópico a ser publicado --

const char *ssid = "familia souza";
const char *password = "34632010";
const char *MQTTServer = "192.168.0.7";
const int   MQTTPort = 1883;
const char* PIRTopic = "pir";


// -- Instância objetos --

WiFiClient myClient;
PubSubClient client(myClient);


// -- Variáveis e constantes --

long lastMillis;
const long interval = 30000;
const int calibratePIR = 30;

// -- Protótipo de funções auxiliares -- 

void connectToWiFi();
void connectToBrokerMQTT();
void reconnectToBrokerMQTT();
void sendSensorData();


// -- Setup -- 

void setup() {
  Serial.begin(115200);
  pinMode(sensorPIR, INPUT);
  for (uint8_t i = 0; i < calibratePIR; i++) {
    Serial.print(".");
    delay(1000);
  }
  connectToWiFi();
  connectToBrokerMQTT();
 
}

void loop() {
  if (!client.connected()) reconnectToBrokerMQTT();
  if (millis() - lastMillis >= interval) {
    sendSensorData();
    lastMillis = millis();
  }
  client.loop();
}

// -- Funções auxiliares --


// -- Fornece as credenciais e conecta a rede -- 

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


// -- Realiza a primeira conexão ao Broker -- 

void connectToBrokerMQTT() {
  client.setServer(MQTTServer, MQTTPort);
  Serial.println("Conectado ao broker");
  delay(500);
}



// -- Reconecta ao Broker e verifica mensagens de erro --

void reconnectToBrokerMQTT() {
  while (!client.connected()) {
    Serial.println("Conectando ao broker MQTT");
    if (client.connect("NodeMCU Client")) Serial.println("Conectado com sucesso!");
    else {
      Serial.print("Falha estado: ");
      Serial.println(client.state());
      Serial.println("Tentar novamente em 5s");
      delay(5000);
    }
  }
}

void sendSensorData() {
  char msgMQTTPIR[2];
  boolean PIRState;
  PIRState = digitalRead(sensorPIR);
  Serial.print("Leitura PIR: ");
  Serial.println(PIRState);
  Serial.print("O estado do sensor pir eh: ");
  Serial.println(PIRState);
  dtostrf(PIRState, 2, 0, msgMQTTPIR);
  client.publish(PIRTopic, msgMQTTPIR);
}
