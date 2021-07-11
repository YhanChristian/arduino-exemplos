#ifndef _SYSCONFIG_H
#define _SYSCONFIG_H

// Bibliotecas Auxiliares 
#include <Arduino.h>
#include <ESP8266WiFi.h>  
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <Ticker.h>

// Defines de Hardware e Gerais do projeto

// Entradas

#define ZERO_CROSS D2
#define SENSOR_NIVEL D3
#define HABILITA_SISTEMA D4

// Saídas 

#define LED_NIVEL_ALTO D5
#define LED_NIVEL_BAIXO D6
#define LED_STATUS D7
#define SAIDA_PWM D8

// Defines Gerais

#define BAUD_RATE 115200

#define INTERVALO_MILLIS_01 250

//Protótipo das Funções

void iniciaIOs();
void iniciaSerial();
void conectaWiFi();
void configModeCallback(WiFiManager *myWiFiManager); 
void tick();
void configInterrupt();
extern ICACHE_RAM_ATTR void trataISR();
void desabilitaWDT();
void habilitaWDT();

#endif