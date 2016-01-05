/* 
  Exemplo de Utilização de Temperatura e LCD
  Utilizado: Arduino Uno, Display LCD 16 x 2 e sensor DHT 11
  Autor: Yhan Christian Souza Silva - Data: 29/12/2015
 */
 
#include <LiquidCrystal.h>							//biblioteca display LCD
#include <DHT.h>									      //biblioteca sensor de temperatura DHT 11

// --- Hardware ---

#define pinoSensor A3 							   
#define sensorTemperatura DHT11

// --- Definição pinos LCD ---

 LiquidCrystal lcd(7,
                   6,
                   5,
                   4,
                   3,
                   2);   

DHT dht(pinoSensor, sensorTemperatura);			

// --- Array símbolo grau ---

byte grau[8] = {0x0C,
				        0x12,
				        0x12,
				        0x0C,
				        0x00,
				        0x00,
				        0x00,
				        0x00};

 // --- Configurações Iniciais ---

void setup()
{
	lcd.begin(16,2);				    //inicializo display LCD 16 x 2
	lcd.clear();				      	//limpa o LCD
	lcd.createChar(0, grau);		//crio um caracterer customizado
}

// --- Loop Infinito ---

void loop()
{
  
// --- Variáveis p/ leitura de temperatura e umidade ---
	
	float humid = dht.readHumidity();
	float temp = dht.readTemperature();

// --- Exibição de temperatura no LCD ---

	lcd.setCursor(0,0);
	lcd.print("Temp.: ");
	lcd.setCursor(7,0);
	lcd.print(temp, 1);
	lcd.setCursor(12,0);
	lcd.write((byte)0);

// --- Exibição de umidade no LCD ---

	lcd.setCursor(0,1);
	lcd.print("Umid.: ");
	lcd.setCursor(7,1);
	lcd.print(humid,1);
	lcd.setCursor(12,1);
	lcd.print("%");
	delay(2000);				//intervalo p/ nova leitura do sensor
}
