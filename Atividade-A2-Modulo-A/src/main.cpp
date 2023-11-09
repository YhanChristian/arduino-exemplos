/*
Nome: Yhan Christian Souza Silva
Módulo A - Atividade A2
Implemente um programa para controlar remotamente um grupo de dispositivos composto
por 1 led, 1 display LCD, 1 Display de 7 segmentos com decodificador CD4511 e 1 Buzzer, e
monitorar os estados de 2 botões (push button).
 Funcionamento:
✔ O usuário deve digitar um comando no monitor serial para ativar o dispositivo desejado.
✔ O monitor serial deve enviar esse comando para um Arduíno, denominado módulo-A
através da USART 0. Logo depois, o Módulo-A deve transmitir o comando recebido
para um outro Arduíno, chamado de módulo-B via USART 1.
✔ O módulo-B irá acionar o dispositivo correspondente ao comando recebido. Além disso,
o módulo-B deve monitorar o estado de 2 botões conectados em dois pinos GPIO.
✔ Quando um dos botões forem pressionados, o módulo-B deverá enviar uma mensagem
ao módulo-A informando o status do botão.
✔ O status do botão recebido pelo módulo-A deve ser mostrado no monitor serial.
 Obs: Somente o Módulo-A deve ser conectado ao monitor serial.
 Digite os comandos (em negrito - cor preta) no monitor serial para controlar os dispositivos e
imprima no monitor serial as mensagens de status (em negrito - cor vermelha) para interagir
com o usuário.
 Definição:
1) LigarLed (click no botão enviar do monitor serial);
 a) Imprima no monitor a mensagem de status: Led Ligado (mensagem deve ser
impressa no monitor quando o led for ligado).
2) Delisgar Led (click em enviar)
 a) status: Led desligado
3) LigarBuzzer (click em enviar)
 a) status: Buzzer ligado
4) DesligarBuzzer (click em enviar)
 a) Status: Buzzer desligado
5) Display7(click em enviar); Obs: Inicialize o display com o valor 0
 a) Imprima no monitor serial “Digite o número desejado entre 0 e 9”.
 ex: 9 (click em enviar);
 b) Status: Número alterado
6) Display LCD (click em enviar)
 a) Imprima no monitor serial “Digite a mensagem desejada”
 ex: teste (click em enviar)
 b) Status: Mensagem alterada
 7) Quando uma das chaves do módulo-B forem pressionadas
a) Status: Chave 1 ON
b) Status: Chave 2 OFF
*/

/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>

/* Define --------------------------------------------------------------------*/
#define BAUD_RATE 9600

/* Typedef -------------------------------------------------------------------*/

typedef enum
{
  SYSTEM_OK = 0,
  /*Lista de erros sistema*/
  SYSTEM_INV_CMD = 1000,
  SYSTEM_INV_PARAM,
  SYSTEM_BUF_OVF,
  MAX_SYSTEM_ERRORS
} e_erros_t;

enum
{
  CMD_LIGAR_LED = 0,
  CMD_DESLIGAR_LED,
  CMD_LIGAR_BUZZER,
  CMD_DESLIGAR_BUZZER,
  CMD_DISPLAY_7SEG,
  CMD_DISPLAY_LCD,
  MAX_CMD
};

enum 
{
  DISP_CMD_NUM = 0,
  DISP_CMD_MSG,
  MAX_DISP_CMD
};

enum
{
  STATUS_BUTTON_1_ON = 0,
  STATUS_BUTTON_1_OFF,
  STATUS_BUTTON_2_ON,
  STATUS_BUTTON_2_OFF,
  STATUS_LED_ON,
  STATUS_LED_OFF,
  STATUS_BUZZER_ON,
  STATUS_BUZZER_OFF,
  STATUS_NUMBER_CHANGED,
  STATUS_MESSAGE_CHANGED,
  MAX_STATUS
};

/* Public objects ------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Global constants ----------------------------------------------------------*/
const char *gpvszComandosSerial[MAX_CMD] = {
    "Ligar Led",
    "Desligar Led",
    "Ligar Buzzer",
    "Desligar Buzzer",
    "Display7",
    "Display LCD"
};

const char *gpszMensagemComandosDisplay[MAX_DISP_CMD] = {
    "Digite o número desejado entre 0 e 9: ",
    "Digite a mensagem desejada: "
};

