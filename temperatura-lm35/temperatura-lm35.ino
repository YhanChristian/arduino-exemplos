/* 
 *  Exemplo de Utilização LM 35 e LCD
 *  Utilizado: Arduino Uno, Display LCD 16 x 2 e LM 35
 *  Autor: Yhan Christian Souza Silva - Data: 29/12/2015
 */

#include <LiquidCrystal.h>

 #define sensorTemperatura A0

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
 
 // Variavel p/ armazenar temperatura
 float temp;

//Array com simbolo de grau
byte grau[8] = {0x0C,0x12,0x12,0x0C,0x00,0x00,0x00,0x00};

 void setup()
 {
 	lcd.begin(16,2);
 	lcd.clear();
 	lcd.createChar(0, grau);
 }

 void loop()
 {
 	temp = (float(analogRead(sensorTemperatura)) * 5 / (1023)) / 0.01;
  // exibição de valores no display lcd
 	lcd.setCursor(0,0);	
 	lcd.print("Temperatura");
 	lcd.setCursor(0,1);
 	lcd.print(temp,1);
 	lcd.setCursor(7,1);
 	lcd.write((byte)0);
  delay(2000); //Delay p/ nova leitura
 }
