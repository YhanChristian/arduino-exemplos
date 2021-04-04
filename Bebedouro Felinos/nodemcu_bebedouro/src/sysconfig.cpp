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
  digitalWrite(LED_NIVEL_ALTO, HIGH);
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
  // Instância Objetos

  Ticker ticker;

  WiFiManager wifiManager;

  //Pisca LED indicando LED_STATUS

  ticker.attach(0.5, tick);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //Chama modo AP para configuração WiFi
  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect("Agua Felinos", "12345678"))
  {
    Serial.println("Falha de conexão WiFi");
  }

  //Conexão realizada com sucesso
  Serial.println("Conectado com sucesso");

  //Desabilita o ticker e mantém led status ligado
  ticker.detach();

  digitalWrite(LED_STATUS, HIGH);
}

/*Função: configModeCallback
  Entra no modo AP e realiza configuração WiFi
  Parâmetros: instância WiFiManager
  Retorno: nenhum
*/

void configModeCallback(WiFiManager *myWiFiManager)
{
  // Instância Objetos

  Ticker ticker;

  Serial.println("Entrou no modo AP - Gentileza Acessar");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());

  //LED_STATUS pisca mais rápido indicando modo de configuração
  ticker.attach(0.2, tick);
}

/*Função: tick
  Função para piscar o LED_STATUS
  Parâmetros: nenhum
  Retorno: nenhum
*/

void tick()
{
  digitalWrite(LED_STATUS, !digitalRead(LED_STATUS));
}