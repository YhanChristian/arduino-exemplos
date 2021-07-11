//Bibliotecas Auxiliares

#include "sysuser.h"

/*Função: leituraEntradaDigital
  Realiza leitura de entrada digital 
  Parâmetros: inteiro: pino
  Retorno: booleano estado lógico
*/

bool leituraEntradaDigital(uint8_t pino)
{
    bool estado  = digitalRead(pino);
    return estado; 
}

/*Função: ligaSaidaDigital
  Escreve na saída digital o valor HIGH
  Parâmetros: inteiro: pino
  Retorno: nenhum
*/

void ligaSaidaDigital(uint8_t pino)
{
  digitalWrite(pino, HIGH);
}

/*Função: desligaSaidaDigital
  Escreve na saída digital o valor LOW
  Parâmetros: inteiro: pino
  Retorno: nenhum
*/
void desligaSaidaDigital(uint8_t pino)
{
  digitalWrite(pino, LOW);
}

/*Função: desabilitaSistema
  Função que chama ações que indicam sistema desabilitado
  Parâmetros: inteiro: nenhum
  Retorno: nenhum
*/

void desabilitaSistema()
{
  static unsigned long ultimoMillis;

  if((long)millis() - ultimoMillis > INTERVALO_MILLIS_01)
  {
    indicaSistemaDesabilitado();
    ultimoMillis = millis();
  }
}

/*Função: indicaSistemaDesabilitado
  Indica com LED_NIVEL_ALTO LED_NIVEL_BAIXO (piscando) de forma alternada
  Parâmetros: inteiro: pino
  Retorno: nenhum
*/

void indicaSistemaDesabilitado()
{
  digitalWrite(LED_NIVEL_ALTO, !digitalRead(LED_NIVEL_ALTO));
  digitalWrite(LED_NIVEL_BAIXO, !digitalRead(LED_NIVEL_BAIXO));
}

