/*
Atividade Prática A3
Implemente um programa com RTOS para controlar uma centrífuga de laboratório usando o
microcontrolador Atmega 328 ou 2560 e outros dispositivos como motor DC ou de passo,
display LCD, display 7-segmentos, leds, keypad, RTC, buzzer e pushbutton.
Utilize com exemplo o código implementado usando RTOS disponível no link abaixo para
criar uma task para acionar cada dispositivo:
https://wokwi.com/projects/349859823762801235

Requisitos:
✔ O motor deve girar no sentido horário durante o tempo de funcionamento inserido no
keypad pelo usuário.
✔ O display LCD deve mostrar o horário computado pelo módulo RTC.
✔ Os displays 7-segmentos devem mostrar a contagem decrescente de tempo de
funcionamento da centrífuga (minutos e segundo).
✔ O tempo de funcionamento e o horário devem ser ajustados
✔ O tempo de funcionamento e o horário devem ser ajustados usando o keypad.
✔ Os pushbuttons devem ser usados para selecionar a velocidade de rotação do motor.
Cada pushbutton deve selecionar uma velocidade diferente de funcionamento. A
centrífuga deve possui, no mínimo, 4 velocidades diferentes.
✔ Cada led deve ser acionado quando uma determinada velocidade de rotação for
ativada. Utilize no mínimo 4 leds.
✔ Além dos pushbuttons de controle de velocidade, utilize mais dois botões, um para ligar
e um outro para desligar o equipamento.
✔ O buzzer deve apitar 3 vezes (1 segundo ligado e 1 segundo desligado) assim que
finalizar o tempo de funcionamento.
Outras funcionalidades são bem-vinda, sua implementação será premiada com um
acréscimo da nota!
*/

/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>
#include <event_groups.h>
#include <LiquidCrystal_I2C.h>
#include <TM1637Display.h>
#include <Keypad.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <A4988.h>

/* Define --------------------------------------------------------------------*/

#define BAUD_RATE 9600

#define CENTRIFUGA_MODO_AUTO 0
#define CENTRIFUGA_MODO_MANUAL 1
#define SISTEMA_ON 2
#define SISTEMA_OFF 3

#define BIT_CLEAR_DISPLAY (1 << 0)
#define BIT_BUTTON_LIGA (1 << 1)
#define BIT_BUTTON_DESLIGA (1 << 2)
#define BIT_BUTTON_MODO (1 << 3)
#define BIT_BUTTON_VELOCIDADE_1 (1 << 4)
#define BIT_BUTTON_VELOCIDADE_2 (1 << 5)
#define BIT_BUTTON_VELOCIDADE_3 (1 << 6)
#define BIT_BUTTON_VELOCIDADE_4 (1 << 7)

#define BIT_OUTPUT_VELOCIDADE_1 (1 << 0)
#define BIT_OUTPUT_VELOCIDADE_2 (1 << 1)
#define BIT_OUTPUT_VELOCIDADE_3 (1 << 2)
#define BIT_OUTPUT_VELOCIDADE_4 (1 << 3)

/* INPUTS --------------------------------------------------------------------*/
#define BTN_LIGA 10
#define BTN_DESLIGA 11
#define BTN_MODO 12

#define BTN_VELOCIDADE_1 32
#define BTN_VELOCIDADE_2 33
#define BTN_VELOCIDADE_3 34
#define BTN_VELOCIDADE_4 35

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

#define KEYPAD_ROW_1 A0
#define KEYPAD_ROW_2 A1
#define KEYPAD_ROW_3 A2
#define KEYPAD_ROW_4 A3
#define KEYPAD_COL_1 A4
#define KEYPAD_COL_2 A5
#define KEYPAD_COL_3 A6
#define KEYPAD_COL_4 A7

#define KEYPAD_CHAR_1 '1'
#define KEYPAD_CHAR_2 '2'
#define KEYPAD_CHAR_3 '3'
#define KEYPAD_CHAR_4 '4'
#define KEYPAD_CHAR_5 '5'
#define KEYPAD_CHAR_6 '6'
#define KEYPAD_CHAR_7 '7'
#define KEYPAD_CHAR_8 '8'
#define KEYPAD_CHAR_9 '9'
#define KEYPAD_CHAR_0 '0'
#define KEYPAD_CHAR_A 'A'
#define KEYPAD_CHAR_B 'B'
#define KEYPAD_CHAR_C 'C'
#define KEYPAD_CHAR_D 'D'
#define KEYPAD_CHAR_STAR '*'
#define KEYPAD_CHAR_HASH '#'

/* OUTPUTS -------------------------------------------------------------------*/
#define LED_VELOCIDADE_1 A10
#define LED_VELOCIDADE_2 A11
#define LED_VELOCIDADE_3 A12
#define LED_VELOCIDADE_4 A13

#define LED_PROCESSO_CENTRIFUGACAO A14

#define BUZZER 36

/* STEP MOTOR ----------------------------------------------------------------*/
#define MOTOR_STEPS 200
#define MOTOR_DIR 7
#define MOTOR_EN 8
#define MICRO_STEPS 1

#define RPM_VELOCIDADE_0 0
#define RPM_VELOCIDADE_1 30
#define RPM_VELOCIDADE_2 60
#define RPM_VELOCIDADE_3 90
#define RPM_VELOCIDADE_4 120

/* LCD -----------------------------------------------------------------------*/
#define LCD_ADDR 0x27
#define LCD_ROWS 2
#define LCD_COLS 16

/* 7SEG ----------------------------------------------'------------------------*/
#define SEG_CLK 2
#define SEG_DIO 3

/* Typedef -------------------------------------------------------------------*/
enum
{
  VELOCIDADE_0 = 0,
  VELOCIDADE_1,
  VELOCIDADE_2,
  VELOCIDADE_3,
  VELOCIDADE_4,
  MAX_VELOCIDADES

};

typedef enum
{
  BUTTON_LIGA = 0,
  BUTTON_DESLIGA,
  BUTTON_MODO,
  BUTTON_VELOCIDADE_1,
  BUTTON_VELOCIDADE_2,
  BUTTON_VELOCIDADE_3,
  BUTTON_VELOCIDADE_4,
  MAX_BUTTONS
} e_buttons_t;

typedef enum
{
  OUTPUT_VELOCIDADE_1 = 0,
  OUTPUT_VELOCIDADE_2,
  OUTPUT_VELOCIDADE_3,
  OUTPUT_VELOCIDADE_4,
  OUTPUT_PROCESSO_CENTRIFUGACAO,
  OUTPUT_BUZZER,
  MAX_OUTPUTS
} e_outputs_t;

