/*
  Exemplo Simples - Botão e Arduino com diminuição corrente.
  Utilizado: Arduino Uno, Push-button, LED
  Autor: Yhan Christian Souza Silva - Data: 01/06/2017
*/

// --- Bibliotecas Auxiliares ---

#include <avr/sleep.h>
#include <avr/power.h>

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
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  sleepNow();
}

// --- Loop---

void loop() {
  if (buttonState){
    turnOnLed();
    buttonState = false;
    delay(250);
    sleepNow();
  }
}

// --- Funções Auxiliares ---

// --- Interrupção, variável buttonState = true ---

void interruptEnable() {
  buttonState = true;
}

/* Mantenho led em nível lógico baixo, interrupção habilitada 
 *  por pino 2 = INT0 ao detectar nível lógico baixo
 *  Habilita-se o modo POWER DOWN, explicado no post
 *  Desabilita-se ADC, SPI, TIMER, TWI(I2C), BCD
 *  Ao entrar na interrupção desabilita modo sleep e habilita todos recursos
 *  de hardware. Por fim desativa a interrupção externa INT0.
 */

void sleepNow() {
  digitalWrite(ledPin, LOW);
  attachInterrupt(0, interruptEnable, LOW); 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer2_disable();
  power_twi_disable();   
  sleep_bod_disable();
  sleep_mode();
  sleep_disable();
  power_all_enable();
  detachInterrupt(0);
}

// ---  Função para ligar led ---
void turnOnLed() {
    digitalWrite(ledPin, HIGH);
}

