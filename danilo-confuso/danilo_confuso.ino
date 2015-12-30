/* 
 *  Exemplo de Contator - Brincadeira da Empresa
 *  Utilizado: Arduino Uno, Push-Button e LCD
 *  Autor: Yhan Christian Souza Silva - Data: 29/12/2015
 */

 #include <LiquidCrystal.h>

 LiquidCrystal lcd(7,6,5,4,3,2);

#define botao 10
int x;

void setup() {
  lcd.begin(16,2);
  pinMode(botao, INPUT_PULLUP);
}

void loop() { 
   lcd.setCursor(0,0);
   lcd.print("Contador Danilo");
   lcd.setCursor(1,1);
   lcd.print("Total: ");
   if(!digitalRead(botao))
   {
        x++;
        delay(250);
   }
   lcd.print(x);
}



