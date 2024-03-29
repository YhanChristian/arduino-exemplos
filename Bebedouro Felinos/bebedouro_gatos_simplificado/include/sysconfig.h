/**
  ******************************************************************************
  * @Company    : Yhan Christian Souza Silva 
  * @file       : config.h
  * @author     : Yhan Christian Souza Silva
  * @date       : 15/10/2021
  * @brief      : Arquivo header sysconfig.h com definições de entradas/saidas 
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef _SYSCONFIG_H
#define _SYSCONFIG_H

/* Define --------------------------------------------------------------------*/

//Entradas

#define SENSOR_NIVEL 2
#define ZERO_CROSS 3
#define CHAVE_GERAL 4

//Saídas

#define SAIDA_TRIAC 6
#define LED_NIVEL_ALTO 8
#define LED_NIVEL_BAIXO 9
#define LED_INDICACAO 10

//Defines Gerais

#define BAUD_RATE 9600
#define TEMPO_ESPERA (uint8_t) 10
#define TEMPO_PISCA_INICIAL 250
#define TEMPO_PISCA_ALARME 100
#define POTENCIA_SAIDA 40

#endif

/*****************************END OF FILE**************************************/
