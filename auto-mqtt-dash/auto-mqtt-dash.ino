/*
  Automação Residencial - MQTT Dash e NodeMCU
  Hardware Utilizado: Nodemcu v1.0, DHT11, Módulo relé 2 canais, 2 lâmpadas AC para teste acionamento
  Autor: Yhan Christian Souza Silva - data: 13/10/2018
*/

// -- Bibliotecas Auxilares --
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// -- Hardware --

#define lampada01 D4
#define lampada02 D5
#define DHTPIN D7
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);


// -- Definições de Rede e conexão MQTT --

const char *ssid = "Seu SSID";
const char *password = "Sua Senha";

/* Dados do CloudMQTT e tópico do MQTT Dash
   Lembre-se que se um destes valores estiver incorreto
   O NodeMCU não enviará dados para MQTT Dash.
*/

const char* cloudMQTTServer = "Server da Instância CloudMQTT";
const char* cloudMQTTUser = "User da Instância CloudMQTT";
const char* cloudMQTTPassword = "Senha da Instância CloudMQTT";
const int cloudMQTTPort = 17770;

const char* MQTTDashTemp = "temperatura";
const char* MQTTDashHum = "umidade";
const char* MQTTDashLamp01 = "lampada01";
const char* MQTTDashLamp02 = "lampada02";


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
  pinMode(lampada01, OUTPUT);
  pinMode(lampada02, OUTPUT);
  connectToWiFi();
  connectToBrokerMQTT();
  dht.begin();
}


// -- Loop para reconectar ao Broker e enviar dados do sensor a cada 30s --

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
  client.setServer(cloudMQTTServer, cloudMQTTPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Conectando ao broker MQTT");
    if (client.connect("NodeMCU Client", cloudMQTTUser, cloudMQTTPassword)) Serial.println("Conectado com sucesso!");
    else {
      Serial.print("Falha estado: ");
      Serial.println(client.state());
      delay(3000);
    }
  }
  client.subscribe(MQTTDashLamp01, 1); // Subscreve valor lâmpada 01 QoS 1
  client.subscribe(MQTTDashLamp02, 1); // Subscreve valor lâmpada 02 QoS 1
}

// -- Reconecta ao Broker e verifica mensagens de erro --

void reconnectToBrokerMQTT() {
  while (!client.connected()) {
    Serial.println("Tentando conexão ao broker MQTT");

    boolean connectedToBroker = strlen(cloudMQTTUser) > 0 ?
                                client.connect("NodeMCU Client", cloudMQTTUser, cloudMQTTPassword) :
                                client.connect("NodeMCU Client");

    if (connectedToBroker)  {
      Serial.println("Conectado com sucesso!");
      client.subscribe(MQTTDashLamp01, 1); // Subscreve valor lâmpada 01 QoS 1
      client.subscribe(MQTTDashLamp02, 1); // Subscreve valor lâmpada 02 QoS 1
    }
    else {
      Serial.println("Falha durante a conexão erro#code: ");
      Serial.println(String(client.state()).c_str());
      Serial.println("Tentando conexão novamente em 15s");
      delay(15000);
    }
  }
}

/* Verificar tópico para subescrever alterando o estado lâmpada 01 ou lâmpada 02,conforme mensagem
    TÓPICO: lampada01 1 - LIGADO 0 - DESLIGADO
    TÓPICO: lampada02 1 - LIGADO 0 - DESLIGADO

    Vale lembrar que o módulo relé trabalha com nível baixo, por isso LOW (A LÂMPADA LIGA)
*/

void callback(String topic, byte* message, unsigned int length) {
  message[length] = '\0';
  String messageTopic = String((char*)message);
  Serial.print("Mensagem recebida no tópico: ");
  Serial.print(topic);
  Serial.print(" | ");
  Serial.print("Mensagem: ");
  Serial.println(messageTopic);

  if (topic == MQTTDashLamp01) {
    if (messageTopic == "1") {
      digitalWrite(lampada01, LOW);
      Serial.print("LÂMPADA 01 - LIGADA");
    }
    else if (messageTopic == "0") {
      digitalWrite(lampada01, HIGH);
      Serial.print("LÂMPADA 01 - DESLIGADA");
    }
  }

  if (topic == MQTTDashLamp02) {
    if (messageTopic == "1") {
      digitalWrite(lampada02, LOW);
      Serial.print("LÂMPADA 02 - LIGADA");
    }
    else if (messageTopic == "0") {
      digitalWrite(lampada02, HIGH);
      Serial.print("LÂMPADA 02 - DESLIGADA");
    }
  }
  Serial.println();
}

/* FAz leitura do sensor DHT converte o valor para caractere
   publica o valor de temperatura no tópico temperatura
   pulica o valor de umidade no tópico umidade
*/

void sendSensorData() {
  char msgTempMQTT[10];
  char msgHumMQTT[10];
  float leituraTemperatura = dht.readTemperature();
  float leituraUmidade = dht.readHumidity();
  if (isnan(leituraTemperatura) || isnan(leituraUmidade)) Serial.print("Falha leitura sensor DHT11");
  else {
    Serial.print("Temperatura: ");
    Serial.print(leituraTemperatura);
    dtostrf(leituraTemperatura, 10, 0, msgTempMQTT);
    client.publish(MQTTDashTemp, msgTempMQTT);
    Serial.print(" | ");
    Serial.print("Umidade: ");
    Serial.println(leituraUmidade);
    dtostrf(leituraUmidade, 10, 0, msgHumMQTT);
    client.publish(MQTTDashHum, msgHumMQTT);
  }
}
