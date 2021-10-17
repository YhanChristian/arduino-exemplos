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

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef _MAIN_H
#define _MAIN_H

/* Includes ------------------------------------------------------------------*/

#include <Arduino.h>
#include <Nextion.h>
#include <SPI.h> //Necessário para funcionamento
#include <SoftwareSerial.h> 
#include <SD.h>
#include "sysconfig.h"
#include <avr/wdt.h>

/* Public objects ------------------------------------------------------------*/

// page ID, component ID, object name

NexPage pg_inicial = NexPage(0, 0, "pg_inicial");
NexPage pg_principal = NexPage(1, 0, "pg_principal");
NexDSButton bt0 = NexDSButton(1, 3, "bt0");
NexDSButton bt1 = NexDSButton(1, 5, "bt1");
NexDSButton bt2 = NexDSButton(1, 7, "bt2");
NexDSButton bt3 = NexDSButton(1, 9, "bt3");

// Registros Touch

NexTouch *nex_listen_list[] = {
    &pg_inicial,
    &pg_principal,
    &bt0,
    &bt1,
    &bt2,
    &bt3,
    NULL};


/* Private function prototypes -----------------------------------------------*/

void init_io();
void init_nextion();
void enable_wdt();
void bt0_callback(void *ptr);
void bt1_callback(void *ptr);
void bt2_callback(void *ptr);
void bt3_callback(void *ptr);

#endif

/*****************************END OF FILE**************************************/
