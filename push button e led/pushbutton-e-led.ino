/*
  Exemplo de Utilização Push-button e LED
  Utilizado: Arduino Uno, Push-button, LED 
  Autor: Yhan Christian Souza Silva - Data: 17/11/2016
*/


// --- Hardware --- 

#define buttonPin 2
#define ledPin 8

// --- Variáveis --- 

int ledState = HIGH,buttonState, lastButtonState = LOW;            
long lastDebounceTime = 0, debounceDelay = 50;    

// --- Setup ---

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
}

// --- Loop --- 

void loop() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }
  digitalWrite(ledPin, ledState);
  lastButtonState = reading;
}

