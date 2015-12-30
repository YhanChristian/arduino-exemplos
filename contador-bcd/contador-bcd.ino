/*
    Exemplo de Utilização BCD e LDR - Contagem de acionamentos 0 a 9
    Utilizado: Arduino Uno, BCD 7 seg e LDR
    Autor: Yhan Christian Souza Silva - Data: 30/12/2015
*/

#define ldrSensor 0
#define carga 9
#define valorEscuro 400
#define valorClaro 600

int incremento = 0, leituraSensor = 0;

byte segmento[10] = {   
B01111110,  // = Digito 0   
B00001100,  // = Digito 1   
B10110110,  // = Digito 2   
B10011110,  // = Digito 3   
B11001100,  // = Digito 4   
B11011010,  // = Digito 5   
B11111010,  // = Digito 6   
B00001110,  // = Digito 7   
B11111110,  // = Digito 8   
B11001110,  // = Digito 9   
}; 
 
void setup()
{
  Serial.begin(9600);
 digitalWrite(carga, HIGH);
	for(int i=2; i<=9; i++)
		pinMode(i, OUTPUT);
}

void loop()
{
  //Leitura do sensor e armazenamento do valor na variavel, exibição do valor no monitor serial
	leituraSensor = analogRead(ldrSensor);
	Serial.println(leituraSensor);
  //Condição p/ acionamento da carga e incremento no display
  if(leituraSensor < valorEscuro)
	{
		  digitalWrite(carga, HIGH);
      incremento++;
      while(analogRead(ldrSensor)!= valorEscuro)
      {
         PORTD = segmento[incremento];
      }
      if (incremento > 9)
      {
          incremento = 0;
      }
  }
 	//Condição p/ desligar a carga
	else if (leituraSensor >= valorClaro)
	{
		digitalWrite(carga, LOW);
	}
 	delay(50);
}


