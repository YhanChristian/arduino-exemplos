/** =============================================================================================================
  Projeto: Controle de saída PWM (bomba submersível) e monitoramento do nível
  Utilizado: Arduino, Leds de Indicação, Sensor de nível, chave ON/OFF, módulo TRIAC
  @file main.cpp
  @author Yhan Christian Souza Silva
  @date  15/10/2021
  @version V.1.0.0

          **DESCRIÇÃO DE FUNCIONAMENTO DO FIRMWARE**
  - O sistema após inicialização fará o acionamento do TRIAC via PWM para controle do fluxo de água da bomba 
  submersível, é colocado um sensor de nível que identifica o baixo nível e o LED_BAIXO_NIVEL será aceso e 
  a bomba desabilitada. Versão Simplificada Arduino

  Aplicação do Projeto: Pessoal, para controle do fluxo de água do bebedouro dos gatos. 
  ============================================================================================================== */

/* Includes ------------------------------------------------------------------*/

#include <Arduino.h>
#include "sysconfig.h"

/* Private variables ---------------------------------------------------------*/

/* Private ISR's (Interrupt Service Routine) ---------------------------------*/

/* Private functions ---------------------------------------------------------*/

void inicia_io();


/* Bodie of setup function ---------------------------------------------------*/

void setup()
{
  //Configura IO - Verifica erros 
  inicia_io();


}


/* Bodie of loop function ----------------------------------------------------*/

void loop()
{
  // put your main code here, to run repeatedly:
}

/*Bodie of private ISR's (Interrupt Service Routine) --------------------------*/



/* Bodies of private functions -----------------------------------------------*/

void inicia_io()
{
  //Configura entradas
  for(uint8_t i = 2; i < 5; i++)
  {
    pinMode(i, INPUT_PULLUP);
  }

  //Configura saídas e inicia com todas desligadas

  for(uint8_t i = 6; i < 11; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

}

/*****************************END OF FILE**************************************/
