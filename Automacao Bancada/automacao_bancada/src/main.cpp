/**
  ******************************************************************************
  * @Company    : Yhan Christian Souza Silva  MEI
  * @file       : main.cpp
  * @author     : Yhan Christian Souza Silva
  * @version	: 1.0.1
  * @date       : 17/10/2021
  * @brief      : Arquivo principal projeto de automação bancada de trabalho.
  *               Utilizado Arduino UNO, display Nextion e 4 saídas a Relé
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include "main.h"


/* Bodie of setup function ---------------------------------------------------*/

void setup()
{
  //Inicia - Config IO
  init_io();

  //Inicia - Nextion
  init_nextion();

  //Habilita WDT
  enable_wdt();
}

/* Bodie of loop function ----------------------------------------------------*/

void loop()
{
  //Feedback WDT - Watch Dog Timer
  wdt_reset();

  // NexLoop escuta eventos Callback
  nexLoop(nex_listen_list);
}

/* Bodies of private functions -----------------------------------------------*/

/**
  * @Func       : init_io   
  * @brief      : Configura Relés de saída e inicia com todos desligados
  * @parameters : Nenhum
  * @retval     : Nenhum
  */
void init_io()
{
  //Defino pinos relés como saída e inicio todos DESLIGADOS
  for (uint8_t uiPin = 8; uiPin < 12; uiPin++)
  {
    pinMode(uiPin, OUTPUT);
    digitalWrite(uiPin, HIGH);
  }
}

/**
  * @Func       : init_nextion
  * @brief      : Configura Serial e Display Nextion (inicia e funções callback)
  * @parameters : Nenhum
  * @retval     : Nenhum
  */
void init_nextion()
{
  //Inicia Serial
  Serial.begin(BAUD_RATE);
  //Inicia Comunicação Disp Nextion
  nexInit();

  //Eventos Callback Display
  bt0.attachPop(bt0_callback, &bt0);
  bt1.attachPop(bt1_callback, &bt1);
  bt2.attachPop(bt2_callback, &bt2);
  bt3.attachPop(bt3_callback, &bt3);
}

/**
  * @Func       : enable_wdt
  * @brief      : Configura WDT com tempo de estou de 4s
  * @parameters : Nenhum
  * @retval     : Nenhum
  */
void enable_wdt()
{
  wdt_enable(WDTO_4S);
}

/**
  * @Func       : bt0_callback
  * @brief      : Evento Callback 
  * @parameters : *ptr aponta para endereço bt correspondente
  * @retval     : Nenhum
  */
void bt0_callback(void *ptr)
{
  uint32_t uiBotao;
  bt0.getValue(&uiBotao);

  if (uiBotao == 1)
  {
    digitalWrite(RELE_FONTE, LOW);
  }

  else
  {
    digitalWrite(RELE_FONTE, HIGH);
  }
}

/**
  * @Func       : bt1_callback
  * @brief      : Evento Callback 
  * @parameters : *ptr aponta para endereço bt correspondente
  * @retval     : Nenhum
  */
void bt1_callback(void *ptr)
{
  uint32_t uiBotao;
  bt1.getValue(&uiBotao);

  if (uiBotao == 1)
  {
    digitalWrite(RELE_TRAFO, LOW);
  }

  else
  {
    digitalWrite(RELE_TRAFO, HIGH);
  }
}

/**
  * @Func       : bt2_callback
  * @brief      : Evento Callback 
  * @parameters : *ptr aponta para endereço bt correspondente
  * @retval     : Nenhum
  */
void bt2_callback(void *ptr)
{
  uint32_t uiBotao;
  bt2.getValue(&uiBotao);

  if (uiBotao == 1)
  {
    digitalWrite(RELE_ILUMINACAO, LOW);
  }

  else
  {
    digitalWrite(RELE_ILUMINACAO, HIGH);
  }
}

/**
  * @Func       : bt3_callback
  * @brief      : Evento Callback 
  * @parameters : *ptr aponta para endereço bt correspondente
  * @retval     : Nenhum
  */
void bt3_callback(void *ptr)
{
  uint32_t uiBotao;
  bt3.getValue(&uiBotao);

  if (uiBotao == 1)
  {
    digitalWrite(RELE_TESTE, LOW);
  }

  else
  {
    digitalWrite(RELE_TESTE, HIGH);
  }
}

/*****************************END OF FILE**************************************/
