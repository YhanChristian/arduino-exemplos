/*
	Exemplo de Contador - Brincadeira Trabalho
	Utilizado: Arduino Uno, LCD e Push-button
	Autor: Yhan Christian Souza Silva - Data: 30/12/2015
*/

#include <LiquidCrystal.h>					// Biblioteca p/ uso do display LCD
#define botao 10								// Definição do botão como pino 10

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);   // Definição dos pinos LCD

int x;											// Variável de controle


// Configurações iniciais
void setup() {
	lcd.begin(16,2);							// Inicialização do display LCD 16 x 2 
	pinMode(botao, INPUT_PULLUP);			// Definição do botão como entrada, ativação pull-up interno
}

// Loop infinito
void loop() { 
   lcd.setCursor(0,0);						// Posiciomanento do cursor na linha 1 e coluna 1 
   lcd.print("Contador Danilo");			// Exibição de mensagem no display
   lcd.setCursor(1,1);						// Posiciomanento do cursor na linha 2 e coluna 1
   lcd.print("Total: ");					// Exibição de mensagem no display

	// Caso o botão seja pressionado a variável "x" é incrementada
	if(!digitalRead(botao)) {
		x++;
		delay(250);
	}

	lcd.print(x);								// Exibição do valor de "x" no display
}
