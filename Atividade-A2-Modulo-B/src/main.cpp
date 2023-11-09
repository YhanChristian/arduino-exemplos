/*
Nome: Yhan Christian Souza Silva
Módulo B - Atividade A2
Implemente um programa para controlar remotamente um grupo de dispositivos composto
por 1 led, 1 display LCD, 1 Display de 7 segmentos com decodificador CD4511 e 1 Buzzer, e
monitorar os estados de 2 botões (push button).
 Funcionamento:
✔ O usuário deve digitar um comando no monitor Serial para ativar o dispositivo desejado.
✔ O monitor Serial deve enviar esse comando para um Arduíno, denominado módulo-A
através da USART 0. Logo depois, o Módulo-A deve transmitir o comando recebido
para um outro Arduíno, chamado de módulo-B via USART 1.
✔ O módulo-B irá acionar o dispositivo correspondente ao comando recebido. Além disso,
o módulo-B deve monitorar o estado de 2 botões conectados em dois pinos GPIO.
✔ Quando um dos botões forem pressionados, o módulo-B deverá enviar uma mensagem
ao módulo-A informando o status do botão.
✔ O status do botão recebido pelo módulo-A deve ser mostrado no monitor Serial.
 Obs: Somente o Módulo-A deve ser conectado ao monitor Serial.
 Digite os comandos (em negrito - cor preta) no monitor Serial para controlar os dispositivos e
imprima no monitor Serial as mensagens de status (em negrito - cor vermelha) para interagir
com o usuário.
 Definição:
1) LigarLed (click no botão enviar do monitor Serial);
 a) Imprima no monitor a mensagem de status: Led Ligado (mensagem deve ser
impressa no monitor quando o led for ligado).
2) Delisgar Led (click em enviar)
 a) status: Led desligado
3) LigarBuzzer (click em enviar)
 a) status: Buzzer ligado
4) DesligarBuzzer (click em enviar)
 a) Status: Buzzer desligado
5) Display7(click em enviar); Obs: Inicialize o display com o valor 0
 a) Imprima no monitor Serial “Digite o número desejado entre 0 e 9”.
 ex: 9 (click em enviar);
 b) Status: Número alterado
6) Display LCD (click em enviar)
 a) Imprima no monitor Serial “Digite a mensagem desejada”
 ex: teste (click em enviar)
 b) Status: Mensagem alterada
 7) Quando uma das chaves do módulo-B forem pressionadas
a) Status: Chave 1 ON
b) Status: Chave 2 OFF
*/

/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

/* Define --------------------------------------------------------------------*/
#define BAUD_RATE 9600
#define VALOR_MIN_7SEG 0
#define VALOR_MAX_7SEG 9

#define INIT_LAST_DEBOUNCE_TIME 0
#define DEFAULT_DEBOUNCE_DELAY 25

/* INPUTS --------------------------------------------------------------------*/
#define GPIO_BUTTON_1 3
#define GPIO_BUTTON_2 2

/* OUTPUTS -------------------------------------------------------------------*/
#define GPIO_BUZZER 6
#define GPIO_LED 13

#define CD4511_PINO_1 8
#define CD4511_PINO_2 9
#define CD4511_PINO_3 10
#define CD4511_PINO_4 11

/* LCD -----------------------------------------------------------------------*/
#define LCD_ADDR 0x27
#define LCD_COLUNAS 20
#define LCD_LINHAS 4

/* Typedef -------------------------------------------------------------------*/

/*Callbacks*/
typedef void (*pvbuttoncallback_t)(void);

/*Enum Botoes*/
typedef enum
{
  BOTAO_1 = 0,
  BOTAO_2,
  MAX_BUTTONS
} e_buttons_t;

/*Enum Dispositivos*/
typedef enum
{
  LED = 0,
  BUZZER,
  MAX_OUTPUTS
} e_outputs_t;

enum
{
  CMD_LIGAR_LED = 0,
  CMD_DESLIGAR_LED,
  CMD_LIGAR_BUZZER,
  CMD_DESLIGAR_BUZZER,
  MAX_CMD
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

/*Structs*/
typedef struct
{
  e_buttons_t eButton;
  uint8_t uiPin;
  bool bToggle;
  bool bStatus;
  bool bLastStatus;
  unsigned long ulLastDebounceTime;
  unsigned long ulDebounceDelay;
  pvbuttoncallback_t pvbButtonCallback;
} st_buttons_t;

typedef struct
{
  e_outputs_t eOutput;
  uint8_t uiPin;
  bool bStatus;
} st_outputs_t;

/* Public objects ------------------------------------------------------------*/
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLUNAS, LCD_LINHAS);

