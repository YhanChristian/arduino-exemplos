//Bibliotecas Auxiliares 

#include "sysconfig.h"


/*Função: iniciaIOs
  Configura Entradas e Saídas e inicia com todos os pinos desligados 
  Parâmetros: nenhum
  Retorno: nenhum
*/

void iniciaIOs()
{
  //Declara Entradas 
  pinMode(SENSOR_NIVEL, INPUT_PULLUP);
  pinMode(HABILITA_SISTEMA, INPUT_PULLUP);
  //Declara Saídas

  pinMode(LED_NIVEL_ALTO, OUTPUT);
  pinMode(LED_NIVEL_BAIXO, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);

  //Inicia  saídas em nível lógico baixo
  digitalWrite(LED_NIVEL_ALTO, LOW);
  digitalWrite(LED_NIVEL_BAIXO, LOW);
  digitalWrite(LED_STATUS, LOW);

}


/*Função: iniciaSerial
  Inicia comunicação serial 
  Parâmetros: nenhum
  Retorno: nenhum
*/

void iniciaSerial()
{
    Serial.begin(BAUD_RATE);
}

/*Função: conectaWiFi
  Verifica conexão WiFi e chamada modo AP em caso de falha de conexão 
  Parâmetros: nenhum
  Retorno: nenhum
*/

void conectaWiFi()
{

}

/*Função: configModeCallback
  Entra no modo AP e realiza configuração WiFi
  Parâmetros: instância WiFiManager
  Retorno: nenhum
*/

void configModeCallback(WiFiManager *myWiFiManager)
{

}

/*Função: tick
  Função para piscar o LED
  Parâmetros: inteiro: pino LED a ser utilizada função piscaLED
  Retorno: nenhum
*/

void tick(uint8_t pino)
{
    digitalWrite(pino, !digitalRead(pino));
}