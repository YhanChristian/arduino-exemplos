
/*
  I LOVE YOU  - Brincadeira com Arduino
  Autor: Yhan Christian Souza Silva
  Utilizado: Arduino Uno, Display Nokia 5110
*/

// -- Bibliotecas Auxiliares --

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <SPI.h>
#include "images.h"

// -- Instancia do objeto myDisplay --

Adafruit_PCD8544 myDisplay = Adafruit_PCD8544(A0, A1, A2, A3, A4);


void setup() {
  myDisplay.clearDisplay();
  myDisplay.begin();
  myDisplay.setContrast(48);
  myDisplay.setTextColor(BLACK);
  myDisplay.setTextSize(2);

}

void loop() {
  myDisplay.setCursor(10, 0);
  myDisplay.println("EU AMO");
  myDisplay.setCursor(30, 20);
  myDisplay.println("VC");
  myDisplay.display();
  delay(1000);
  myDisplay.clearDisplay();
  myDisplay.setCursor(10, 0);
  myDisplay.println("CASA");
  myDisplay.setCursor(10, 20);
  myDisplay.println("CMG ?");
  myDisplay.display();
  delay(1000);
  myDisplay.clearDisplay();

  for (int i = 0; i < 10; i++) {

    myDisplay.drawBitmap(0, 0, smile01, 84, 48, 1);
    myDisplay.display();
    delay(300);
    myDisplay.clearDisplay();
    myDisplay.drawBitmap(0, 0, smile02, 84, 48, 1);
    myDisplay.display();
    delay(300);
    myDisplay.clearDisplay();
    myDisplay.drawBitmap(0, 0, smile03, 84, 48, 1);
    myDisplay.display();
    delay(300);
    myDisplay.clearDisplay();
    myDisplay.drawBitmap(0, 0, smile04, 84, 48, 1);
    myDisplay.display();
    delay(300);
  }
  myDisplay.clearDisplay();
}
