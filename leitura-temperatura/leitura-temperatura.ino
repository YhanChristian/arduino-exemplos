/* 
 *  Exemplo de Utilização de Temperatura e LCD
 *  Utilizado: Arduino Uno, Display LCD 16 x 2 e sensor DHT 11
 *  Autor: Yhan Christian Souza Silva - Data: 29/12/2015
 */
 
#include <LiquidCrystal.h>
#include <DHT.h>

#define pinoSensor A3 
#define sensorTemperatura DHT11

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
DHT dht(pinoSensor, sensorTemperatura);

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
  //variavéis p/ leitura de umidade e temperatura
	float humid = dht.readHumidity();
	float temp = dht.readTemperature();

	//exibição temperatura display lcd
	lcd.setCursor(0,0);
	lcd.print("Temp.: ");
	lcd.setCursor(7,0);
	lcd.print(temp, 1);
	lcd.setCursor(12,0);
	lcd.write((byte)0);

	//exibição umidade display lcd
	lcd.setCursor(0,1);
	lcd.print("Umid.: ");
	lcd.setCursor(7,1);
	lcd.print(humid,1);
	lcd.setCursor(12,1);
	lcd.print("%");

	//Intervalo para nova leitura sensor
	delay(2000);

}
