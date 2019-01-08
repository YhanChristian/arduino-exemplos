/*
  Sistema de Alarme IoT
  Hardware Utilizado: Nodemcu v1.0, Sensor de Presença PIR, Push-button, LED RGB e Buzzer
  Autor: Yhan Christian Souza Silva - data: 13/12/2018
*/

// -- Bibliotecas Auxiliares --

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial

// -- Hardware --

#define buzzer D5
#define PIRSensor D4
#define pushButton D3
#define redPin D2
#define greenPin D1
#define bluePin D0

// -- Conexão e Token --

const char *authToken = "Sua auth Token";
const char *ssid = "Seu SSID";
const char *pass = "Sua Senha";

// -- Variáveis e constantes --

int virtualPinRead;
const int calibrationTime = 30;

// -- Escopo funções auxiliares --

void notifyAlarm();


void setup() {
  Serial.begin(115200);
  Blynk.begin(authToken, ssid, pass);
  pinMode(buzzer, OUTPUT);
  pinMode(PIRSensor, INPUT);
  pinMode(pushButton, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  digitalWrite(buzzer, LOW);
  digitalWrite(redPin, HIGH); // Inicia-se sistema com alarme desativado
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
  for (int i = 0; i < calibrationTime; i++) Serial.println("Aguardando calibração - Sensor PIR");
}

void loop() {
  Blynk.run();
  notifyAlarm();
}

BLYNK_WRITE(V0) {
  virtualPinRead = param.asInt();
}

void notifyAlarm() {
  if (virtualPinRead) {
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH); // Indica alarme ativado
    int PIRSensorActive = digitalRead(PIRSensor);
    int buttonPressed = digitalRead(pushButton);

    /* Se PIR for ativado ou botão pressionado, é enviada notificação
       Acionado buzzer e led azul piscando
    */
    if (PIRSensorActive) {
      Serial.println("Intruso detectado - Sensor presença");
      Blynk.notify("Intruso detectado - Sensor presença");
      digitalWrite(bluePin, HIGH);
      digitalWrite(buzzer, HIGH);
      digitalWrite(greenPin, LOW);
      delay(5000);
    }
    else if (pushButton) {
      Serial.println("Intruso detectado - Botão (sensor)");
      Blynk.notify("Intruso detectado - Botão (sensor)");
      digitalWrite(bluePin, HIGH);
      digitalWrite(buzzer, HIGH);
      digitalWrite(greenPin, LOW);
      delay(5000);
    }

    // -- Caso ambos não sejam acionados manter luz verde acesa --
    else {
      digitalWrite(greenPin, HIGH);
      digitalWrite(bluePin, LOW);
      digitalWrite(buzzer, LOW);
    }
  }

  // -- Alarme desativado --

  else {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
    digitalWrite(buzzer, LOW);
  }
}
