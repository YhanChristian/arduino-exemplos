/*
  Robô Seguidor de Linha (linha reta 1 sensor)
  Utilizado: Arduino Uno, Ponte H L298N, sensor TCRT5000
  Autor: Yhan Christian Souza Silva - Data: 05/07/2019
*/


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

// -- Variáveis e Constantes --

// Velocidade Constante Motores

const int velocidadeMotores = 85;

// Correção de Velocidade
const int corrigeMotorA = 0;
const int corrigeMotorB = 0;

//Guarda leitura anterior sensores

bool leituraAnteriorSensorA = false;


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
}

// -- Loop --

void loop() {

  bool valorSensorA = digitalRead(pinoSensorA);

  // Sensor fora da linha
  if (!valorSensorA) {
    valorSensorA = leituraAnteriorSensorA;
  }

  // Sensor na linha
  else if (valorSensorA) {
    analogWrite(ENA, velocidadeMotores + corrigeMotorA);
    analogWrite(ENB, velocidadeMotores + corrigeMotorB);
  }
  leituraAnteriorSensorA = valorSensorA;

  delay(50); //Delay para próxima leitura
}