/* Private objects -----------------------------------------------------------*/
st_buttons_t gstButtons[MAX_BUTTONS] = {
    /*button, button_pin, toogle, status, last_status, lastDebounce, debounce, callback*/
    {BOTAO_1, GPIO_BUTTON_1, false, true, true, INIT_LAST_DEBOUNCE_TIME, DEFAULT_DEBOUNCE_DELAY, NULL},
    {BOTAO_2, GPIO_BUTTON_2, false, true, true, INIT_LAST_DEBOUNCE_TIME, DEFAULT_DEBOUNCE_DELAY, NULL}};

st_outputs_t gstOutputs[MAX_OUTPUTS] = {
    /*gpio, gpio_pin, status*/
    {LED, GPIO_LED, false},
    {BUZZER, GPIO_BUZZER, false}};

/* Global constants ----------------------------------------------------------*/

const char *gpvszComandosRecebidos[MAX_CMD] = {
    "Ligar Led",
    "Desligar Led",
    "Ligar Buzzer",
    "Desligar Buzzer",
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

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/
void LeDadosSerial1();
void IniciaDisplayLCD();
void IniciaDisplay7Seg();
void ConfiguraSaidas(const st_outputs_t *pstOutputs, uint8_t uiNumOutputs);
void ConfiguraBotoes(const st_buttons_t *pstButtons, uint8_t uiNumButtons);
void CheckButtonStatus(st_buttons_t *pstButtons, uint8_t uiNumButtons);
void SetButtonCallback(st_buttons_t *pstButtons, uint8_t uiButtonPin, pvbuttoncallback_t pvbButtonCallback);
void Button01Callback(void);
void Button02Callback(void);
void AtualizaDisplay7Seg(uint8_t uiValor);
void AtualizaDisplayLCD(const char *pszMensagem);

/* Setup ---------------------------------------------------------------------*/
void setup()
{
  Serial1.begin(BAUD_RATE);
  Serial.begin(BAUD_RATE);

  /*Configura Botões*/
  ConfiguraBotoes(gstButtons, MAX_BUTTONS);

  /*Configura Saídas*/
  ConfiguraSaidas(gstOutputs, MAX_OUTPUTS);

  /*Inicia Display LCD */
  IniciaDisplayLCD();

  /*Inicia Display 7Seg*/
  IniciaDisplay7Seg();

  /*Pritna Msg indicando aguardar comandos*/
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Aguardando Comandos.");
}

/* Loop ----------------------------------------------------------------------*/
void loop()
{
  /*Le dados Serial (Comandos recebidos)*/
  LeDadosSerial1();

  /*Verifica Botões*/
  CheckButtonStatus(gstButtons, MAX_BUTTONS);

  /*Configura Callbacks*/
  SetButtonCallback(gstButtons, BOTAO_1, Button01Callback);
  SetButtonCallback(gstButtons, BOTAO_2, Button02Callback);
}

/* Body of private functions -------------------------------------------------*/
void LeDadosSerial1()
{
  String strComando;
  char c;
  int i = 0;

  if (Serial1.available() > 0)
  {
    strComando = Serial1.readStringUntil('\n');
    Serial.println(strComando);

    for (i = 0; i < MAX_CMD; i++)
    {
      if (!strncmp(strComando.c_str(), gpvszComandosRecebidos[i], strlen(gpvszComandosRecebidos[i])))
      {
        switch (i)
        {
        case CMD_LIGAR_LED:
          gstOutputs[LED].bStatus = true;
          digitalWrite(gstOutputs[LED].uiPin, gstOutputs[LED].bStatus);
          Serial1.println(gpvszStatus[STATUS_LED_ON]);
          return;
        case CMD_DESLIGAR_LED:
          gstOutputs[LED].bStatus = false;
          digitalWrite(gstOutputs[LED].uiPin, gstOutputs[LED].bStatus);
          Serial1.println(gpvszStatus[STATUS_LED_OFF]);
          return;
        case CMD_LIGAR_BUZZER:
          gstOutputs[BUZZER].bStatus = true;
          digitalWrite(gstOutputs[BUZZER].uiPin, gstOutputs[BUZZER].bStatus);
          Serial1.println(gpvszStatus[STATUS_BUZZER_ON]);
          return;
        case CMD_DESLIGAR_BUZZER:
          gstOutputs[BUZZER].bStatus = false;
          digitalWrite(gstOutputs[BUZZER].uiPin, gstOutputs[BUZZER].bStatus);
          Serial1.println(gpvszStatus[STATUS_BUZZER_OFF]);
          return;
        default:
          return;
        }
      }
    }

    c = strComando.charAt(0);
    if (isdigit(c) && c >= '0' && c <= '9')
    {
      int i = c - '0';
      AtualizaDisplay7Seg(i);
      return;
    }

    if (strComando.length() > 1  && strComando.length() <= LCD_COLUNAS)
    {
      AtualizaDisplayLCD((char *)strComando.c_str());
      return;
    }
    return;
  }
}

void IniciaDisplayLCD()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print("Bem-Vindo");
  lcd.setCursor(5, 2);
  lcd.print("Modulo B");
}