typedef enum
{
  SYS_OK = 0,
  SYS_ERROR = 1000,
  SYS_ERROR_INV_PARAM,
  SYS_ERROR_PARAM_OVERFLOW,
  SYS_ERROR_CREATE_TASK,
  SYS_ERROR_CREATE_QUEUE,
  SYS_ERROR_CREATE_EVENT_GROUP,
  MAX_ERRORS
} e_errors_t;

typedef enum
{
  RTOS_OK = 0,
  RTOS_ERROR = 2000,
  RTOS_ERROR_CREATE_TASK,
  RTOS_ERROR_CREATE_SEMAPHORE,
  RTOS_ERROR_CREATE_QUEUE,
  RTOS_ERROR_CREATE_EVENT_GROUP,
  MAX_RTOS_ERRORS
} e_rtos_errors_t;

/* Structs -------------------------------------------------------------------*/
typedef struct
{
  e_buttons_t eButton[MAX_BUTTONS];
  int iMode[MAX_BUTTONS];
  bool fState[MAX_BUTTONS];
  bool fPressed[MAX_BUTTONS];
} st_buttons_t;

typedef struct
{
  e_outputs_t eOutput[MAX_OUTPUTS];
  bool fState[MAX_OUTPUTS];
} st_outputs_t;

typedef struct
{
  char szHorarioCentrifugacao[8 + 1];
  char szTempoCentrifugacao[6 + 1];
  uint8_t uiRpmCentrifugacao;
  bool fModoCentrifugacao;
  bool fCentrifugacaoAtiva;
  bool fCentrifugacaoFinalizada;
  bool fHabilitaCentrifugacao;
} st_centrifuga_t;

/* Global constants ----------------------------------------------------------*/

const uint8_t uiRpmCentrifuga[MAX_VELOCIDADES] = {RPM_VELOCIDADE_0, RPM_VELOCIDADE_1,
                                                  RPM_VELOCIDADE_2, RPM_VELOCIDADE_3, RPM_VELOCIDADE_4};

const char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
    {KEYPAD_CHAR_1, KEYPAD_CHAR_2, KEYPAD_CHAR_3, KEYPAD_CHAR_A},
    {KEYPAD_CHAR_4, KEYPAD_CHAR_5, KEYPAD_CHAR_6, KEYPAD_CHAR_B},
    {KEYPAD_CHAR_7, KEYPAD_CHAR_8, KEYPAD_CHAR_9, KEYPAD_CHAR_C},
    {KEYPAD_CHAR_STAR, KEYPAD_CHAR_0, KEYPAD_CHAR_HASH, KEYPAD_CHAR_D}};

byte rowPins[KEYPAD_ROWS] = {KEYPAD_ROW_1, KEYPAD_ROW_2, KEYPAD_ROW_3, KEYPAD_ROW_4};
byte colPins[KEYPAD_COLS] = {KEYPAD_COL_1, KEYPAD_COL_2, KEYPAD_COL_3, KEYPAD_COL_4};

byte modoAuto[] = {
    B01110,
    B10001,
    B10001,
    B11111,
    B11011,
    B11011,
    B11111,
    B00000};

byte modoManual[] = {
    B01110,
    B10000,
    B10000,
    B11111,
    B11011,
    B11011,
    B11111,
    B00000};

byte sistemaOn[] = {
    B01010,
    B01010,
    B11111,
    B11111,
    B11111,
    B01110,
    B00100,
    B00100};

byte sistemaOff[] = {
    B01010,
    B01010,
    B11111,
    B10001,
    B10001,
    B01110,
    B00100,
    B00100};

const uint8_t SEG_DONE[] = {
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,         // d
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // O
    SEG_C | SEG_E | SEG_G,                         // n
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G          // E
};

const char *gpszErroCriacaoSemaforo = "Erro ao Criar Semáforo";
const char *gpszErroCriacaoFila = "Erro ao Criar Fila";
const char *gpszErroCriacaoGrupoEventos = "Erro ao Criar Grupo de Eventos";
const char *gpszErroCriacaoTarefa = "Erro ao Criar Tarefa";

const char *gpszMsgInstrucaoSistema = "Sistema Controle Centrífuga Iniciado\n"
                                      "Instruções de Uso:\n"
                                      "O Sistema Iniciará com Velocidade 1 no inicio da centrifugação\n"
                                      "O Sistema Iniciará com Tempo de Centrifugação 1min e Horario 12:00:00\n"
                                      "Pressione Liga para Iniciar\n"
                                      "Pressione Desliga para Parar\n"
                                      "Pressione Modo para Alterar Modo Centrifugação (Auto / Manual)\n"
                                      "Pressione Velocidade 1, 2, 3 ou 4 para Selecionar Velocidade durante centrifugacao\n"
                                      "Pressione (#) para Ajustar Horario, press. novamente Tempo de Centrifugacao e press. novamente para Salvar\n"
                                      "Pressione (*) para na Configuração para retornar ao Padrão Horário e Tempo de Centrifiguração,\n"
                                      "press. novamente para sair SEM SALVAR, fora da configuração exibirá o horário e tempo de centrifugação CONFIGURADOS\n"
                                      "Pressione (A) para Tempo de Centrifugação 1min e Horario 12:00:00\n"
                                      "Pressione (B) para Tempo de Centrifugação 5min e Horario 12:00:00\n"
                                      "Pressione (C) para Tempo de Centrifugação 10min e Horario 12:00:00\n"
                                      "Pressione (D) para Tempo de Centrifugação 15min e Horario 12:00:00\n"
                                      "Ao Finalizar Tempo de Centrifugação o Buzzer Tocará 3 vezes\n";
