/*
  Exemplo de Utilização de Menus com LCD
  Utilizado: Arduino Uno, Display LCD 16 x 2 , Push Button e Sensores
  Autor: Yhan Christian Souza Silva - Data: 27/02/2018
*/

#include <LiquidCrystal.h>              //biblioteca p/ uso do display LCD

// -- Hardware --

#define sensorTemperatura A0
#define sensorLuminosidade A1
#define pinoBotao 9

// -- Definição pinos LCD --

LiquidCrystal lcd(7,
                  6,
                  5,
                  4,
                  3,
                  2);

// -- Variáveis --

float leituraTemperatura, leituraLuminosidade;
int menu = 0;
boolean estadoBotao;

// -- Setup --

void setup() {
  pinMode(pinoBotao, INPUT_PULLUP);
  digitalWrite(pinoBotao, HIGH);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pressione o botão");
  lcd.setCursor(0, 1);
  lcd.print("para ver sensor");
  delay(250);

}

// -- Loop --

void loop() {

/* realiza leitura de temperatura e luminosidade
 *  verifica se o botão foi pressionado incrementando a variavel menu
 *  caso 1 - exibe temperatura (menu 1)
 *  caso 2 - exibe luminosidade (menu 2)
 *  default - volta a tela inicial e pede para pressionar botão novamente.
 * 
 */
  estadoBotao = digitalRead(pinoBotao);
  leituraTemperatura = (float(analogRead(sensorTemperatura)) * 5 / (1023)) / 0.01;
  leituraLuminosidade = analogRead(sensorLuminosidade);
  if (!estadoBotao) menu++;

  switch (menu) {
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Temperatura");
      lcd.setCursor(0, 1);
      lcd.print(leituraTemperatura);
      delay(250);
      break;

    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Luminosidade");
      lcd.setCursor(0, 1);
      lcd.print(leituraLuminosidade);
      delay(250);
      break;

    default:
      lcd.print("Pressione o botão");
      lcd.setCursor(0, 1);
      lcd.print("para ver sensor");
      delay(250);
      menu = 0;
      break;
  }
}