void IniciaDisplay7Seg()
{
  /*Configura Pinos como Saídas*/
  pinMode(CD4511_PINO_1, OUTPUT);
  pinMode(CD4511_PINO_2, OUTPUT);
  pinMode(CD4511_PINO_3, OUTPUT);
  pinMode(CD4511_PINO_4, OUTPUT);

  /*Inicializa Display faz for de 0 à 9 */
  for (int i = 0; i <= VALOR_MAX_7SEG; i++)
  {
    AtualizaDisplay7Seg(i);
    delay(250);
  }

  /*Mantém Display com Valor Zero*/
  AtualizaDisplay7Seg(VALOR_MIN_7SEG);
}

void ConfiguraSaidas(const st_outputs_t *pstOutputs, uint8_t uiNumOutputs)
{
  int i = 0;
  for (i = 0; i < uiNumOutputs; i++)
  {
    pinMode(pstOutputs[i].uiPin, OUTPUT);
    digitalWrite(pstOutputs[i].uiPin, pstOutputs[i].bStatus);
  }
}

void ConfiguraBotoes(const st_buttons_t *pstButtons, uint8_t uiNumButtons)
{
  int i = 0;
  for (i = 0; i < uiNumButtons; i++)
  {
    pinMode(pstButtons[i].uiPin, INPUT_PULLUP);
  }
}

void SetButtonCallback(st_buttons_t *pstButtons, uint8_t uiButtonPin, pvbuttoncallback_t pvbButtonCallback)
{
  pstButtons[uiButtonPin].pvbButtonCallback = pvbButtonCallback;
}

void CheckButtonStatus(st_buttons_t *pstButtons, uint8_t uiNumButtons)
{
  int i = 0;
  bool bReading = false;
  unsigned long ulCurrentTime = 0;

  for (i = 0; i < uiNumButtons; i++)
  {
    bReading = digitalRead(pstButtons[i].uiPin);
    ulCurrentTime = millis();

    if (bReading != pstButtons[i].bLastStatus)
    {
      pstButtons[i].ulLastDebounceTime = ulCurrentTime;
    }

    if ((ulCurrentTime - pstButtons[i].ulLastDebounceTime) > pstButtons[i].ulDebounceDelay)
    {
      if (bReading != pstButtons[i].bStatus)
      {
        pstButtons[i].bStatus = bReading;
        if (pstButtons[i].bStatus == true)
        {
          pstButtons[i].bToggle = !pstButtons[i].bToggle;
          if (pstButtons[i].pvbButtonCallback != NULL)
          {
            pstButtons[i].pvbButtonCallback();
          }
        }
      }
    }
    pstButtons[i].bLastStatus = bReading;
  }
}

void Button01Callback(void)
{
  if (gstButtons[BOTAO_1].bToggle == true)
  {
    Serial1.println(gpvszStatus[STATUS_BUTTON_1_ON]);
  }
  else
  {
    Serial1.println(gpvszStatus[STATUS_BUTTON_1_OFF]);
  }
}

void Button02Callback(void)
{
  if (gstButtons[BOTAO_2].bToggle == true)
  {
    Serial1.println(gpvszStatus[STATUS_BUTTON_2_ON]);
  }
  else
  {
    Serial1.println(gpvszStatus[STATUS_BUTTON_2_OFF]);
  }
}

void AtualizaDisplay7Seg(uint8_t uiValor)
{

  digitalWrite(CD4511_PINO_1, (uiValor & 0x01));
  digitalWrite(CD4511_PINO_2, (uiValor & 0x02));
  digitalWrite(CD4511_PINO_3, (uiValor & 0x04));
  digitalWrite(CD4511_PINO_4, (uiValor & 0x08));
  Serial1.println(gpvszStatus[STATUS_NUMBER_CHANGED]);
}

void AtualizaDisplayLCD(const char *pszMensagem)
{
  char szDisplayMsg[LCD_COLUNAS + 1];
  memset(szDisplayMsg, '\0', sizeof(szDisplayMsg));
  if (pszMensagem != NULL && strlen(pszMensagem) <= LCD_COLUNAS)
  {
    strncpy(szDisplayMsg, pszMensagem, strlen(pszMensagem) - 1);
    lcd.clear();
    lcd.print(szDisplayMsg);
    Serial1.println(gpvszStatus[STATUS_MESSAGE_CHANGED]);
    return;
  }
  return;
}