/*
  Nodemcu e AdafruitIO
  Hardware Utilizado: Nodemcu v1.0, Thermistor e LED
  Autor: Yhan Christian Souza Silva - data: 25/11/2017
*/

// -- Bibliotecas auxiliares --

#include <ESP8266WiFi.h>
#include <Thermistor.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// -- Conexão e credenciais adafruit --

const char *ssid = "Seu SSID";
const char *pass = "Sua Senha";

#define aioServer  "io.adafruit.com"
#define aioServerPort  1883
#define aioUser  "Seu Usuário"
#define aioKey  "Sua YOUR secret AIO KEY"

// -- Instancia objetos --

Thermistor temperature(0);
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, aioServer, aioServerPort, aioUser, aioKey);

// -- Declaração dos feeds (lembre-se do nome que colocou no AdafruitIO)

Adafruit_MQTT_Subscribe _led = Adafruit_MQTT_Subscribe(&mqtt, aioUser "/feeds/Led", MQTT_QOS_1);
Adafruit_MQTT_Publish _temperature = Adafruit_MQTT_Publish(&mqtt, aioUser "/feeds/Temperatura", MQTT_QOS_1);

// -- Variáveis globais pinLed = pino D7 NodeMCU --

const int led = 13;
long lastMillis = 0;
const long interval = 5000;


// -- Protótipo de funções auxiliares --

void conectToWiFi();
void readSensor();
void initMQTT();
void ledCallBack(char *data, int len);
void conectToBroker();

// -- Setup --

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  conectToWiFi();
  initMQTT();
}

// -- Loop infinito --
// Conecta ao Broker, processa a cada 5s e faz a leitura do sensor.

void loop() {
  conectToBroker();
  mqtt.processPackets(5000);
  readSensor();
}

// -- Funções Auxiliares -- 

void conectToWiFi() {
  Serial.println("Conectando a rede: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
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

// -- Realiza a leitura do sensor e pública a cada 5s -- 

void readSensor() {
  if(millis() - lastMillis > interval && mqtt.connected()) {
    float readTemp = temperature.getTemp();
    Serial.print("Temperatura: ");
    Serial.println(readTemp);
    if(! _temperature.publish(readTemp)) Serial.println("Falha ao enviar valor do sensor");  
    lastMillis = millis();
  }
}

// -- Inicia MQTT com Callback escre estado led --

void initMQTT() {
  _led.setCallback(ledCallBack);
  mqtt.subscribe(&_led);
}

// -- Função Callback para assinar valor --

void ledCallBack(char *data, uint16_t  len){
  String ledState = data;
  if(ledState == "ON") digitalWrite(led, HIGH);
  else digitalWrite(led, LOW);
  Serial.print("O led esta: ");
  Serial.println(ledState);
}

// -- Conecta ao BROKER MQTT --
// Faz conexão ao broker, caso haja falha será realizada nova tentativa após 5s

void conectToBroker() {
  int8_t ret, tentativas = 5;
  if(mqtt.connected()) {
    return;
  }
  Serial.println("Conectando-se ao Broker MQTT...");
  while((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Falha ao se conectar.. Tentando novamente em 5s");
    mqtt.disconnect();
    delay(5000);
    tentativas--;
    if(tentativas == 0) {
      Serial.println("O NodeMCU será resetado..");
      while(1);
    }
    Serial.println("Conectado ao Broker MQTT com sucesso");
  }
}




