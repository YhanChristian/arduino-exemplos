/*
  Nome: Yhan Christian Souza Silva
  Neste projeto, simulando uma estufa de hortaliças, deve ser desenvolvido no simulador TinkerCad (https://www.tinkercad.com/) um circuito eletrônico envolvendo um sensor de temperatura, uma buzina, um LED e um motor, controlados por meio de um Arduino. O projeto deve seguir a seguinte funcionalidade:

  (a) Fazer a leitura da temperatura;

  (b) Fazer o acionamento de um motor de ventilador caso a temperatura seja igual ou maior a 30 °C;

  (c) Caso a temperatura ultrapasse os 50 °C, um LED vermelho e uma buzina devem acionar avisando uma situação de emergência.

  Todo o processo descrito deve ser programado em linguagem C na placa de projetos Arduino.
*/


/* Define --------------------------------------------------------------------*/
#define MOTOR_PIN 4
#define BUZZER_PIN 3
#define LED_PIN 2
#define TEMP_SENSOR 0 /*A0*/

/* Global variables ----------------------------------------------------------*/
const uint16_t interval = 1000;
unsigned long long lastMillis;


/* Bodies of private functions -----------------------------------------------*/
float getTemperature(const int pin) {
  return ((analogRead(pin) * (5 / 1024.0)) - 0.5) * 100;
}


void checkStatus() {
  float temp = 0;

  /*Obtem temp atual*/
  temp = getTemperature(TEMP_SENSOR);

  Serial.println("Temperatura: " + String(temp) + "C");

  if (temp > 50) {
    Serial.println("EMERGENCIA! Temperatura acima de 50C");
    changeOutputState(MOTOR_PIN, true);
    changeOutputState(BUZZER_PIN, true);
    changeOutputState(LED_PIN, true);
    return;
  }
  if (temp > 30) {
    Serial.println("Acionado ventilador temp acima de 30C");
    changeOutputState(MOTOR_PIN, true);
    changeOutputState(BUZZER_PIN, false);
    changeOutputState(LED_PIN, false);
    return;
  }

  changeOutputState(MOTOR_PIN, false);
  changeOutputState(BUZZER_PIN, false);
  changeOutputState(LED_PIN, false);
}

void changeOutputState(const int pin, boolean state) {
  digitalWrite(pin, state);
}



/* Setup ---------------------------------------------------------------------*/
void setup()
{
  Serial.begin(9600);
  Serial.println("Iniciando setup projeto!");
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TEMP_SENSOR, OUTPUT);
}


/* Loop ----------------------------------------------------------------------*/
void loop()
{
  /*Realiza leitura periódica da temperatura*/
  if (millis() - lastMillis > interval) {
    lastMillis = millis();
    checkStatus();
  }
}
