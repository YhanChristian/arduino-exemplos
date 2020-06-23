/* Filtro Média Móvel - Exemplo de Algoritmo
   Hardware: Arduino UNO, Potenciometro 10k
   Autor: Yhan Christian Souza Silva - Data: 23/06/2020
*/

//============================= Hardware e Defines ==================================

#define POT_PIN     A0

#define BAUD_RATE   9600

#define N           10 //Número de pontos média

#define INTERVALO   100


//============================ Variáveis e Constantes ================================

uint16_t leituraSensor, leituraFiltrada, leituras[N];

long ultimaLeituraSensor;

//========================= Protótipo Funções Auxiliares =============================

long mediaMovel();

//================================== Setup ==========================================

void setup() {
  Serial.begin(BAUD_RATE);

}

//================================== Loop ===========================================

void loop() {
  if (millis() - ultimaLeituraSensor > INTERVALO) {
    leituraSensor = analogRead(POT_PIN);
    leituraFiltrada = mediaMovel();
    Serial.print(leituraSensor);
    Serial.print(" ");
    Serial.println(leituraFiltrada);
    ultimaLeituraSensor = millis();
  }
}


//============================ Funções Auxiliares ====================================

//Calcula média móvel e retorna valor long

long mediaMovel() {

  //Desloca vetor atribuindo leituras

  for (uint8_t i = N - 1; i > 0; i--) {
    leituras[i] = leituras[i - 1];
  }

  //Atribui a primeira posição do vetor a leituraSensor (raw)

  leituras[0] = leituraSensor;

  long acumulador = 0;

  //Realiza somatória do número de pontos

  for (uint8_t i = 0; i < N; i++) {
    acumulador += leituras[i];
  }

  //Retorna média móvel

  return acumulador / N;

}
