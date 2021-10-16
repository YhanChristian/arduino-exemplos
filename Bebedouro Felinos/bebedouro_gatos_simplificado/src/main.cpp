/** =============================================================================================================
  Projeto: Controle de saída PWM (bomba submersível) e monitoramento do nível
  Utilizado: Arduino, Leds de Indicação, Sensor de nível, chave ON/OFF, módulo TRIAC
  @file main.cpp
  @author Yhan Christian Souza Silva
  @date  15/10/2021
  @version V.1.0.0

          **DESCRIÇÃO DE FUNCIONAMENTO DO FIRMWARE**
  - O sistema após inicialização fará o acionamento da SAIDA_TRIAC via PWM para controle do fluxo de água da bomba 
  submersível, é colocado um sensor de nível que identifica o baixo nível e o LED_NIVEL_BAIXO será aceso e 
  a bomba desabilitada. Versão Simplificada Arduino

  Aplicação do Projeto: Pessoal, para controle do fluxo de água do bebedouro dos gatos. 
  ============================================================================================================== */

/* Includes ------------------------------------------------------------------*/

#include <Arduino.h>
#include "sysconfig.h"

/* Private variables ---------------------------------------------------------*/


/* Private ISR's (Interrupt Service Routine) ---------------------------------*/

void trata_isr_sensor();
void trata_isr_zc();


/* Function Pointer FSM - Finite State Machine -------------------------------*/

void (*pvMaquinaEstados)();


/* Private functions ---------------------------------------------------------*/

void inicia_io();
void config_interrupcao();
void estado_inicial();
void estado_normal();
void estado_alarme();


/* Bodie of setup function ---------------------------------------------------*/

void setup()
{
  //Configura IO - Verifica erros 
  inicia_io();

  //Configura pinos - interrupção
  config_interrupcao();

}


/* Bodie of loop function ----------------------------------------------------*/

void loop()
{
  // put your main code here, to run repeatedly:
}

/*Bodie of private ISR's (Interrupt Service Routine) --------------------------*/

void trata_isr_sensor()
{

}

void trata_isr_zc()
{

}

/* Bodies of private functions -----------------------------------------------*/

void inicia_io()
{
  //Configura entradas - input pullup
  for(uint8_t i = 2; i < 5; i+=2)
  {
    pinMode(i, INPUT_PULLUP);
  }

  //Configura entrada - ZC
  pinMode(ZERO_CROSS, INPUT);
  
  //Configura saídas e inicia com todas desligadas
  for(uint8_t i = 6; i < 11; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

}

void config_interrupcao()
{
  //Configura interrupção sensor nível
  attachInterrupt(digitalPinToInterrupt(SENSOR_NIVEL), trata_isr_sensor, FALLING);

  //Configura interrupção zero cross módulo triac
  attachInterrupt(digitalPinToInterrupt(ZERO_CROSS), trata_isr_zc, RISING);

}

/*****************************END OF FILE**************************************/
