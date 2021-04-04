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

#include <Arduino.h>
#include "sysconfig.h"

void setup()
{
  //Configura pinos de IO
  iniciaIOs();

  //Configura comunicação Serial
  iniciaSerial();

}

void loop()
{
  
}