const char *gpszMsgBemVindo = "Bem-Vindo!";
const char *gpszMsgIniciaSistema = "Sistema Iniciado";
const char *gpszMsgRTCInicio = "RTC Iniciado";
const char *gpszMsgSemRTC = "RTC Não Encontrado";
const char *gpszMsgFalhaRTC = "Falha ao Iniciar RTC";
const char *gpszMsgSistemaLigado = "Sistema Ligado";
const char *gpszMsgSistemaDesligado = "Sistema Desligado";
const char *gpszMsgBuzzer = "Buzzer";
const char *gpszMsgLiga = "Liga";
const char *gpszMsgDesliga = "Desliga";
const char *gpszMsgModo = "Modo";
const char *gpszMsgVelocidade1 = "Velocidade 1";
const char *gpszMsgVelocidade2 = "Velocidade 2";
const char *gpszMsgVelocidade3 = "Velocidade 3";
const char *gpszMsgVelocidade4 = "Velocidade 4";
const char *gpszMsgErroDesconhecido = "Erro Desconhecido";
const char *gpszMsgTempoCentrifugacao = "TEMPO CENTRIF.";
const char *gpszMsgHorarioCentrifugacao = "HORA CENTRIF.";
const char *gpszMsgSalvaConfiguracao = "SALVA CONFIG.";
const char *gpszMsgConfiguraPreset = "CONFIG. PRESET";
const char *gpzMsgPresetA = "Preset A - Tempo: 1min, Horario: 12:00:00";
const char *gpzMsgPresetB = "Preset B - Tempo: 5min, Horario: 12:00:00";
const char *gpzMsgPresetC = "Preset C - Tempo: 10min, Horario: 12:00:00";
const char *gpzMsgPresetD = "Preset D - Tempo: 15min, Horario: 12:00:00";
const char *gpszMsgHoraPreset = "12:00:00";
const char *gpszMsgTempoPresetA = "01:00";
const char *gpszMsgTempoPresetB = "05:00";
const char *gpszMsgTempoPresetC = "10:00";
const char *gpszMsgTempoPresetD = "15:00";
const char *gpszMsgHorarioLimpo = "Dados Horario Limpos - Valor Padrão 12:00:00";
const char *gpszMsgTempoLimpo = "Dados Tempo Limpos - Valor Padrão 01:00";
const char *gpszMsgPresetNDisp = "Preset somente pode ser selecionado fora do menu configuração\n"
                                 "Para sair do menu configuração pressione (*)\n"
                                 "Para Selecionar um dos valores Preset Pressione (A), (B), (C) ou (D)";
const char *gpszMsgNAlteraVel = "Não é possível alterar velocidade antes do processo de  centrifugação";

/* Global Variables ----------------------------------------------------------*/

/* Public objects ------------------------------------------------------------*/
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
TM1637Display segDisplay(SEG_CLK, SEG_DIO);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);
A4988 centrifuga(MOTOR_STEPS, MOTOR_DIR, MOTOR_EN);

st_buttons_t gstButtons[MAX_BUTTONS];
st_outputs_t gstOutputs[MAX_OUTPUTS];
st_centrifuga_t gstCentrifuga;

/* FreeRTOS Section (Tasks & Handles) ----------------------------------------*/

/* Queues --------------------------------------------------------------------*/
QueueHandle_t queue_rtc = NULL;
QueueHandle_t queue_rpm = NULL;
QueueHandle_t queue_keypad = NULL;

/* Event Groups --------------------------------------------------------------*/
EventGroupHandle_t event_group = NULL;

/* Tasks ---------------------------------------------------------------------*/
TaskHandle_t task_config_centrifuga = NULL;

static void vTaskAtualizaHoraData(void *pvParameters);
static void vTaskCheckButtons(void *pvParameters);
static void vTaskCheckRTC(void *pvParameters);
static void vTaskCheckKeypad(void *pvParameters);
static void vTaskCheckOutputs(void *pvParameters);
static void vTaskCheckCentrifuga(void *pvParameters);
static void vTaskConfigCentrifuga(void *pvParameters);
static void vTaskShowTempoCentrifugacao(void *pvParameters);

/* Function Prototypes -------------------------------------------------------*/
static void InitSerial();
static void InitOutputs();
static void InitButtons();
static void InitRTC();
static void InitDisplay();
static void InitSegDisplay();
static void InitCentrifuga();
static void InitFreeRTOS();
static void ResetaSistema(e_rtos_errors_t eRtosErrors, const char *psgMsgErro);
const char *pszGetButtonName(e_buttons_t eButton);
const char *pszGetOutputName(e_outputs_t eOutput);
static void SetLedVelocidadeAtual(st_outputs_t *pstOutputs, int iValue);
static void SetLedCentrifuga(st_outputs_t *pstOutputs, bool fState);
static void SetBuzzer(st_outputs_t *pstOutputs, bool fState);
static e_errors_t eSetHoraTempoCentrifuga(st_centrifuga_t *pstCentrifuga, const char *pszHora, const char *pszTempo, bool fCfgEmAndamento);
const char *pszGetErrorMsg(e_errors_t eError);
const char *pszGetRtosErrorMsg(e_rtos_errors_t eRtosError);
static void handleError(uint8_t uiCfgMode);

/* Setup ---------------------------------------------------------------------*/
void setup()
{
  /*Inicia Comunicação Serial*/
  InitSerial();

  /*Inicia Saídas*/
  InitOutputs();

  /*Inicia Entradas*/
  InitButtons();

  /*Inicia Display*/
  InitDisplay();

  /*Inicia 7Seg*/
  InitSegDisplay();

  /*Inicia RTC*/
  InitRTC();

  /*Inicia Motor de Passo*/
  InitCentrifuga();

  /*Inicia FreeRTOS (Queues, Event Groups and Tasks)*/
  InitFreeRTOS();
}

/* Loop ----------------------------------------------------------------------*/
void loop()
{
  /*Keep Alive Loop*/
  vTaskDelay(pdMS_TO_TICKS(10));
}

