/*
  Envio de dados NodeMCU via HTTP POST 
  Hardware Utilizado: Nodemcu v1.0
  Autor: Yhan Christian Souza Silva - data: 17/11/2020
*/

//============================ Bibliotecas Auxiliares ===============================

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino.h>

//=============================== Defines / Hardware ===================================

#define WIFI_SSID  "Seu SSID"
#define WIFI_PASS "SUA SENHA"
#define BAUD_RATE 9600

#define INTERVALO 60000

//============================ Variáveis e Constantes ===============================

long long ultimoEnvio = 0;
uint16_t contador = 0;

//========================= Protótipo Funções Auxiliares ============================

void conectaWiFi();
boolean checaConexaoWiFi();
void enviaDadosHTTPPost();

//================================== Setup ==========================================

void setup()
{
  Serial.begin(BAUD_RATE);
  conectaWiFi();
  enviaDadosHTTPPost();
}

//================================== Loop ===========================================

void loop()
{
  if((long)millis() - ultimoEnvio >= INTERVALO)
  {
    enviaDadosHTTPPost();
    ultimoEnvio = millis();
  }
}

//=============================== Funções Auxiliares =================================

//Realiza conexão WiFi
void conectaWiFi()
{
  Serial.println("Conectando a rede: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
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

//Checa conexão e retorna status

boolean checaConexaoWiFi()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    return true;
  }
  else
  {
    conectaWiFi();
    return false;
  }
}

//Envia dado via post
void enviaDadosHTTPPost()
{
  if(checaConexaoWiFi())
  {
    HTTPClient http;
    String postData;

    postData += "valor=";
    postData += contador;

    http.begin("http://192.168.0.101/envio_dados_esp/post.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(postData);
    String payload = http.getString();

    Serial.println(postData);
    Serial.println(httpCode);
    Serial.println(payload);

    http.end();
    contador++;
  }

  else 
  {
    Serial.println("Falha na conexão WiFi");
  }

}