/* Exemplo de utilização de Matrix 8x8 de Led com MAX7219
 * Utilizado: Arduino Uno e Matriz 8x8 de Led
 * Autor: Yhan Christian Souza Silva - Data: 06/05/2017
 */

// -- Biblioteca auxiliar --

#include <LedControl.h>

#define din  12
#define clk  11
#define cs   10

// -- Cria-se objeto lc, com 1 matriz led --

LedControl lc= LedControl(din,clk,cs, 1);

// -- Variáveis globais -- 

int i = 0;

// -- Cria-se array com caracteres E,C e um coração --
const byte IMAGES[][8] = {
{
  B00000000,
  B01111110,
  B01100000,
  B01100000,
  B01111100,
  B01100000,
  B01100000,
  B01111110
},{
  B00000000,
  B00111100,
  B01100110,
  B01100000,
  B01100000,
  B01100000,
  B01100110,
  B00111100
},{
  B00000000,
  B00100010,
  B01110111,
  B01111111,
  B01111111,
  B00111110,
  B00011100,
  B00001000
}};

const int IMAGES_LEN = sizeof(IMAGES)/8;


// -- Configurações iniciais do display -- 

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
}

// -- Função para exibição de caracteres --

void displayImage(const byte* image) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, bitRead(image[i], 7 - j));
    }
  }
}


// -- Loop infinito c/ delay para troca de caracteres de 500ms --

void loop() {
  displayImage(IMAGES[i]);
  if (++i >= IMAGES_LEN ) {
    i = 0;
  }
  delay(500);
}


