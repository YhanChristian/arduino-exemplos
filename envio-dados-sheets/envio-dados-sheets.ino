/*
  Envio de dados para Google Sheets
  Hardware Utilizado: Nodemcu v1.0, DHT11
  Autor: Yhan Christian Souza Silva - data: 27/07/2018
  Referências: https://youtu.be/fS0GeaOkNRw
*/

// -- Bibliotecas auxiliares --

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"

// -- Hardware --

#define DHTPIN D2
#define DHTTYPE DHT11

// -- Instância objeto --

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure client;

// -- Variáveis e constantes --

const char *ssid = "Seu SSID";
const char *password = "Sua Senha";
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String googleSheetsID = "Copie aqui o ID da planilha Google";
long previusMillis;
const long interval = 30000;
int temperature, humidity;
float lastH, lastT;

// -- Escopo funções auxiliares --
void connectToWiFi();
void readSensor();
void sendDataToGoogleSheets(int temp, int hum);

// -- Setup -- 

void setup() {
  dht.begin();
  Serial.begin(115200);
  connectToWiFi();
}

// -- Loop leitura de sensor e envio de dados a cada 30s --

void loop() {
  if (millis() - previusMillis >= interval) {
    readSensor();
    previusMillis = millis();
  }
}

// -- Funções auxiliares --

// -- Conecta a rede WiFi fornecida e exibe endereço IP --

void connectToWiFi() {
  Serial.println("Conectando a rede: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.mode(WIFI_STA);
  Serial.println("");
  Serial.println("Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}

/* Realiza leitura de sensor DHT11, verifica se a leitura é válida, caso não seja utiliza o último valor lido através de lastT e lasH 
 * Caso seja atribui os valores de h e t. 
 * Estes valores são convertidos para int e são parâmetros da função sendDataToGoogleSheets
 */

void readSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.print("Temperatura: ");
    Serial.print(lastH);
    Serial.print(" Umidade: ");
    Serial.println(lastT);
    temperature = (int) lastT;
    humidity = (int) lastH;
    sendDataToGoogleSheets(lastT, lastH);
  } else {
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.print(" Umidade: ");
    Serial.println(h);
    temperature = (int) t;
    humidity = (int) h;
    sendDataToGoogleSheets(t, h);
    lastH = h;
    lastT = t;
  }
}

/* Recebe como parâmetros os valores inteiros de temperatura e umidade lidos pelo DHT
 * Conecta ao host scripts google, verifica o certificado fingerprint. 
 * Converte os valores de temperatura e umidade para string, envia estes dados via GET
 * Monta a requisição e resposta e plota os dados na serial
 */

void sendDataToGoogleSheets(int temp, int hum) {
  Serial.print("Conectando à: ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("Falha na conexão ao Google Sheets");
    return;
  }
  if (client.verify(fingerprint, host)) Serial.println("Certificado OK");
  else Serial.println("Certificado NOK, verificar");

  String stringTemp = String(temp, DEC);
  String stringHum = String(hum, DEC);
  String url = "/macros/s/" + googleSheetsID + "/exec?TEMPERATURA=" + stringTemp + "&UMIDADE=" + stringHum;
  Serial.print("Requisitando URL ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Request enviada");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("Headers Received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("Sucesso");
  } else {
    Serial.println("Falha no envio!");
  }
  Serial.println("Resposta:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("Fechando conexão!");
}

