/*
  Exemplo Simples - Botão e Arduino com diminuição corrente (libs externas).
  Utilizado: Arduino Uno, Push-button, LED
  Autor: Yhan Christian Souza Silva - Data: 01/06/2017
*/


// --- Bibliotecas Auxiliares ---

#include "LowPower.h"

// --- Protótipo funções auxiliares ---

void interruptEnable();
void sleepNow();
void turnOnLed();

// --- Hardware ---

const int ledPin = 8;
const int buttonPin = 2;
boolean buttonState;

// --- Setup ---

void setup() {
    pinMode(buttonPin, INPUT_PULLUP);   
    pinMode(ledPin, OUTPUT);
}

// --- Loop ---

void loop() {
  sleepNow();
}

// --- Funções Auxiliares ---

// --- Interrupção, chama função turnOnLed ---

void interruptEnable() {  
  buttonState = true;
  turnOnLed();
}

/* Mantém nível lógico baixo no led
 * Interrupção externa INT0 = pino digital 2 ao detectar nível lógico baixo
 * Desabilita ADC, BOD e chama sleep. Percebe-se que a biblioteca oculta muita coisa rs =)
 * Por fim desativa a interrupção externa INT0.
 */

void sleepNow() {
    digitalWrite(ledPin, LOW);
    attachInterrupt(0, interruptEnable, LOW);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
    detachInterrupt(0);
}

void turnOnLed() {
   digitalWrite(ledPin, HIGH);
}
