/*
  Exemplo Simples - Botão e Arduino sem diminuir corrente.
  Utilizado: Arduino Uno, Push-button, LED
  Autor: Yhan Christian Souza Silva - Data: 01/06/2017
*/

// --- Hardware ---

const int buttonPin = 2;
const int ledPin =  8;

// --- Variáveis ---

boolean buttonState = true;

// --- Setup ---

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

// --- Loop ---

void loop() {
  buttonState = digitalRead(buttonPin);
  if (!buttonState) digitalWrite(ledPin, HIGH);
  else  digitalWrite(ledPin, LOW);
}