/* Tasks ---------------------------------------------------------------------*/
static void vTaskAtualizaHoraData(void *pvParameters)
{
  tmElements_t tm;
  char szData[24 + 1];
  char szHora[24 + 1];
  char szHorarioAtual[8 + 1];
  EventBits_t uxBits;
  for (;;)
  {
    if (xQueueReceive(queue_rtc, &tm, pdMS_TO_TICKS(1000)))
    {
      uxBits = xEventGroupWaitBits(event_group, BIT_CLEAR_DISPLAY, pdTRUE, pdFALSE, 0);
      if (uxBits & BIT_CLEAR_DISPLAY)
      {
        lcd.clear();
      }
      memset(szData, 0, sizeof(szData));
      memset(szHora, 0, sizeof(szHora));
      memset(szHorarioAtual, 0, sizeof(szHorarioAtual));
      snprintf(szHorarioAtual, sizeof(szHorarioAtual), "%02d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);

      /*Verifica se deve ativar centrifugacao auto*/
      if (!strcmp(szHorarioAtual, gstCentrifuga.szHorarioCentrifugacao) &&
          !gstCentrifuga.fModoCentrifugacao && gstCentrifuga.fHabilitaCentrifugacao)
      {
        gstCentrifuga.fCentrifugacaoAtiva = true;
        /*Por padrão considera velocidade inicial 1*/
        xQueueSend(queue_rpm, &uiRpmCentrifuga[VELOCIDADE_1], 0);
        SetLedVelocidadeAtual(gstOutputs, VELOCIDADE_1);
      }

      snprintf(szHora, sizeof(szHora), "Hora: %s", szHorarioAtual);
      snprintf(szData, sizeof(szData), "Data: %02d/%02d/%02d", tm.Day, tm.Month, tm.Year - 30);
      lcd.setCursor(0, 0);
      lcd.print(szData);
      lcd.setCursor(0, 1);
      lcd.print(szHora);
      lcd.setCursor(15, 0);
      lcd.write(!gstCentrifuga.fHabilitaCentrifugacao ? SISTEMA_OFF : SISTEMA_ON);
      lcd.setCursor(15, 1);
      lcd.write(!gstCentrifuga.fModoCentrifugacao ? CENTRIFUGA_MODO_AUTO : CENTRIFUGA_MODO_MANUAL);
    }
    else
    {
      /*Em caso de erro habilita limpar display*/
      xEventGroupSetBits(event_group, BIT_CLEAR_DISPLAY);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hora: --:--:--");
      lcd.setCursor(0, 1);
      lcd.print("Data: --/--/--");
      lcd.setCursor(15, 0);
      lcd.write(!gstCentrifuga.fHabilitaCentrifugacao ? SISTEMA_OFF : SISTEMA_ON);
      lcd.setCursor(15, 1);
      lcd.write(!gstCentrifuga.fModoCentrifugacao ? CENTRIFUGA_MODO_AUTO : CENTRIFUGA_MODO_MANUAL);
    }
  }
}

static void vTaskCheckButtons(void *pvParameters)
{
  bool fState = false;
  for (;;)
  {
    for (int i = 0; i < MAX_BUTTONS; i++)
    {
      fState = digitalRead(gstButtons->eButton[i]);
      if (fState != gstButtons->fState[i])
      {
        gstButtons->fState[i] = fState;
        if (!fState)
        {
          gstButtons->fPressed[i] = true;
          Serial.println("Botão Pressionado: " + String(pszGetButtonName((e_buttons_t)i)));
          xEventGroupSetBits(event_group, (1 << (i + 1)));
        }
      }
      gstButtons->fState[i] = fState;
    }
    /*Debounce*/
    vTaskDelay(pdMS_TO_TICKS(25));
  }
}

static void vTaskCheckKeypad(void *pvParameters)
{
  char cKey = 0;
  for (;;)
  {
    cKey = keypad.getKey();
    if (cKey)
    {
      Serial.println("Tecla Pressionada: " + String(cKey));
      xQueueSend(queue_keypad, &cKey, 0);
      vTaskDelay(pdMS_TO_TICKS(25));
    }
  }
}

static void vTaskCheckRTC(void *pvParameters)
{
  tmElements_t tm;
  for (;;)
  {
    if (!RTC.read(tm))
    {
      Serial.println(gpszMsgFalhaRTC);
    }
    else
    {
      xQueueSend(queue_rtc, &tm, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

static void vTaskCheckOutputs(void *pvParameters)
{
  EventBits_t uxBits;
  for (;;)
  {
    uxBits = xEventGroupWaitBits(event_group,
                                 (BIT_BUTTON_LIGA |
                                  BIT_BUTTON_DESLIGA |
                                  BIT_BUTTON_MODO |
                                  BIT_BUTTON_VELOCIDADE_1 |
                                  BIT_BUTTON_VELOCIDADE_2 |
                                  BIT_BUTTON_VELOCIDADE_3 |
                                  BIT_BUTTON_VELOCIDADE_4),
                                 pdTRUE, pdFALSE, portMAX_DELAY);

    if (uxBits & BIT_BUTTON_LIGA)
    {
      Serial.println(gpszMsgSistemaLigado);
      /*Habilita Centrifugação*/
      gstCentrifuga.fHabilitaCentrifugacao = true;
      /*Verifica Modo Centrifugação e habilita Centrifugação Ativa*/
      if (gstCentrifuga.fModoCentrifugacao && gstCentrifuga.fHabilitaCentrifugacao)
      {
        /*Habilita Centrifugação*/
        gstCentrifuga.fCentrifugacaoAtiva = true;
        /*Inicia na velocidade 1*/
        xQueueSend(queue_rpm, &uiRpmCentrifuga[VELOCIDADE_1], 0);
        SetLedVelocidadeAtual(gstOutputs, VELOCIDADE_1);
      }
      /*Desabilita Task de Configuração*/
      vTaskSuspend(task_config_centrifuga);
    }
    else if (uxBits & BIT_BUTTON_DESLIGA)
    {
      Serial.println(gpszMsgSistemaDesligado);
      SetLedVelocidadeAtual(gstOutputs, 0);
      xQueueSend(queue_rpm, &uiRpmCentrifuga[VELOCIDADE_0], 0);
      if (gstCentrifuga.fCentrifugacaoAtiva)
      {
        /*Finaliza processo de centrifugação*/
        gstCentrifuga.fCentrifugacaoFinalizada = true;
        gstCentrifuga.fCentrifugacaoAtiva = false;
      }

      /*Desabilita Centrifugação*/
      gstCentrifuga.fHabilitaCentrifugacao = false;
      /*Habilita Task de Configuração*/
      vTaskResume(task_config_centrifuga);
    }
    else if (uxBits & BIT_BUTTON_MODO)
    {
      /*Inverte Modo Centrifugação*/
      gstCentrifuga.fModoCentrifugacao = !gstCentrifuga.fModoCentrifugacao;
      /*Verifica Modo Centrifugação e habilita Centrifugação Ativa*/
      if (gstCentrifuga.fModoCentrifugacao && gstCentrifuga.fHabilitaCentrifugacao)
      {
        /*Habilita Centrifugação*/
        gstCentrifuga.fCentrifugacaoAtiva = true;
        /*Inicia na velocidade 1*/
        xQueueSend(queue_rpm, &uiRpmCentrifuga[VELOCIDADE_1], 0);
        SetLedVelocidadeAtual(gstOutputs, VELOCIDADE_1);
      }
      Serial.println("Modo Centrifugação: " + String(gstCentrifuga.fModoCentrifugacao ? "Manual" : "Automático"));
    }
    else if (uxBits & BIT_BUTTON_VELOCIDADE_1)
    {
      if (gstCentrifuga.fCentrifugacaoAtiva)
      {
        Serial.println(gpszMsgVelocidade1);
        SetLedVelocidadeAtual(gstOutputs, BIT_OUTPUT_VELOCIDADE_1);
        xQueueSend(queue_rpm, &uiRpmCentrifuga[VELOCIDADE_1], 0);
      }
      else
      {
        Serial.println(gpszMsgNAlteraVel);
      }
    }
    else if (uxBits & BIT_BUTTON_VELOCIDADE_2)
    {
      if (gstCentrifuga.fCentrifugacaoAtiva)
      {
        Serial.println(gpszMsgVelocidade2);
        SetLedVelocidadeAtual(gstOutputs, BIT_OUTPUT_VELOCIDADE_2);
        xQueueSend(queue_rpm, &uiRpmCentrifuga[VELOCIDADE_2], 0);
      }
      else
      {
        Serial.println(gpszMsgNAlteraVel);
      }
    }
    else if (uxBits & BIT_BUTTON_VELOCIDADE_3)
    {
      if (gstCentrifuga.fCentrifugacaoAtiva)
      {
        Serial.println(gpszMsgVelocidade3);
        SetLedVelocidadeAtual(gstOutputs, BIT_OUTPUT_VELOCIDADE_3);
        xQueueSend(queue_rpm, &uiRpmCentrifuga[VELOCIDADE_3], 0);
      }
      else
      {
        Serial.println(gpszMsgNAlteraVel);
      }
    }
    else if (uxBits & BIT_BUTTON_VELOCIDADE_4)
    {
      if (gstCentrifuga.fCentrifugacaoAtiva)
      {
        Serial.println(gpszMsgVelocidade4);
        SetLedVelocidadeAtual(gstOutputs, BIT_OUTPUT_VELOCIDADE_4);
        xQueueSend(queue_rpm, &uiRpmCentrifuga[VELOCIDADE_4], 0);
      }
      else
      {
        Serial.println(gpszMsgNAlteraVel);
      }
    }
    else
    {
      Serial.println(gpszMsgErroDesconhecido);
      xQueueSend(queue_rpm, &uiRpmCentrifuga[VELOCIDADE_0], 0);
      SetLedVelocidadeAtual(gstOutputs, 0);
    }

    vTaskDelay(pdMS_TO_TICKS(25));
  }
}

static void vTaskCheckCentrifuga(void *pvParameters)
{
  bool fEnable = false;
  for (;;)
  {
    if (xQueueReceive(queue_rpm, &gstCentrifuga.uiRpmCentrifugacao, 0))
    {
      /*Somente Habilita Centrifugação se flag ativa é true*/
      if (gstCentrifuga.fCentrifugacaoAtiva)
      {
        (gstCentrifuga.uiRpmCentrifugacao > 0) ? fEnable = true : fEnable = false;
      }
      else
      {
        fEnable = false;
      }

      Serial.println("RPM: " + String(gstCentrifuga.uiRpmCentrifugacao));
      centrifuga.setRPM(gstCentrifuga.uiRpmCentrifugacao);
    }
    if (fEnable)
    {
      SetLedCentrifuga(gstOutputs, true);
      centrifuga.rotate(360);
    }
    else
    {
      SetLedCentrifuga(gstOutputs, false);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static void vTaskConfigCentrifuga(void *pvParameters)
{
  char cKey = 0;
  char szHorarioAux[8 + 1], szTempoAux[6 + 1];
  uint8_t uiContador = 0, uiCfgMode = 0;
  int i = 0;
  e_errors_t eError = SYS_OK;
  bool fConfigEmAndamento = false, fTeclaValida = false;

  memset(szHorarioAux, 0, sizeof(szHorarioAux));
  memset(szTempoAux, 0, sizeof(szTempoAux));

  strncpy(szHorarioAux, gpszMsgHoraPreset, sizeof(szHorarioAux) - 1);
  strncpy(szTempoAux, gpszMsgTempoPresetA, sizeof(szTempoAux) - 1);

  for (;;)
  {
    if (xQueueReceive(queue_keypad, &cKey, portMAX_DELAY))
    {
      /*Checa Tecla Pressionada*/
      switch (cKey)
      {
      case KEYPAD_CHAR_HASH:
        switch (uiContador)
        {
        case 0:
          fConfigEmAndamento = true;
          uiCfgMode = 0;
          i = 0;
          uiContador++;
          Serial.println(gpszMsgHorarioCentrifugacao);
          break;
        case 1:
          fConfigEmAndamento = true;
          uiCfgMode = 1;
          i = 0;
          uiContador++;
          Serial.println(gpszMsgTempoCentrifugacao);
          break;
        case 2:
          fConfigEmAndamento = false;
          uiContador = 0;
          uiCfgMode = 0;
          i = 0;
          Serial.println(gpszMsgSalvaConfiguracao);
          eError = eSetHoraTempoCentrifuga(&gstCentrifuga, szHorarioAux, szTempoAux, fConfigEmAndamento);
          if (eError != SYS_OK)
          {
            Serial.println("Erro ao Configurar Horario e Tempo Centrifugação: " + String(pszGetErrorMsg(eError)));
          }
          break;
        }
        break;
      case KEYPAD_CHAR_STAR:
        /*Exibe horário e tempo de centrifugação configurados atualmente*/
        if (!fConfigEmAndamento)
        {
          Serial.println("Horario Centrifugação Atual: " + String(gstCentrifuga.szHorarioCentrifugacao));
          Serial.println("Tempo Centrifugação Atual: " + String(gstCentrifuga.szTempoCentrifugacao));
          break;
        }
        /*Com configuração em andamento - retorna ao inicio/ sai sem alterar horário*/
        i = 0;
        if (uiCfgMode == 0)
        {
          if (!strcmp(szHorarioAux, gpszMsgHoraPreset))
          {
            Serial.println("Saindo da Configuração sem Alterar Horario");
            fConfigEmAndamento = false;
            uiContador = 0;
            uiCfgMode = 0;
            i = 0;
          }
          else
          {
            memset(szHorarioAux, 0, sizeof(szHorarioAux));
            strncpy(szHorarioAux, gpszMsgHoraPreset, sizeof(szHorarioAux) - 1);
            Serial.println(gpszMsgHorarioLimpo);
          }
        }
        else if (uiCfgMode == 1)
        {
          if (!strcmp(szTempoAux, gpszMsgTempoPresetA))
          {
            Serial.println("Saindo da Configuração sem Alterar Tempo e Horario");
            fConfigEmAndamento = false;
            uiContador = 0;
            uiCfgMode = 0;
            i = 0;
          }
          else
          {
            memset(szTempoAux, 0, sizeof(szTempoAux));
            strncpy(szTempoAux, gpszMsgTempoPresetA, sizeof(szTempoAux) - 1);
            Serial.println(gpszMsgTempoLimpo);
          }
        }
        break;

      /*Seta Valores Pressetados*/
      case KEYPAD_CHAR_A:
        eError = eSetHoraTempoCentrifuga(&gstCentrifuga, gpszMsgHoraPreset, gpszMsgTempoPresetA, fConfigEmAndamento);
        if (eError != SYS_OK)
        {
          Serial.println("Erro ao Configurar Horario e Tempo Centrifugação: " + String(pszGetErrorMsg(eError)));
        }
        break;
      case KEYPAD_CHAR_B:
        eError = eSetHoraTempoCentrifuga(&gstCentrifuga, gpszMsgHoraPreset, gpszMsgTempoPresetB, fConfigEmAndamento);
        if (eError != SYS_OK)
        {
          Serial.println("Erro ao Configurar Horario e Tempo Centrifugação: " + String(pszGetErrorMsg(eError)));
        }
        break;
      case KEYPAD_CHAR_C:
        eError = eSetHoraTempoCentrifuga(&gstCentrifuga, gpszMsgHoraPreset, gpszMsgTempoPresetC, fConfigEmAndamento);
        if (eError != SYS_OK)
        {
          Serial.println("Erro ao Configurar Horario e Tempo Centrifugação: " + String(pszGetErrorMsg(eError)));
        }
        break;
      case KEYPAD_CHAR_D:
        eError = eSetHoraTempoCentrifuga(&gstCentrifuga, gpszMsgHoraPreset, gpszMsgTempoPresetD, fConfigEmAndamento);
        if (eError != SYS_OK)
        {
          Serial.println("Erro ao Configurar Horario e Tempo Centrifugação: " + String(pszGetErrorMsg(eError)));
        }
        break;
      default:
        if (fConfigEmAndamento)
        {
          switch (uiCfgMode)
          {
          /*Configura Horário e Realiza Validações*/
          case 0:
            fTeclaValida = true;
            switch (i)
            {
            case 0:
              if (cKey > '2')
              {
                fTeclaValida = false;
              }
              break;
            case 1:
              if ((szHorarioAux[0] == '2' && cKey > '3') || cKey > '9')
              {
                fTeclaValida = false;
              }
              break;
            case 3:
            case 6:
              if (cKey > '5')
              {
                fTeclaValida = false;
              }
              break;
            case 4:
            case 7:
              if (cKey > '9')
              {
                fTeclaValida = false;
              }
              break;
            }

            if (fTeclaValida)
            {
              szHorarioAux[i] = cKey;
              i++;
              if (i == 2 || i == 5)
              {
                szHorarioAux[i] = ':';
                i++;
              }
              Serial.println("Cfg Horario: " + String(szHorarioAux));
            }
            else
            {
              handleError(uiCfgMode);
            }
            break;

          /*Configura Tempo e Realiza Validações*/
          case 1:
            fTeclaValida = true;
            switch (i)
            {
            case 0:
            case 3:
              if (cKey > '5')
              {
                fTeclaValida = false;
              }
            case 1:
            case 4:
              if (cKey > '9')
              {
                fTeclaValida = false;
              }
              break;
            }
            if (fTeclaValida)
            {
              szTempoAux[i] = cKey;
              i++;
              if (i == 2)
              {
                szTempoAux[i] = ':';
                i++;
              }
              Serial.println("Cfg Tempo: " + String(szTempoAux));
            }
            else
            {
              handleError(uiCfgMode);
            }
            break;
          default:
            Serial.println(gpszMsgErroDesconhecido);
            break;
          }
        }
        break;
      }
    }
  }
}

static void vTaskShowTempoCentrifugacao(void *pvParameters)
{
  uint8_t uiMinutos = 0, uiSegundos = 0;
  bool fInicioCentrifugacao = false;
  char szAux[2 + 1];
  int i;
  for (;;)
  {
    if (gstCentrifuga.fCentrifugacaoAtiva && !gstCentrifuga.fCentrifugacaoFinalizada)
    {
      if (!fInicioCentrifugacao)
      {
        fInicioCentrifugacao = true;

        memset(szAux, 0, sizeof(szAux));
        strncpy(szAux, gstCentrifuga.szTempoCentrifugacao, 2);
        uiMinutos = atoi(szAux);
        memset(szAux, 0, sizeof(szAux));
        strncpy(szAux, &gstCentrifuga.szTempoCentrifugacao[3], 2);
        uiSegundos = atoi(szAux);
      }
      /*Decrementa Tempo somente com sistema ligado, caso contrário mantém relógio parado*/
      if (uiMinutos > 0 && uiSegundos == 0)
      {
        uiMinutos--;
        uiSegundos = 59;
      }
      if (uiSegundos > 0)
      {
        uiSegundos--;
      }
      /*Finaliza Centrifugação*/
      else
      {
        gstCentrifuga.fCentrifugacaoAtiva = false;
        gstCentrifuga.fCentrifugacaoFinalizada = true;
      }

      segDisplay.showNumberDecEx(uiMinutos, SEG_DP, true, 2, 0);
      segDisplay.showNumberDec(uiSegundos, true, 2, 2);
    }
    else if (gstCentrifuga.fCentrifugacaoFinalizada && !gstCentrifuga.fCentrifugacaoAtiva)
    {
      xQueueSend(queue_rpm, &uiRpmCentrifuga[VELOCIDADE_0], 0);
      SetLedVelocidadeAtual(gstOutputs, VELOCIDADE_0);

      segDisplay.setSegments(SEG_DONE);
      for (i = 0; i < 6; i++)
      {
        SetBuzzer(gstOutputs, !gstOutputs->fState[OUTPUT_BUZZER]);
        vTaskDelay(pdMS_TO_TICKS(1000));
      }
      /* Limpa Variáveis e Flags */
      i = 0;
      uiMinutos = 0;
      uiSegundos = 0;
      gstCentrifuga.fCentrifugacaoFinalizada = false;
      gstCentrifuga.fCentrifugacaoAtiva = false;
      gstCentrifuga.fModoCentrifugacao = CENTRIFUGA_MODO_AUTO;
    }
    else
    {
      segDisplay.showNumberDecEx(0, 0b01000000, true);
      fInicioCentrifugacao = false;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
/* Functions -----------------------------------------------------------------*/
static void InitSerial()
{
  Serial.begin(BAUD_RATE);
  Serial.println(gpszMsgInstrucaoSistema);
}

static void InitOutputs()
{
  int i = 0;
  gstOutputs->eOutput[OUTPUT_VELOCIDADE_1] = (e_outputs_t)LED_VELOCIDADE_1;
  gstOutputs->eOutput[OUTPUT_VELOCIDADE_2] = (e_outputs_t)LED_VELOCIDADE_2;
  gstOutputs->eOutput[OUTPUT_VELOCIDADE_3] = (e_outputs_t)LED_VELOCIDADE_3;
  gstOutputs->eOutput[OUTPUT_VELOCIDADE_4] = (e_outputs_t)LED_VELOCIDADE_4;
  gstOutputs->eOutput[OUTPUT_PROCESSO_CENTRIFUGACAO] = (e_outputs_t)LED_PROCESSO_CENTRIFUGACAO;
  gstOutputs->eOutput[OUTPUT_BUZZER] = (e_outputs_t)BUZZER;
  for (i = 0; i < MAX_OUTPUTS; i++)
  {
    gstOutputs->fState[i] = false;
    pinMode(gstOutputs->eOutput[i], OUTPUT);
    digitalWrite(gstOutputs->eOutput[i], LOW);
  }
}

static void InitButtons()
{
  int i = 0;
  gstButtons->eButton[BUTTON_LIGA] = (e_buttons_t)BTN_LIGA;
  gstButtons->eButton[BUTTON_DESLIGA] = (e_buttons_t)BTN_DESLIGA;
  gstButtons->eButton[BUTTON_MODO] = (e_buttons_t)BTN_MODO;
  gstButtons->eButton[BUTTON_VELOCIDADE_1] = (e_buttons_t)BTN_VELOCIDADE_1;
  gstButtons->eButton[BUTTON_VELOCIDADE_2] = (e_buttons_t)BTN_VELOCIDADE_2;
  gstButtons->eButton[BUTTON_VELOCIDADE_3] = (e_buttons_t)BTN_VELOCIDADE_3;
  gstButtons->eButton[BUTTON_VELOCIDADE_4] = (e_buttons_t)BTN_VELOCIDADE_4;

  for (i = 0; i < MAX_BUTTONS; i++)
  {
    gstButtons->fState[i] = false;
    gstButtons->fPressed[i] = false;
    gstButtons->iMode[i] = INPUT_PULLUP;
    pinMode(gstButtons->eButton[i], gstButtons->iMode[i]);
  }
}

static void InitRTC()
{
  tmElements_t tm;
  char szDataHora[20 + 1];

  if (RTC.read(tm))
  {
    snprintf(szDataHora, sizeof(szDataHora), "%02d/%02d/%02d %02d:%02d:%02d",
             tm.Day, tm.Month, tm.Year - 30, tm.Hour, tm.Minute, tm.Second);
    Serial.println(szDataHora);
  }
  else
  {
    Serial.println(gpszMsgFalhaRTC);
    if (RTC.chipPresent())
    {
      Serial.println(gpszMsgRTCInicio);
    }
    else
    {
      Serial.println(gpszMsgSemRTC);
    }
    Serial.println(gpszMsgFalhaRTC);
  }
}

static void InitDisplay()
{
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, modoAuto);
  lcd.createChar(1, modoManual);
  lcd.createChar(2, sistemaOn);
  lcd.createChar(3, sistemaOff);
  lcd.setCursor(3, 0);
  lcd.print(gpszMsgBemVindo);
  lcd.setCursor(0, 1);
  lcd.print(gpszMsgIniciaSistema);
}

static void InitSegDisplay()
{
  segDisplay.setBrightness(0x0f);
  segDisplay.showNumberDecEx(0, 0b01000000, true);
}

static void InitCentrifuga()
{
  memset(&gstCentrifuga, 0, sizeof(gstCentrifuga));
  gstCentrifuga.uiRpmCentrifugacao = RPM_VELOCIDADE_0;
  strncpy(gstCentrifuga.szHorarioCentrifugacao, gpszMsgHoraPreset,
          sizeof(gstCentrifuga.szHorarioCentrifugacao) - 1);
  strncpy(gstCentrifuga.szTempoCentrifugacao, gpszMsgTempoPresetA,
          sizeof(gstCentrifuga.szTempoCentrifugacao) - 1);
  centrifuga.begin(gstCentrifuga.uiRpmCentrifugacao, MICRO_STEPS);
  centrifuga.enable();
}

static void InitFreeRTOS()
{

  /*Configura Semáforos - FreeRTOS*/

  /*Configura Filas - FreeRTOS*/
  queue_rtc = xQueueCreate(1, sizeof(tmElements_t));
  queue_rpm = xQueueCreate(1, sizeof(uint8_t));
  queue_keypad = xQueueCreate(1, sizeof(char));

  if (queue_rtc == NULL || queue_rpm == NULL || queue_keypad == NULL)
    ResetaSistema(RTOS_ERROR_CREATE_QUEUE, gpszErroCriacaoFila);

  /*Configura Grupo de Eventos - FreeRTOS*/
  event_group = xEventGroupCreate();

  if (event_group == NULL)
    ResetaSistema(RTOS_ERROR_CREATE_EVENT_GROUP, gpszErroCriacaoGrupoEventos);

  /*Seta Bit Limpa Display*/
  xEventGroupSetBits(event_group, BIT_CLEAR_DISPLAY);

  /*Configura Tarefas - FreeRTOS*/
  if (xTaskCreate(vTaskAtualizaHoraData, "vTaskAtualizaHoraData", 512, NULL, 1, NULL) != pdTRUE)
    ResetaSistema(RTOS_ERROR_CREATE_TASK, gpszErroCriacaoTarefa);

  if (xTaskCreate(vTaskCheckButtons, "vTaskCheckButtons", 128, NULL, 1, NULL) != pdTRUE)
    ResetaSistema(RTOS_ERROR_CREATE_TASK, gpszErroCriacaoTarefa);

  if (xTaskCreate(vTaskCheckRTC, "vTaskCheckRTC", 128, NULL, 1, NULL) != pdTRUE)
    ResetaSistema(RTOS_ERROR_CREATE_TASK, gpszErroCriacaoTarefa);

  if (xTaskCreate(vTaskCheckKeypad, "vTaskCheckKeypad", 128, NULL, 1, NULL) != pdTRUE)
    ResetaSistema(RTOS_ERROR_CREATE_TASK, gpszErroCriacaoTarefa);

  if (xTaskCreate(vTaskCheckOutputs, "vTaskCheckOutputs", 128, NULL, 1, NULL) != pdTRUE)
    ResetaSistema(RTOS_ERROR_CREATE_TASK, gpszErroCriacaoTarefa);

  if (xTaskCreate(vTaskCheckCentrifuga, "vTaskCheckCentrifuga", 128, NULL, 1, NULL) != pdTRUE)
    ResetaSistema(RTOS_ERROR_CREATE_TASK, gpszErroCriacaoTarefa);

  if (xTaskCreate(vTaskConfigCentrifuga, "vTaskConfigCentrifuga", 128, NULL, 1, &task_config_centrifuga) != pdTRUE)
    ResetaSistema(RTOS_ERROR_CREATE_TASK, gpszErroCriacaoTarefa);

  if (xTaskCreate(vTaskShowTempoCentrifugacao, "vTaskShowTempoCentrifugacao", 128, NULL, 1, NULL) != pdTRUE)
    ResetaSistema(RTOS_ERROR_CREATE_TASK, gpszErroCriacaoTarefa);
}

static void ResetaSistema(e_rtos_errors_t eRtosErrors, const char *psgMsgErro)
{

  switch (eRtosErrors)
  {
  case RTOS_ERROR_CREATE_TASK:
  case RTOS_ERROR_CREATE_SEMAPHORE:
  case RTOS_ERROR_CREATE_QUEUE:
  case RTOS_ERROR_CREATE_EVENT_GROUP:
    Serial.println(psgMsgErro + String(pszGetRtosErrorMsg(eRtosErrors)));
    break;
  default:
    return;
  }
  Serial.println("Reiniciando Sistema");
  vTaskDelay(pdMS_TO_TICKS(1000));
  asm volatile("  jmp 0");
}

const char *pszGetButtonName(e_buttons_t eButton)
{
  switch (eButton)
  {
  case BUTTON_LIGA:
    return gpszMsgLiga;
  case BUTTON_DESLIGA:
    return gpszMsgDesliga;
  case BUTTON_MODO:
    return gpszMsgModo;
  case BUTTON_VELOCIDADE_1:
    return gpszMsgVelocidade1;
  case BUTTON_VELOCIDADE_2:
    return gpszMsgVelocidade2;
  case BUTTON_VELOCIDADE_3:
    return gpszMsgVelocidade3;
  case BUTTON_VELOCIDADE_4:
    return gpszMsgVelocidade4;
  default:
    return gpszMsgErroDesconhecido;
  }
}
const char *pszGetOutputName(e_outputs_t eOutput)
{
  switch (eOutput)
  {
  case OUTPUT_VELOCIDADE_1:
    return gpszMsgVelocidade1;
  case OUTPUT_VELOCIDADE_2:
    return gpszMsgVelocidade2;
  case OUTPUT_VELOCIDADE_3:
    return gpszMsgVelocidade3;
  case OUTPUT_VELOCIDADE_4:
    return gpszMsgVelocidade4;
  case OUTPUT_BUZZER:
    return gpszMsgBuzzer;
  default:
    return gpszMsgErroDesconhecido;
  }
}

static void SetLedVelocidadeAtual(st_outputs_t *pstOutputs, int iValue)
{
  int i = 0;
  for (i = 0; i < MAX_OUTPUTS - 2; i++)
  {
    if (iValue & (1 << i))
    {
      pstOutputs->fState[i] = true;
      digitalWrite(pstOutputs->eOutput[i], HIGH);
    }
    else
    {
      pstOutputs->fState[i] = false;
      digitalWrite(pstOutputs->eOutput[i], LOW);
    }
  }
}

static void SetLedCentrifuga(st_outputs_t *pstOutputs, bool fState)
{
  pstOutputs->fState[OUTPUT_PROCESSO_CENTRIFUGACAO] = fState;
  digitalWrite(pstOutputs->eOutput[OUTPUT_PROCESSO_CENTRIFUGACAO], fState);
}

static void SetBuzzer(st_outputs_t *pstOutputs, bool fState)
{
  pstOutputs->fState[OUTPUT_BUZZER] = fState;
  digitalWrite(pstOutputs->eOutput[OUTPUT_BUZZER], fState);
  Serial.println("Buzzer: " + String(fState ? "Ligado" : "Desligado"));
}

static e_errors_t eSetHoraTempoCentrifuga(st_centrifuga_t *pstCentrifuga, const char *pszHora, const char *pszTempo, bool fCfgEmAndamento)
{

  if (fCfgEmAndamento)
  {
    Serial.println(gpszMsgPresetNDisp);
    return SYS_OK;
  }

  if (pstCentrifuga == NULL || pszHora == NULL || pszTempo == NULL)
  {
    return SYS_ERROR_INV_PARAM;
  }

  if ((strlen(pszHora) >= sizeof(pstCentrifuga->szHorarioCentrifugacao)) ||
      (strlen(pszTempo) >= sizeof(pstCentrifuga->szTempoCentrifugacao)))
  {
    return SYS_ERROR_PARAM_OVERFLOW;
  }

  /*Seta Hora*/
  strncpy(pstCentrifuga->szHorarioCentrifugacao, pszHora, sizeof(pstCentrifuga->szHorarioCentrifugacao) - 1);
  pstCentrifuga->szHorarioCentrifugacao[sizeof(pstCentrifuga->szHorarioCentrifugacao) - 1] = '\0';

  /*Seta Tempo*/
  strncpy(pstCentrifuga->szTempoCentrifugacao, pszTempo, sizeof(pstCentrifuga->szTempoCentrifugacao) - 1);
  pstCentrifuga->szTempoCentrifugacao[sizeof(pstCentrifuga->szTempoCentrifugacao) - 1] = '\0';

  Serial.print("Horario Centrifugacao: " + String(pstCentrifuga->szHorarioCentrifugacao));
  Serial.println("| Tempo Centrifugacao: " + String(pstCentrifuga->szTempoCentrifugacao));

  return SYS_OK;
}

const char *pszGetErrorMsg(e_errors_t eError)
{
  switch (eError)
  {
  case SYS_OK:
    return "SYS_OK";
  case SYS_ERROR:
    return "SYS_ERROR";
  case SYS_ERROR_INV_PARAM:
    return "SYS_ERROR_INV_PARAM";
  case SYS_ERROR_PARAM_OVERFLOW:
    return "SYS_ERROR_PARAM_OVERFLOW";
  default:
    return "SYS_ERROR";
  }
}

const char *pszGetRtosErrorMsg(e_rtos_errors_t eRtosError)
{
  switch (eRtosError)
  {
  case RTOS_OK:
    return "RTOS_OK";
  case RTOS_ERROR:
    return "RTOS_ERROR";
  case RTOS_ERROR_CREATE_TASK:
    return "RTOS_ERROR_CREATE_TASK";
  case RTOS_ERROR_CREATE_SEMAPHORE:
    return "RTOS_ERROR_CREATE_SEMAPHORE";
  case RTOS_ERROR_CREATE_QUEUE:
    return "RTOS_ERROR_CREATE_QUEUE";
  case RTOS_ERROR_CREATE_EVENT_GROUP:
    return "RTOS_ERROR_CREATE_EVENT_GROUP";
  default:
    return "RTOS_ERROR";
  }
}

static void handleError(uint8_t uiCfgMode)
{
  (!uiCfgMode) ? Serial.println("Tecla Inválida ao Configurar Horario: " + String(pszGetErrorMsg(SYS_ERROR_INV_PARAM)))
               : Serial.println("Tecla Inválida ao Configurar Tempo: " + String(pszGetErrorMsg(SYS_ERROR_INV_PARAM)));
}
