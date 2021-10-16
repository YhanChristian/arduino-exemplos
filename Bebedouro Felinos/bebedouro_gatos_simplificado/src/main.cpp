/** =============================================================================
  Projeto: Controle de saída PWM (bomba submersível) e monitoramento do nível
  Utilizado: Arduino, Leds de Indicação, Sensor de nível, chave ON/OFF, módulo TRIAC
  @file main.cpp
  @author Yhan Christian Souza Silva
  @date  15/10/2021
  @version V.1.0.0

                **DESCRIÇÃO DE FUNCIONAMENTO DO FIRMWARE**
  - O sistema após inicialização fará o acionamento da SAIDA_TRIAC Para controle 
  do fluxo de água da bomba submersível, é colocado um sensor de nível que 
  identifica o baixo nível e o LED_NIVEL_BAIXO será aceso e  a bomba desabilitada. 
  
  Versão Simplificada Arduino
  Aplicação do Projeto: Pessoal, para controle do fluxo de água do 
  bebedouro dos gatos. 
  =========================================================================== */

/* Includes ------------------------------------------------------------------*/

#include <Arduino.h>
#include "sysconfig.h"
#include <avr/wdt.h>

/* Private variables ---------------------------------------------------------*/

bool bHabilitaSistema, bBaixoNivel, bZeroCrossDetector;

/* Private ISR's (Interrupt Service Routine) ---------------------------------*/

void trata_isr_zc();

/* Function Pointer FSM - Finite State Machine -------------------------------*/

void (*pvMaquinaEstados)();

/* Private functions ---------------------------------------------------------*/

void inicia_io();
void config_interrupcao();
void inicia_flags();
void pisca_led(uint8_t uiPino, uint16_t uiIntervalo);
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

  //Inicia Variáveis
  inicia_flags();

  //Habilitando comunicação Serial
  Serial.begin(BAUD_RATE);

  Serial.println("Inicio sistema");

  Serial.println("Estado Sensor:" + String(bBaixoNivel));

  //Aponta estado inicial sistema
  pvMaquinaEstados = estado_inicial;

  //Habilita WDT - Watch Dog Timer
  wdt_enable(WDTO_4S);
}

/* Bodie of loop function ----------------------------------------------------*/

void loop()
{
  //Realimenta o WDT
  wdt_reset();
  //Chama a função apontada pela máquina de estados
  (*pvMaquinaEstados)();
}

/*Bodie of private ISR's (Interrupt Service Routine) --------------------------*/

void trata_isr_zc()
{
  bZeroCrossDetector = true;
}

/* Bodies of private functions -----------------------------------------------*/

void inicia_io()
{
  //Configura entradas - input pullup
  for (uint8_t i = 2; i < 5; i += 2)
  {
    pinMode(i, INPUT_PULLUP);
  }

  //Configura entrada - ZC
  pinMode(ZERO_CROSS, INPUT);

  //Configura saídas e inicia com todas desligadas
  for (uint8_t i = 6; i < 11; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

void config_interrupcao()
{
  //Configura interrupção zero cross módulo triac
  attachInterrupt(digitalPinToInterrupt(ZERO_CROSS), trata_isr_zc, RISING);
}

void inicia_flags()
{
  //Estado inicial flags indicação sistema
  bBaixoNivel = true;
  bHabilitaSistema = false;
  bZeroCrossDetector = false;
}

void pisca_led(uint8_t uiPino, uint16_t uiIntervalo)
{
  static unsigned long ulUltimoMillis = 0;

  if ((long)millis() - ulUltimoMillis > uiIntervalo)
  {
    digitalWrite(uiPino, !digitalRead(uiPino));
    ulUltimoMillis = millis();
  }
}

// Implementação dos estados - FSM

void estado_inicial()
{
  //Inicia com LEDs piscando alternando - Azul ligado Fixo
  static bool bAlternaLed = false;
  if (!bAlternaLed)
  {
    pisca_led(LED_NIVEL_ALTO, TEMPO_PISCA_INICIAL);
    bAlternaLed = true;
  }
  else
  {
    pisca_led(LED_NIVEL_BAIXO, TEMPO_PISCA_INICIAL);
    bAlternaLed = false;
  }
  digitalWrite(LED_INDICACAO, HIGH);

  //Saída Triac - Desabilitada
  digitalWrite(SAIDA_TRIAC, LOW);

  //Realiza leitura baixo nível
  bBaixoNivel = digitalRead(SENSOR_NIVEL);

  //Verifica chave Geral e sinal de (nível)
  if (!digitalRead(CHAVE_GERAL) && bBaixoNivel)
  {
    Serial.println("Vá para o estado normal");
    pvMaquinaEstados = estado_normal;
  }

  else if (!digitalRead(CHAVE_GERAL) && !bBaixoNivel)
  {
    Serial.println("Vá para o estado alarme");
    pvMaquinaEstados = estado_alarme;
  }
}

void estado_normal()
{
  //Inicia com LED Azul desligado, Vermelho Desligado, Verde Aceso Fixo
  digitalWrite(LED_NIVEL_BAIXO, LOW);
  digitalWrite(LED_NIVEL_ALTO, HIGH);
  digitalWrite(LED_INDICACAO, LOW);

  //Verifica flag interrupção Triac para acionamento saída PWM
  if (bZeroCrossDetector)
  {
    unsigned long uiTempoTriac = 8200L * (100L - POTENCIA_SAIDA) / 100L;
    unsigned long uiTempoEspera = 6;
    delayMicroseconds(uiTempoTriac);
    digitalWrite(SAIDA_TRIAC, HIGH);
    delayMicroseconds(uiTempoEspera);
    digitalWrite(SAIDA_TRIAC, LOW);
    bZeroCrossDetector = false;
  }

  //Realiza leitura baixo nível
  bBaixoNivel = digitalRead(SENSOR_NIVEL);

  //Verifica Alarme e Chave Geral para direcionar ao estado correspondente
  if (!digitalRead(CHAVE_GERAL) && !bBaixoNivel)
  {
    Serial.println("Vá para o estado alarme");
    pvMaquinaEstados = estado_alarme;
  }

  //Chave geral  desligada volta estado inicial
  else if (digitalRead(CHAVE_GERAL))
  {
    Serial.println("Vá para o estado inicial");
    inicia_flags();
    pvMaquinaEstados = estado_inicial;
  }
}

void estado_alarme()
{
  //Inicia com LED Azul piscando - Vermelho aceso fixo
  pisca_led(LED_INDICACAO, TEMPO_PISCA_ALARME);
  digitalWrite(LED_NIVEL_BAIXO, HIGH);
  digitalWrite(LED_NIVEL_ALTO, LOW);

  //Saída Triac - Desabilitada
  digitalWrite(SAIDA_TRIAC, LOW);

  //Realiza leitura baixo nível
  bBaixoNivel = digitalRead(SENSOR_NIVEL);

  //Verifica chave geral para retornar ao estado correspondente
  if (!digitalRead(CHAVE_GERAL) && bBaixoNivel)
  {
    Serial.println("Vá para o estado normal");
    pvMaquinaEstados = estado_normal;
  }

  //Chave geral  desligada volta estado inicial
  else if (digitalRead(CHAVE_GERAL))
  {
    Serial.println("Vá para o estado inicial");
    inicia_flags();
    pvMaquinaEstados = estado_inicial;
  }
}

/*****************************END OF FILE**************************************/
