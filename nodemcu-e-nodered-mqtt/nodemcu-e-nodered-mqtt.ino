/*
  Controlando NodeMCU com Node-RED via MQTT
  Hardware Utilizado: Nodemcu v1.0, LDR, LED
  Autor: Yhan Christian Souza Silva - data: 05/08/2018
*/

// -- Bibliotecas Auxiliares --

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// -- Hardware --

#define LDR A0
#define pinLED D4

// -- Definições de Rede e conexão MQTT / Tópicos --

const char *ssid = "Seu SSID";
const char *password = "Sua Senha";
const char *MQTTServer = "IP local o qual o Mosquitto esta instalado";
const int   MQTTPort = 1883;
const char* ledTopic = "led";
const char* sensorTopic = "luminosity";


// -- Instância objetos --

WiFiClient myClient;
PubSubClient client(myClient);

// -- Variáveis e constantes --

long lastMillis;
const long interval = 30000;


// -- Escopo funções auxiliares --

void connectToWiFi();
void connectToBrokerMQTT();
void reconnectToBrokerMQTT();
void callback(String topic, byte* message, unsigned int length);
void sendSensorData();

// -- Setup --

void setup() {
  Serial.begin(115200);
  pinMode(pinLED, OUTPUT);
  connectToWiFi();
  connectToBrokerMQTT();
}

// -- Loop --

void loop() {
  if (!client.connected()) reconnectToBrokerMQTT();
  if (millis() - lastMillis >= interval) {
    sendSensorData();
    lastMillis = millis();
  }
  client.loop();
}

// -- Funções Auxiliares --


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
  client.setCallback(callback);
  Serial.println("Conectado ao broker");
  delay(500);
}

// -- Reconecta ao Broker e verifica mensagens de erro --

void reconnectToBrokerMQTT() {
  while (!client.connected()) {
    Serial.println("Conectando ao broker MQTT");
    if (client.connect("NodeMCU Client")) {
      client.subscribe(ledTopic);
      Serial.println("Conectado com sucesso!");
    }
    else {
      Serial.print("Falha estado: ");
      Serial.println(client.state());
      Serial.println("Tentar novamente em 5s");
      delay(5000);
    }
  }
}

/* Verificar tópico para subescrever alterando o estado do led,conforme mensagem
    ON - LIGA LED  | OFF - DESLIGA LED
*/

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.print(topic);
  Serial.print(". Mensagem: ");
  String messageTopic;

  for (uint8_t i = 0; i <  length; i++) {
    Serial.print((char)message[i]);
    messageTopic += (char)message[i];
  }
  Serial.println();

  if (topic == ledTopic) {
    if (messageTopic == "ON") {
      digitalWrite(pinLED, HIGH);
      Serial.print("LED - LIGADO");
    }
    else if (messageTopic == "OFF") {
      digitalWrite(pinLED, LOW);
      Serial.print("LED - DESLIGADO");
    }
  }
  Serial.println();
}

/* Faz leitura do sensor LDR converte o valor para caractere
   publica o valor no tópico
*/

void sendSensorData() {
  char msgLumMQTT[10];
  int readSensor = analogRead(LDR);
  Serial.print("Luminosidade: ");
  Serial.println(readSensor);
  dtostrf(readSensor, 10, 0, msgLumMQTT);
  client.publish(sensorTopic, msgLumMQTT);
}

