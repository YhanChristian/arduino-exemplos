/** =============================================================================================================
  Projeto: Controle de saída PWM (bomba submersível) e monitoramento do nível
  Utilizado: NodeMCU, Leds de Indicação, Sensor de nível, chave ON/OFF, módulo TRIAC
  @file main.cpp
  @author Yhan Christian Souza Silva
  @date  03/04/2021
  @version V.1.0.0

          **DESCRIÇÃO DE FUNCIONAMENTO DO FIRMWARE**
  - O sistema após inicialização fará o acionamento do TRIAC via PWM para controle do fluxo de água da bomba 
  submersível, é colocado um sensor de nível que identifica o baixo nível e o LED_BAIXO_NIVEL será aceso e 
  a bomba desabilitada, além disso, o usuário receberá uma notificação em app android. 
  - Inicialmente a comunicação será via HTTP, entre o NodeMCU e a base de dados (Base de dados será o Firebase)
  - Projeto em desenvolvimento e mudanças podem ocorrer, de acordo com as necessidades.

  Aplicação do Projeto: Pessoal, para controle do fluxo de água do bebedouro dos gatos. 
  ============================================================================================================== */

//Biblioteca Auxiliares

#include "main.h"

//Variáveis e Constantes

bool zeroCrossDetector = false;


//Setup

void setup()
{
  //Desabilita WDT
  desabilitaWDT();

  //Configura pinos de IO
  iniciaIOs();

  //Configura comunicação Serial
  iniciaSerial();

  //Inicia comunicação WiFi
  conectaWiFi();

  //Configura interrupção D2 = ZERO CROSS DETECTOR

  configInterrupt();

  //Habilita WDT 1000ms (ou seja 1s)
  habilitaWDT();
}

//Loop
void loop()
{
  bool leHabilitaSistema = leituraEntradaDigital(HABILITA_SISTEMA);

  if (!leHabilitaSistema)
  {
    bool leSensorNivel = leituraEntradaDigital(SENSOR_NIVEL);

    if (leSensorNivel)
    {
      ligaSaidaDigital(LED_NIVEL_ALTO);
      desligaSaidaDigital(LED_NIVEL_BAIXO);
    }

    else
    {
      ligaSaidaDigital(LED_NIVEL_BAIXO);
      desligaSaidaDigital(LED_NIVEL_ALTO);
    }
  }
  else
  {
    desabilitaSistema();
  }

  //Alimenta WDT atualizando sistemaT
   yield();
  ESP.wdtFeed();
}

/*Função: trataISR
  Tratamento interrupçao ZeroCross Detector TRIAC, tornando o bit zeroCrossDetector = 1; 
  Parâmetros: nenhum
  Retorno: nenhum
*/
ICACHE_RAM_ATTR void trataISR()
{
  zeroCrossDetector = true; 
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("O pai ta on!");
}