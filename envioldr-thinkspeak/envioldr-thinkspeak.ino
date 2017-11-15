/*
  Envio de dados do Nodemcu para Thinkspeak
  Hardware Utilizado: Nodemcu v1.0, LDR
  Autor: Yhan Christian Souza Silva - data: 15/11/2017
*/

// --- Bibliotecas Auxiliares ---

#include <ESP8266WiFi.h>

// --- Definição de Hardware --- 

#define ldrPin 0


// -- Variáveis para conexão ---

String writeApiKey = "Sua chave de Escrita";
const char *ssid =  "Seu SSID";
const char *pass =  "Sua senha";
const char* thinkSpeakServer = "api.thingspeak.com";

// -- Objeto client ---

WiFiClient client;

// --- Setup realiza conexão ---

void setup() {
  Serial.begin(115200);

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


// --- Loop faz leitura LDR e envia para Think Speak a cada 30s ---

void loop() {
  int readLdr = analogRead(ldrPin);
  if (client.connect(thinkSpeakServer, 80)) {
    String postString = writeApiKey;
    postString += "&field1=";
    postString += String(readLdr);
    postString += "\r\n";
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + writeApiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postString.length());
    client.print("\n\n");
    client.print(postString);
    Serial.print("Luminosidade: ");
    Serial.println(readLdr);
  }
  client.stop();
  Serial.println("Aguardando novo envio...");
  delay(30000);
}

