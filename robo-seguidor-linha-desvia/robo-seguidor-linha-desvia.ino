/*
  Robô Seguidor de Linha que desvia de obstaculo
  Utilizado: Arduino Uno, Ponte H L298N, sensor TCRT5000
  Autor: Yhan Christian Souza Silva - Data: 05/07/2019
*/


// -- Bibliotecas Auxiliares --

#include <Ultrasonic.h>

// -- Hardware --

// Motores

#define IN1 4
#define IN2 5
#define ENA 6 // Controle de Velocidade motor A (PWM)
#define IN3 8
#define IN4 9
#define ENB 10 // Controle de Velocidade motor B (PWM)

// Sensores

#define pinoSensorA 2
#define pinoSensorB 3

// Instância objeto Ultrassonic

Ultrasonic ultrasonic(12, 13); //Define pinos de Trigger e Echo


// -- Variáveis e Constantes --

// Velocidade Constante Motores

const int velocidadeMotores = 85;
const int velocidadeCurva = 80;

// Correção de Velocidade

const int corrigeMotorA = 0;
const int corrigeMotorB = 0;


// Guarda leitura anterior sensores

bool leituraAnteriorSensorA = false;
bool leituraAnteriorSensorB = false;


// Distância mínima ultrassônico
const int distanciaMinima = 10;

// Correção erro leitura ultrassônico

const float offsetDistancia = -0.31;

// -- Setup --

void setup() {
  pinMode(pinoSensorA, INPUT);
  pinMode(pinoSensorB, INPUT);
  for (uint8_t motorA = 4; motorA < 7; motorA++) pinMode(motorA, OUTPUT); // Habilita como saída pinos motor A
  for (uint8_t motorB = 8; motorB < 12; motorB++) pinMode(motorB, OUTPUT); // Habilita como saída pinos motor B

  // Liga inicialmente os dois motores com mesma velocidade
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, velocidadeMotores + corrigeMotorA);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, velocidadeMotores + corrigeMotorB);
  delay(50);
}

// -- Loop --

void loop() {
  bool valorSensorA = digitalRead(pinoSensorA);
  bool valorSensorB = digitalRead(pinoSensorB);
  int distancia = ultrasonic.read();

  // Dois sensores fora da linha
  if (!valorSensorA && !valorSensorB) {
    valorSensorA = leituraAnteriorSensorA;
    valorSensorB = leituraAnteriorSensorB;
  }

  // Dois sensores na linha
  else if (valorSensorA && valorSensorB) {
    analogWrite(ENA, velocidadeMotores + corrigeMotorA);
    analogWrite(ENB, velocidadeMotores + corrigeMotorB);
  }

  // Apenas sensor A na linha
  else if (valorSensorA && !valorSensorB) analogWrite(ENB, 0); // Para a roda motor B

  // Apenas sensor B na linha
  else if (!valorSensorA && valorSensorB) analogWrite(ENA, 0); // Para a roda motor A


// Detectou obstaculo desvia e volta pra rota

  if (distancia < (distanciaMinima - offsetDistancia)) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(250);
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    delay(500);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, velocidadeMotores + corrigeMotorA);
    analogWrite(ENB, velocidadeMotores + corrigeMotorB);
    delay(500);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, velocidadeMotores + corrigeMotorA);
    analogWrite(ENB, velocidadeMotores + corrigeMotorB);
    delay(500);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(500);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(500);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(500);
    while (valorSensorA) { //Verifica se ainda esta no obstaculo
      valorSensorA = digitalRead(pinoSensorA);
      valorSensorB = digitalRead(pinoSensorB);
    }

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(100);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(500);
  }

  // Atualiza valor da última leitura

  leituraAnteriorSensorA = valorSensorA;
  leituraAnteriorSensorB = valorSensorB;
  delay(50); // Delay p/ próxima leitura

}
