
/*
  Subescrevendo tópico no LED
  Hardware Utilizado: Nodemcu v1.0, LED
  Autor: Yhan Christian Souza Silva - data: 11/12/2018
*/

// -- Bibliotecas Auxiliares --

#include <ESP8266WiFi.h>
#include <PubSubClient.h>


// -- Hardware --

#define pinLED D7

// -- Definições de Rede e conexão MQTT / Tópico a ser publicado --

const char *ssid = "familia souza";
const char *password = "34632010";
const char *MQTTServer = "192.168.0.7";
const int   MQTTPort = 1883;
const char* ledTopic = "led";

// -- Instância objetos --

WiFiClient myClient;
PubSubClient MQTT(myClient);

// -- Escopo funções auxiliares --

void connectToWiFi();
void connectToBrokerMQTT();
void reconnectToBrokerMQTT();
void callback(String topic, byte* message, unsigned int length);

// -- Setup --

void setup() {
  Serial.begin(115200);
  pinMode(pinLED, OUTPUT);
  pinMode(pinLED, LOW);
  connectToWiFi();
  connectToBrokerMQTT();
}

void loop() {
  if (!MQTT.connected()) reconnectToBrokerMQTT();
  MQTT.loop();
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
  MQTT.setServer(MQTTServer, MQTTPort);
  MQTT.setCallback(callback);
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

// -- Reconecta ao Broker e verifica mensagens de erro --

void reconnectToBrokerMQTT() {
  while (!MQTT.connected()) {
    Serial.println("Tentando se conectar ao Broker MQTT: " + String(MQTTServer));
    if (MQTT.connect("NodeMCU MQTT")) {
      Serial.println("Conectado");
      MQTT.subscribe(ledTopic);
    } else {
      Serial.println("Falha ao Reconectar");
      Serial.println("Tentando se reconectar em 2 segundos");
      delay(2000);
    }
  }
}
