/*
  Exemplo de Contador - Brincadeira Trabalho
  Utilizado: Arduino Uno, LCD e Push-button
  Autor: Yhan Christian Souza Silva - Data: 30/12/2015
*/


 #include <LiquidCrystal.h>             //biblioteca p/ uso do display LCD

 #define botao 10                       //defino o botão como pino 10

// --- Definição pinos LCD ---

 LiquidCrystal lcd(7,
                   6,
                   5,
                   4,
                   3,
                   2);   

int x;                                //variavel p/ incrementar contador


 // --- Configurações Iniciais ---

void setup() 
{
  lcd.begin(16,2);                   //inicializo display LCD 16 x 2 
  pinMode(botao, INPUT_PULLUP);      //defino botão como entrada, ativa Pull-up interno
}

// --- Loop Infinito ---

void loop() 
{ 
   lcd.setCursor(0,0);              //posiciona o cursor na linha 1 coluna 1 
   lcd.print("Contador Danilo");    //exibe mensagem no display
   lcd.setCursor(1,1);              //posiciona o cursor na linha 2 coluna 1
   lcd.print("Total: ");            //exibe mensagem no display

// --- Se botão for precionado a variavel é incrementada em 1 ---

   if(!digitalRead(botao))
   {
        x++;
        delay(250);
   }
   lcd.print(x);                  //exibição do valor no display
}



