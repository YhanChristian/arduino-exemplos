/*
  Medindo distância por ultrasom - exibição no LCD
  Utilizado: Arduino Uno, Ultrasonico HC-SR04, LCD
  Autor: Yhan Christian Souza Silva - Data: 23/01/2015
*/

// --- Bibliotecas Auxiliares ---
#include <Ultrasonic.h>

// --- Hardware ---
#define  triggerPin 8
#define  echoPin 9


// --- Inicializo o sensor ultrasom ---
Ultrasonic ultrasonic(triggerPin, echoPin);

void setup(){
  Serial.begin(9600);
}

void loop()
{
  float cm, pol;           
  long microsec = ultrasonic.timing();
  cm = ultrasonic.convert(microsec, Ultrasonic::CM);
  pol = ultrasonic.convert(microsec,Ultrasonic::IN);

// --- Exibição dos valores no monitor serial --
  Serial.print("CM: ");
  Serial.print(cm);
  Serial.print( " | ");
  Serial.print("POL: ");
  Serial.println(pol);
  delay(1000);
}
