/*
  Nodemcu e Blynk App
  Hardware Utilizado: Nodemcu v1.0, Thermistor
  Autor: Yhan Christian Souza Silva - data: 20/11/2017
  Mais detalhes do Blynk: http://www.blynk.cc/getting-started/
*/

// -- Bibliotecas Auxiliares --

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Thermistor.h>

// -- Defines --

#define BLYNK_PRINT Serial


// -- Conexão e Token --

const char *authToken = "Sua auth Token";
const char *ssid = "Seu SSID";
const char *pass = "Sua Senha";

// --- Instancia objetos ---

BlynkTimer timer;
Thermistor temperature(0);

// -- Protótipo de funções auxiliares --

void sendSensor();

// -- Setup --

void setup() {
  Serial.begin(115200);
  Blynk.begin(authToken, ssid, pass);
  timer.setInterval(1000L, sendSensor);
}

// -- Loop infinito --

void loop() {
  Blynk.run();
  timer.run();
}

// -- Funções Auxiliares --

void sendSensor(){
  float readTemp = temperature.getTemp();
  Serial.print("Temperatura: ");
  Serial.println(readTemp);
  Blynk.virtualWrite(V5, readTemp);
}

