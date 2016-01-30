/*
  Medindo distância por ultrasom - exibição no LCD
  Utilizado: Arduino Uno, Ultrasonico HC-SR04, LCD
  Autor: Yhan Christian Souza Silva - Data: 23/01/2015
*/

// --- Bibliotecas Auxiliares ---
#include <Ultrasonic.h>
#include <LiquidCrystal.h>

// --- Hardware ---
#define  trigger_pin 8
#define  echo_pin 9

// --- Definição pinos LCD ---
 LiquidCrystal lcd(7,
                   6,
                   5,
                   4,
                   3,
                   2);  

// --- Inicializo o sensor ultrasom ---
Ultrasonic ultrasonic(trigger_pin, echo_pin);

void setup()
{
	lcd.begin(16,2);		// Inicialização do display LCD 16 x 2 
	lcd.clear();			//Limpo o LCD
}

void loop()
{
	float cm, pol;						//Variáveis p/ leitura em cm e pol 
	long microsec = ultrasonic.timing();
	cm = ultrasonic.convert(microsec, Ultrasonic::CM);
	pol = ultrasonic.convert(microsec,Ultrasonic::IN);

// --- Exibição de valores no LCD ---

	lcd.setCursor(0,0);
	lcd.print("CM ");
	lcd.print(cm, 0);
	lcd.setCursor(1,1);
	lcd.print("POL ");
	lcd.print(pol,1);
	delay(1000);
}
