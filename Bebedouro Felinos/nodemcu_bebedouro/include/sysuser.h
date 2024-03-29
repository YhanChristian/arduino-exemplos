#ifndef _SYSUSER_H
#define _SYSUSER_H

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