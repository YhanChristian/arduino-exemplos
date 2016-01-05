/*
  Exemplo de Utilização BCD e LDR - Contagem de acionamentos 0 a 9
  Utilizado: Arduino Uno, BCD 7 seg e LDR
  Autor: Yhan Christian Souza Silva - Data: 30/12/2015
*/

#define ldrSensor 0
#define carga 9
#define valorEscuro 400
#define valorClaro 600

int incremento = 0, leituraSensor = 0; //declaração de variaveis 


// --- Array c/ dígitos 7 segmentos ---

byte segmento[10] = { B01111110,    //  Digito 0   
                      B00001100,    //  Digito 1   
                      B10110110,    //  Digito 2   
                      B10011110,    //  Digito 3   
                      B11001100,    //  Digito 4   
                      B11011010,    //  Digito 5   
                      B11111010,    //  Digito 6   
                      B00001110,    //  Digito 7   
                      B11111110,    //  Digito 8   
                      B11001110, }; //  Digito 9     
 

 // --- Configurações Iniciais ---

void setup()
{
  Serial.begin(9600);                                   //inicio comunicação serial                    
	for(int i=2; i<=9; i++) pinMode(i, OUTPUT);          //defino os pinos 2,3,4,5,6,7,8,9 como saída 
}

// --- Loop Infinito ---

void loop()
{
	leituraSensor = analogRead(ldrSensor);           //leitura do sensor e armazenamento na varíval leituraSensor
	Serial.println(leituraSensor);                   //exibição do valor no monitor serial

// --- Verifica se o valor de leitura é menor que o definido como escuro p/ acionamento da carga ---

  if(leituraSensor < valorEscuro)                
	{
		  digitalWrite(carga, HIGH);                      //acionamento da carga
      incremento++;                                   //variavel incrementa valor 
      if (incremento > 9) incremento = 0;             //condição para zerar contador
  }

// --- Condição para desligar a carga ---

	else if (leituraSensor >= valorClaro)	digitalWrite(carga, LOW);                        

 // --- Loop p/ evitar incrementos indevidos no contador ---     
    
  while(analogRead(ldrSensor)> valorEscuro) PORTD = segmento[incremento]; 
  delay(50);                                        //atraso p/ nova leitura
}


