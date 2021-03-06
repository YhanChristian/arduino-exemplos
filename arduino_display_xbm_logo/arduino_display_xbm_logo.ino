/* Exibição de LOGO com Arduino e Display OLED SSD1309 128x64 (SPI)
   Utilizado: Arduino MEGA e Display OLED SSD1309
   Autor: Yhan Christian Souza Silva - Data: 25/10/2020
*/

// - Bibliotecas Auxiliares -

#include "U8glib.h"

// - Instância Objeto -

U8GLIB_SSD1309_128X64 u8g(52, 51, 53, 49, 48);

// - Define LOGO comprimento e altura -


#define logo_width 64
#define logo_height 64

// - Bits LOGO -

static char logo_bits[] = {
  0x00, 0x00, 0x00, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8,
  0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x3F, 0x00, 0x00,
  0x00, 0x00, 0x80, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF,
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0x7F, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x7F, 0x00, 0x00,
  0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF,
  0xDF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0x03, 0x3F, 0x00, 0x00,
  0x00, 0x00, 0xFC, 0xFF, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x0F,
  0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x3C, 0x00, 0x00,
  0x00, 0x00, 0x1E, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00,
  0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x38, 0x00, 0x00,
  0x00, 0x00, 0x0E, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00,
  0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x3F, 0xFC, 0x3F, 0x00, 0x00,
  0x00, 0x00, 0x3C, 0xF0, 0x07, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x1C, 0xE0,
  0x03, 0x38, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x40, 0x03, 0x18, 0x00, 0x00,
  0x00, 0x00, 0x0C, 0x40, 0x02, 0x28, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x20,
  0x06, 0x28, 0x00, 0x00, 0x00, 0x00, 0x14, 0x20, 0x04, 0x28, 0x00, 0x00,
  0x00, 0x00, 0x14, 0x30, 0x0C, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x60, 0x1C,
  0x38, 0x06, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x07, 0xE0, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x00,
};

// - Função para fazer logo na posição x,y definida -

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here
  u8g.drawXBM( 0, 0, logo_width, logo_height, logo_bits);
}

// - Função para exibir hello world -

void helloWorld(void) {
  // graphic commands to redraw the complete screen should be placed here
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(0, 20);
  // call procedure from base class, http://arduino.cc/en/Serial/Print
  u8g.print("Hello World!");
}
// - Setup -

void setup() {
  // put your setup code here, to run once:

}

// - Loop -

void loop(void) {
  // picture loop
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );

  // rebuild the picture after some delay
  delay(1000);



  u8g.firstPage();
  do {
    helloWorld();
  } while ( u8g.nextPage() );

  delay(1000);
}