const char *gpvszStatus[MAX_STATUS] = {
    "Status: Chave 1 ON",
    "Status: Chave 1 OFF",
    "Status: Chave 2 ON",
    "Status: Chave 2 OFF",
    "Status: Led Ligado",
    "Status: Led Desligado",
    "Status: Buzzer Ligado",
    "Status: Buzzer Desligado",
    "Status: Número alterado",
    "Status: Mensagem alterada",
};

/* Function Prototypes -------------------------------------------------------*/
void LeDadosSerial();
void LeStatusSerial1();
e_erros_t EnviaComando(const char *pszComando);
e_erros_t RecebeStatus(const char *pszStatus);
const char *pszErrorToString(e_erros_t eRet);

/* Setup ---------------------------------------------------------------------*/
void setup()
{
  Serial.begin(BAUD_RATE);
  Serial1.begin(BAUD_RATE);
}

/* Loop ----------------------------------------------------------------------*/
void loop()
{
  /*Le dados Serial (Comandos a enviar) */
  LeDadosSerial();

  /*Verifica Dados Serial1 (Recebidos Modulo B)*/
  LeStatusSerial1();
}

/* Body of private functions -------------------------------------------------*/
void LeDadosSerial()
{
  String strComando;
  e_erros_t eRet = SYSTEM_OK;
  
  if (Serial.available())
  {
    /*Lê dados serial*/
    strComando = Serial.readStringUntil('\n');
    /*Envia comando para módulo B*/
    eRet = EnviaComando(strComando.c_str());
    if (!eRet) return;
    /*Imprime erro caso ocorra*/
    Serial.println(pszErrorToString(eRet));
  }
}

void LeStatusSerial1()
{
  String strStatus;
  e_erros_t eRet = SYSTEM_OK;

  if (Serial1.available())
  {
    /*Lê dados serial*/
    strStatus = Serial1.readStringUntil('\n');
    /*Recebe status dos botões do módulo B*/
    eRet = RecebeStatus(strStatus.c_str());
    if (!eRet) return;
    /*Imprime erro caso ocorra*/
    Serial.println(pszErrorToString(eRet));
  }
}

e_erros_t EnviaComando(const char *pszComando)
{
  String strComando;
  int i = 0;
  char c;
  /*Verifica Comando Recebido e envia para módulo B*/
  for (i = 0; i < MAX_CMD; i++)
  {
    if (!strncmp(pszComando, gpvszComandosSerial[i], strlen(gpvszComandosSerial[i])))
    {
     /*Printa Mensagem e Aguarda número digitado*/
      if(i == CMD_DISPLAY_7SEG)
      {
        Serial.println(gpszMensagemComandosDisplay[i - 4]);
        while(!Serial.available());
        strComando = Serial.readStringUntil('\n');
        c = strComando.charAt(0);
        if(!isDigit(c)) 
        {
          return SYSTEM_INV_PARAM;
        }
        Serial1.println(strComando);
        return SYSTEM_OK;
      }
      /*Printa Mensagem e Aguarda mensagem digitada*/
      else if(i == CMD_DISPLAY_LCD)
      {
        Serial.println(gpszMensagemComandosDisplay[i - 4]);
        while(!Serial.available());
        strComando = Serial.readStringUntil('\n');
        if(strComando.length() < 2 || isDigit(strComando.charAt(0)))
        {
          return SYSTEM_INV_PARAM;
        }
        if(strComando.length() > 20)
        {
          return SYSTEM_BUF_OVF;
        }
        Serial1.println(strComando);
        return SYSTEM_OK;
      }
      else
      {
        Serial1.println(pszComando);
        return SYSTEM_OK;
      }
      
    }
  }
  return SYSTEM_INV_CMD;
}
e_erros_t RecebeStatus(const char *pszStatus)
{
  int i = 0;
  /*Verifica Status dos botões recebidos do módulo B*/
  for (i = 0; i < MAX_STATUS; i++)
  {
    if (!strncmp(pszStatus, gpvszStatus[i], strlen(gpvszStatus[i])))
    {
      Serial.println(pszStatus);
      return SYSTEM_OK;
    }
  }
  return SYSTEM_INV_PARAM;
}

const char *pszErrorToString(e_erros_t eRet)
{
  switch (eRet)
  {
  case SYSTEM_OK:
    return "SYSTEM_OK";
  case SYSTEM_INV_CMD:
    return "SYSTEM_INV_CMD";
  case SYSTEM_INV_PARAM:
    return "SYSTEM_INV_PARAM";
  case SYSTEM_BUF_OVF:
    return "SYSTEM_BUF_OVF";
  default:
    return "SYSTEM_INV_ERROR";
  }
}