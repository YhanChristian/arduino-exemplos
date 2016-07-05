/* 
  Leitura de dois sensores via bluettoh
  Utilizado: Arduino Uno, LM35, LDR e Módulo Bluetooth HC-05
  Autor: Yhan Christian Souza Silva - Data: 04/07/2016
 */

#include <SoftwareSerial.h> //Biblioteca auxiliar

SoftwareSerial bluetooth(2, 3); //Defino RX pino 2 e pino 3
#define sensor_temperatura A0  //Sensor de temperatura liga ao pino A0
#define sensor_luminosidade A1  //Sensor de temperatura liga ao pino A1

void setup(){
	Serial.begin(9600);         //Inicio comunicação serial
	bluetooth.begin(115200);   // Inicio comunicação com módulo bluetooh, baud-rate 115200 
}

void loop(){
  // --- Variaveis para leitura de sensores ---
	int leitura_luminosidade = 0;
	float leitura_temperatura = 0;
	leitura_luminosidade = analogRead(sensor_luminosidade);
	leitura_temperatura = (float(analogRead(sensor_temperatura)) * 5 / (1023)) / 0.01;
  // --- Exibição via serial --- 
  	Serial.print("Luminosidade: ");
	Serial.println(leitura_luminosidade);
  	Serial.print("Temperatura: ");
	Serial.println(leitura_temperatura);
  // Print de dados módulo bluetooth ---
  	bluetooth.print("Luminosidade: ");
  	bluetooth.println(leitura_luminosidade);
  	bluetooth.print("Temperatura: ");
  	bluetooth.println(leitura_temperatura);
	delay(1000); //atraso de 1s para nova leitura
}
