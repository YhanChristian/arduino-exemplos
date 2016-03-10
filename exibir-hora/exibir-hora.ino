/* 
  Exemplo: Relógio com Arduíno
  Utilizado: Arduino Uno, RTC DS 1307, Display LCD
  Autor: Yhan Christian Souza Silva - Data: 31/01/2016
 */

 // --- Bibliotecas Auxiliares---

#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2); //Definição pinos LCD

RTC_DS1307 RTC;                    //Cria o objeto RTC 

 // --- Configurações Iniciais ---

void setup() {
	Serial.begin(9600);            
	Wire.begin();                   
	RTC.begin();                    
	lcd.begin(16,2);
  
 // --- Condição p/ testar funcionamento RTC ---
 
	if(! RTC.isrunning()) {
		Serial.println("RTC não está funcionando");
		RTC.adjust(DateTime(__DATE__, __TIME__));       //Configuração Data e hora, comente após ser configurado
	}
}

// --- Loop infinito para visualiar as informações no LCD

void loop() {
	DateTime now = RTC.now();
	lcd.setCursor(0, 0);
  	lcd.print("Data: ");
	lcd.print(now.day(), DEC);
	lcd.print("/");
	lcd.print(now.month(), DEC);
  lcd.print("/");
	lcd.print(now.year(), DEC);
	lcd.print(" ");
	lcd.setCursor(0, 1);
	  lcd.print("Hora: ");
  
	if(now.hour() < 10 ) lcd.print("0");
	lcd.print(now.hour(), DEC);
	lcd.print(":");

	if(now.minute() < 10) lcd.print("0");
	lcd.print(now.minute(), DEC);
	lcd.print(":");

	if(now.second() < 10) lcd.print("0");
	lcd.print(now.second(), DEC);

	delay(1000);
}
