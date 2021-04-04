#ifndef _SYSUSER_H_
#define _SYSUSER_H_

// Bibliotecas Auxiliares 

#include <Arduino.h>
#include "sysconfig.h"

//Protótipo das Funções

bool leituraEntradaDigital(uint8_t pino);
void ligaSaidaDigital(uint8_t pino);
void desligaSaidaDigital(uint8_t pino);
void desabilitaSistema();
void indicaSistemaDesabilitado();

#endif