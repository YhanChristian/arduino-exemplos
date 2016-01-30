/* 
  Exemplo simples -  Bluetooth e Arduino
  Utilizado: Arduino Uno, Módulo Bluetooth HC-05
  Autor: Yhan Christian Souza Silva - Data: 29/01/2015
 */


#include <SoftwareSerial.h>  //Biblioteca Auxiliar 

#define redLed 13          

SoftwareSerial mySerial(6, 7); // pino 6 RxD e pino 7 TxD

char var;

 // --- Configurações Iniciais ---

void setup() {
  pinMode (redLed, OUTPUT);
  digitalWrite(redLed , LOW);
  mySerial.begin(38400);
}

void loop() {

  // Verifica se há dados para ler 
 while(mySerial.available() > 0) {
  var = mySerial.read(); 
 }
 //Condição para Ligar ou desligar carga
 if(var == 'L') digitalWrite(redLed,HIGH);
 if(var == 'D') digitalWrite(redLed,LOW);
 delay(500);
}

