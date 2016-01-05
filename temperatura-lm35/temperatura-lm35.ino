/*
  Exemplo de Utilização LM 35 e LCD
  Utilizado: Arduino Uno, Display LCD 16 x 2 e LM 35
  Autor: Yhan Christian Souza Silva - Data: 29/12/2015
*/

#include <LiquidCrystal.h>              //biblioteca p/ uso do display LCD

// --- Hardware ---

#define sensorTemperatura A0            

// --- Definição pinos LCD ---

 LiquidCrystal lcd(7,
                   6,
                   5,
                   4,
                   3,
                   2);  


float temp;                         // Variavel p/ armazenar temperatura

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
  lcd.begin(16, 2);             //inicializo display LCD 16 x 2
  lcd.clear();                  //limpa o LCD
  lcd.createChar(0, grau);      //crio um caracterer customizado
}

// --- Loop Infinito ---

void loop()
{

// --- Variável temp armazena temperatura a partir da leitura analógica do sensor ---

  temp = (float(analogRead(sensorTemperatura)) * 5 / (1023)) / 0.01;

// --- Exibição do valor no display LCD ---

  lcd.setCursor(0, 0);
  lcd.print("- Temperatura -");
  lcd.setCursor(6, 1);
  lcd.print(temp, 1);
  lcd.setCursor(10, 1);
  lcd.write((byte)0);
  delay(2000);                //atraso p/ nova leitura
}
