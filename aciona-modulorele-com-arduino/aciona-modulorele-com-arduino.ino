/*
  Exemplo de Utilização Módulo Relé e botões
  Utilizado: Arduino Uno, Push-button, módulo relé 2 canais
  Autor: Yhan Christian Souza Silva - Data: 25/11/2016
*/


// --- Hardware ---

#define btnRelay01 2
#define btnRelay02 3
#define relay01 8
#define relay02 9

// --- Variáveis ---

int buttonState01, buttonState02, relay01State = HIGH, relay02State = HIGH, lastBtn01State = LOW, lastBtn02State = LOW;
unsigned long lastDebounceTime = 0, debounceDelay = 50;

// --- Setup ---

void setup() {
  pinMode(btnRelay01, INPUT);
  pinMode(btnRelay02, INPUT);
  pinMode(relay01, OUTPUT);
  pinMode(relay02, OUTPUT);
  digitalWrite(relay01, relay01State);
  digitalWrite(relay02, relay02State);
}

// --- Loop ---

void loop() {
  int readRelay01 = digitalRead(btnRelay01);
  int readRelay02 = digitalRead(btnRelay02);
  if (readRelay01 != lastBtn01State) {
    lastDebounceTime = millis();
  }
  if (readRelay02 != lastBtn02State) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (readRelay01 != buttonState01) {
      buttonState01 = readRelay01;
      if (buttonState01 == HIGH) {
        relay01State = !relay01State;
      }
    }
    if (readRelay02 != buttonState02) {
      buttonState02 = readRelay02;
      if (buttonState02 == HIGH) {
        relay02State = !relay02State;
      }
    }
  }
  digitalWrite(relay01, relay01State);
  digitalWrite(relay02, relay02State);
  lastBtn01State = readRelay01;
  lastBtn02State = readRelay02;
}

