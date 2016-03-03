/*
  Exemplo de Utilização Sensor de Corrente SCT 013-000
  Utilizado: Arduino Uno, Sensor de Corrente SCT 013-000
  Autor: Yhan Christian Souza Silva - Data: 30/01/2016
*/

#include "EmonLib.h"            //biblioteca auxiliar

#define sensorCorrente A1      //definição pino sensor

EnergyMonitor emon1;

int tensao = 220;             //variavél definindo tensão da rede, caso seja 110, mude o valor

void setup(){
	Serial.begin(9600);                   //Inicio comunicação serial
	emon1.current(sensorCorrente, 60);     //Configuração sensor, baseado no http://tyler.anairo.com/projects/open-energy-monitor-calculator
}

void loop(){
	float irms = emon1.calcIrms(1480);    //Realiza o calculo da corrente 
	Serial.print("Corrente: ");           
	Serial.println(irms);                //Exibição valor corrente 
	Serial.print("Potencia: ");
	//Condição para calculo de potência, corrente menor que 5mA,potencia = 0
  if (irms < 0.005) Serial.println(0);
	else Serial.println(tensao * irms);
	delay(1000);                      
}



