/*

Nome: Yhan Christian Souza Silva
Data: 22/09/2023

Passo 1: Monte um circuito contento 1 TMP36, 3 botão, 4 leds, e 1 buzzer.
Passo 2: Implemente um programa para monitorar a temperatura e fornecer informações sobre sua
variação no ambiente.
✔ Os leds deverão ser usados para indicar a quantidade de vezes que os botões (b1, b2 e b3) foram
pressionados (represente nos leds o valor em binário: 0-F ). Lembre-se de fazer o tratamento do
efeito de bounce.
✔ Quando a contagem (b1+b2+b3) ultrapassar o valor decimal igual a 15, os leds devem piscar 3
vezes, sendo 1 segundos ligado e 1 segundo desligado .
✔ Após os leds piscarem 3 vezes, o contador deve ser zerado e os leds devem retornar ao valor
inicial (todos apagados).
✔ As medições de temperatura devem ser realizadas a cada 1 segundo.
✔ Independente dos botões, o buzzer deve apitar a cada 3 segundos (sendo 0,5 segundo ligado e
0,5 segundo desligado) quando a temperatura medida for superior ao valor limite estabelecido.
✔ Quando a temperatura estiver abaixo do limite estabelecido, o buzzer deve permanecer
desligados.
Passo 3: Defina as funções de cada botão no circuito.
✔ Quando o botão 1 for pressionado deve ser mostrado no monitor serial o menor valor de
temperatura mensurado durante o experimento;
✔ Quando o botão 2 for pressionado deve ser mostrado no monitor serial a temperatura média
medida nos últimos 30 segundos (as medições devem ser feitas a cada 1 segundo);
✔ Quando a chave 3 for pressionada deve ser mostrado no monitor serial a temperatura máxima
mensurada durante o experimento.

Observação: Para realizar a atividade utilize o sensor de temperatura TMP36 disponível no
Tinkercad ao invés do LM35 (indisponível no Tinkercad). Pesquise uma forma adequada de
realização do cálculo de temperatura utilizando o TMP36 para implementar o seu código.

*/

/* Include -------------------------------------------------------------------*/
#include <Arduino.h>

/* Define --------------------------------------------------------------------*/

/* SERIAL --------------------------------------------------------------------*/
#define BAUD_RATE 9600

/* INPUTS --------------------------------------------------------------------*/
#define SENSOR_TEMP A0
#define GPIO_BTN_A 2
#define GPIO_BTN_B 3
#define GPIO_BTN_C 4

/* OUTPUTS -------------------------------------------------------------------*/
#define GPIO_BUZZER 6
#define GPIO_LED_A 8
#define GPIO_LED_B 9
#define GPIO_LED_C 10
#define GPIO_LED_D 11

/* DEFINES GERAIS PROJETO ----------------------------------------------------*/
#define MAX_LEITURAS 30
#define MAX_TEMPERATURA 35.0
#define MAX_CONTADOR 15

/* Typedef -------------------------------------------------------------------*/

/*Callbacks*/
typedef void (*pvButtonCallback)(void);

/*Enum - Botões*/
typedef enum
{
  BUTTON_A = 0,
  BUTTON_B,
  BUTTON_C,
  MAX_BUTTONS
} e_buttons_t;

/*Enum - Leds*/
enum
{
  LED_A = 0,
  LED_B,
  LED_C,
  LED_D,
  MAX_LEDS
};

/*Enum - Timers*/
typedef enum
{
  TIMER_DEBOUNCE_BTN_A,
  TIMER_DEBOUNCE_BTN_B,
  TIMER_DEBOUNCE_BTN_C,
  TIMER_1_SEG,
  TIMER_BLINK_LED,
  TIMER_BUZZER,
  MAX_TIMERS
} e_timeout_t;

/*Enum - Tempos Timers*/
enum
{
  TEMPO_TIMER_DEBOUNCE_BTN_A = 50,
  TEMPO_TIMER_DEBOUNCE_BTN_B = 50,
  TEMPO_TIMER_DEBOUNCE_BTN_C = 50,
  TEMPO_TIMER_1_SEG = 1000,
  TEMPO_TIMER_BLINK_LED = 1000,
  TEMPO_TIMER_BUZZER = 500,
};

/* Struct Buttons*/
typedef struct
{
  int *piButtonsPins;
  int iNumButtons;
  pvButtonCallback pvCallbackBtnPressed[MAX_BUTTONS];
} st_buttons_t;

/* Struct Leds*/
typedef struct
{
  int *piLedsPins;
  int iNumLeds;
} st_leds_t;

/* Struct Timers*/
typedef struct
{
  unsigned long ulTimers[MAX_TIMERS];
  bool fTimerExpirado[MAX_TIMERS];
} st_timers_t;

/* Struct Dados Temperatura*/
typedef struct
{
  float fTempMin;
  float fTempMax;
  float fUltimoValor;
  float fLeiturasTemp[MAX_LEITURAS];
  float fTempMedia;
  int iContadorMedia;
  bool fIniciaLeituraTemp;
} st_tempdata_t;

/* Struct Eventos (Contador e Buzzer)*/
typedef struct
{
  int iContador;
  bool fBuzzerLigado;
  bool fReiniciaContador;
} st_eventos_t;

/* Public objects ------------------------------------------------------------*/
const static int iBtnPins[] = {GPIO_BTN_A, GPIO_BTN_B, GPIO_BTN_C};
const static st_buttons_t gstButtons = {
    .piButtonsPins = (int *)iBtnPins,
    .iNumButtons = MAX_BUTTONS};

const static int iLedsGpio[] = {GPIO_LED_A, GPIO_LED_B, GPIO_LED_C, GPIO_LED_D};
const static st_leds_t gstLeds = {
    .piLedsPins = (int *)iLedsGpio,
    .iNumLeds = MAX_LEDS};

st_tempdata_t gstDadosTemperatura;
st_timers_t gstTimers;
st_eventos_t gstEventos;

/* Global constants -----------------------------------------------------------*/
const int gviTempoTimers[MAX_TIMERS] = {
    TEMPO_TIMER_DEBOUNCE_BTN_A,
    TEMPO_TIMER_DEBOUNCE_BTN_B,
    TEMPO_TIMER_DEBOUNCE_BTN_C,
    TEMPO_TIMER_1_SEG,
    TEMPO_TIMER_BLINK_LED,
    TEMPO_TIMER_BUZZER};

/* Global Variables -----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
void ConfigButtons(const st_buttons_t *pstButtons);
void SetBtnPressedCallback(st_buttons_t *pstButtons, e_buttons_t eButton, pvButtonCallback pvCallback);
void CallbackButtonPressedA();
void CallbackButtonPressedB();
void CallbackButtonPressedC();
void CheckButtonsPressed(const st_buttons_t *pstButtons, st_timers_t *pstTimers);
void ConfigLeds(const st_leds_t *pstLeds);
void SetLedsOutput(const st_leds_t *pstLeds, int iValue);
void ConfigBuzzer(const int iPin);
void SetBuzzerOutput(const int iPin, st_eventos_t *pstEventos);
void IniciaVariaveis();
void IniciaTimers(st_timers_t *pstTimers);
void AtualizaTimers(st_timers_t *pstTimers);
bool fEstouroTimer(st_timers_t *pstTimers, e_timeout_t eTimeout);
void ResetaTimers(st_timers_t *pstTimers, e_timeout_t eTimeout);
float fLeituraTemperatura(const int iPin);
void AtualizaTemperatura(st_tempdata_t *pstTempData, float fUltimaLeitura);
bool fTemperaturaLimite(st_tempdata_t *pstTempData);
void AtualizaContador(st_eventos_t *pstEventos);
void ReiniciaContador(st_eventos_t *pstEventos);

/* Setup  --------------------------------------------------------------------*/
void setup()
{
  /* Habilita Serial */
  Serial.begin(BAUD_RATE);

  /*Configura Botões como ENTRADA PULL_UP */
  ConfigButtons(&gstButtons);

  /*Configura LEDs como Saídas e inicia com todos desligados */
  ConfigLeds(&gstLeds);

  /*Configura Buzzer como Saída e mantém desligado */
  ConfigBuzzer(GPIO_BUZZER);

  /* Inicializa Variáveis e Estruturas Globais */
  IniciaVariaveis();

  /*Configura e Inializa Timers do Projeto */
  IniciaTimers(&gstTimers);

  /*Msg informando a temperatura máx setada*/
  Serial.println("Bem-Vindo!");
  Serial.println("Temperatura Max: " + String(MAX_TEMPERATURA) + "C");

  /*Msg informando que SETUP foi finalizado*/
  Serial.println(F("Setup finalizado!"));
}

/* Loop ----------------------------------------------------------------------*/
void loop()
{
  /*Variáveis do Escopo Loop */
  float fTemperaturaAtual;

  /*Atualiza Timers */
  AtualizaTimers(&gstTimers);

  /*Verifica Botão Pressionado - Trata Debounce e Executa Callback*/
  CheckButtonsPressed(&gstButtons, &gstTimers);

  /*Verifica se Contador atingiu limite*/
  if (gstEventos.fReiniciaContador)
  {
    /*Reinicia Contador*/
    ReiniciaContador(&gstEventos);
  }

  /*Atualiza Timer de 1s */
  if (fEstouroTimer(&gstTimers, TIMER_1_SEG))
  {
    /* Atualiza Temperatura atual */
    fTemperaturaAtual = fLeituraTemperatura(SENSOR_TEMP);
    /* Atualiza estrutura temperatura */
    AtualizaTemperatura(&gstDadosTemperatura, fTemperaturaAtual);

    /*Verifica se temperatura atingiu limite*/
    fTemperaturaLimite(&gstDadosTemperatura) ? (gstEventos.fBuzzerLigado = true) : (gstEventos.fBuzzerLigado = false);

    /*Reinicia Timer */
    ResetaTimers(&gstTimers, TIMER_1_SEG);
  }
  /*Buzzer Evento Temperatura Maxima*/
  SetBuzzerOutput(GPIO_BUZZER, &gstEventos);
}

/* Body of private functions -------------------------------------------------*/

/**
 * @Func       : ConfigButtons
 * @brief      : Configura botões como entrada PULL_UP
 * @param      : pstButtons -> Estrutura contendo BUTTON (GPIO) e NUM_MAX
 * @retval     : N.A
 */
void ConfigButtons(const st_buttons_t *pstButtons)
{
  int i = 0;
  for (i = 0; i < pstButtons->iNumButtons; i++)
  {
    pinMode(pstButtons->piButtonsPins[i], INPUT_PULLUP);
  }
  /*Configura CallBacks*/
  SetBtnPressedCallback((st_buttons_t *)pstButtons, BUTTON_A, CallbackButtonPressedA);
  SetBtnPressedCallback((st_buttons_t *)pstButtons, BUTTON_B, CallbackButtonPressedB);
  SetBtnPressedCallback((st_buttons_t *)pstButtons, BUTTON_C, CallbackButtonPressedC);
}

/**
 * @Func       : SetBtnPressedCallback
 * @brief      : Configura função de callback para botão pressionado
 * @param      : pstButtons -> Estrutura contendo BUTTON (GPIO) e NUM_MAX
 *             : eButton -> Botão que terá função de callback configurada
 *             : pvCallback -> Função de callback
 * @retval     : N.A
 */
void SetBtnPressedCallback(st_buttons_t *pstButtons, e_buttons_t eButton, pvButtonCallback pvCallback)
{
  pstButtons->pvCallbackBtnPressed[eButton] = pvCallback;
}

/**
 * @Func       : CallbackButtonPressedA
 * @brief      : Função de callback para botão A pressionado
 * @param      : N.A
 * @retval     : N.A
 */
void CallbackButtonPressedA()
{
  /*Variáveis do Escopo */
  char szBuffer[100 + 1];
  char szTempAux[8 + 1];
  /*Limpa Buffers */
  memset(szBuffer, 0, sizeof(szBuffer));
  memset(szTempAux, 0, sizeof(szTempAux));
  /*Converte e printa informação temperatura*/
  dtostrf(gstDadosTemperatura.fTempMin, 6, 2, szTempAux);
  snprintf(szBuffer, sizeof(szBuffer), "Btn A Pressed. Menor Temperatura: %s", szTempAux);
  Serial.println(szBuffer);
  /*Atualiza Contador*/
  AtualizaContador(&gstEventos);
}

/**
 * @Func       : CallbackButtonPressedB
 * @brief      : Função de callback para botão B pressionado
 * @param      : N.A
 * @retval     : N.A
 */
void CallbackButtonPressedB()
{
  /*Variáveis do Escopo */
  char szBuffer[100 + 1];
  char szTempAux[8 + 1];
  /*Limpa Buffers */
  memset(szBuffer, 0, sizeof(szBuffer));
  memset(szTempAux, 0, sizeof(szTempAux));
  /*Converte e printa informação temperatura*/
  dtostrf(gstDadosTemperatura.fTempMedia, 6, 2, szTempAux);
  snprintf(szBuffer, sizeof(szBuffer), "Btn B Pressed. Media Temperatura: %s", szTempAux);
  Serial.println(szBuffer);
  /*Atualiza Contador*/
  AtualizaContador(&gstEventos);
}

/**
 * @Func       : CallbackButtonPressedC
 * @brief      : Função de callback para botão C pressionado
 * @param      : N.A
 * @retval     : N.A
 */
void CallbackButtonPressedC()
{
  /*Variáveis do Escopo */
  char szBuffer[100 + 1];
  char szTempAux[8 + 1];
  /*Limpa Buffers */
  memset(szBuffer, 0, sizeof(szBuffer));
  memset(szTempAux, 0, sizeof(szTempAux));
  /*Converte e printa informação temperatura*/
  dtostrf(gstDadosTemperatura.fTempMax, 6, 2, szTempAux);
  snprintf(szBuffer, sizeof(szBuffer), "Btn C Pressed. Maior Temperatura: %s", szTempAux);
  Serial.println(szBuffer);
  /*Atualiza Contador*/
  AtualizaContador(&gstEventos);
}

/**
 * @Func       : CheckButtonsPressed
 * @brief      : Verifica se botão foi pressionado e executa função de callback
 * @param      : pstButtons -> Estrutura contendo BUTTON (GPIO) e NUM_MAX
 * @retval     : N.A
 */
void CheckButtonsPressed(const st_buttons_t *pstButtons, st_timers_t *pstTimers)
{
  int i = 0;
  for (i = 0; i < pstButtons->iNumButtons; i++)
  {
    if (digitalRead(pstButtons->piButtonsPins[i]) == LOW)
    {
      if (fEstouroTimer(pstTimers, (e_timeout_t)i))
      {
        pstButtons->pvCallbackBtnPressed[i]();
        ResetaTimers(pstTimers, (e_timeout_t)i);
      }
    }
  }
}

/**
 * @Func       : ConfigLeds
 * @brief      : Configura LEDS como saída
 * @param      : pstLeds -> Estrutura contendo LEDS (GPIO) e NUM_MAX
 * @retval     : N.A
 */
void ConfigLeds(const st_leds_t *pstLeds)
{
  int i = 0;
  for (i = 0; i < pstLeds->iNumLeds; i++)
  {
    pinMode(pstLeds->piLedsPins[i], OUTPUT);
    digitalWrite(pstLeds->piLedsPins[i], LOW);
  }
}

/**
 * @Func       : SetLedsOutput
 * @brief      : Configura saída dos LEDS
 * @param      : pstLeds -> Estrutura contendo LEDS (GPIO) e NUM_MAX
 *             : iValue -> Valor a ser escrito nos LEDs
 * @retval     : N.A
 */
void SetLedsOutput(const st_leds_t *pstLeds, int iValue)
{
  int i = 0;
  for (i = 0; i < pstLeds->iNumLeds; i++)
  {
    /*Define o estado (ligado ou desligado) de um LED com base no valor do bit 'i' em 'iValue'.
    Isso envolve o deslocamento do bit desejado para a posição mais à direita e a extração do seu valor
    (1 para ligado, 0 para desligado)
    */
    digitalWrite(pstLeds->piLedsPins[i], (iValue >> i) & 0x01);
  }
}

/**
 * @Func       : ConfigBuzzer
 * @brief      : Configura Buzzer como saída
 * @param      : iPin - Pino vinculado ao buzzer
 * @retval     : N.A
 */
void ConfigBuzzer(const int iPin)
{
  pinMode(iPin, OUTPUT);
  digitalWrite(iPin, LOW);
}

/**
 * @Func       : SetBuzzerOutput
 * @brief      : Configura saída do Buzzer
 * @param      : iPin - Pino vinculado ao buzzer
 *             : pstEventos - Estrutura Dados Eventos
 * @retval     : N.A
 */
void SetBuzzerOutput(const int iPin, st_eventos_t *pstEventos)
{
  static bool fLigaBuzzer = false;
  static bool fIntervaloBuzzer = false;
  static int iContadorBuzzer = 0;

  /*Verifica Timer Buzzer*/
  if (pstEventos->fBuzzerLigado)
  {
    if (fEstouroTimer(&gstTimers, TIMER_BUZZER))
    {
      if (!fIntervaloBuzzer)
      {
        /*Inverte estado Buzzer*/
        fLigaBuzzer = !fLigaBuzzer;
        /*Atualiza saída Buzzer*/
        digitalWrite(iPin, fLigaBuzzer);
      }
      else
      {
        /*Mantém Buzzer Desligado*/
        digitalWrite(iPin, LOW);
      }
      /*Incrementa Contador*/
      iContadorBuzzer++;

      /*Verifica Valor Contador*/
      if (iContadorBuzzer == 6)
      {
        /*Reinicia Contador*/
        iContadorBuzzer = 0;
        /*Reinicia Flag Intervalo*/
        fIntervaloBuzzer = !fIntervaloBuzzer;
      }
      /*Reinicia Timer*/
      ResetaTimers(&gstTimers, TIMER_BUZZER);
    }
  }
  else
  {
    /*Mantém Buzzer Desligado*/
    digitalWrite(iPin, LOW);
    /*Reinicia Flag Intervalo*/
    fIntervaloBuzzer = false;
    /*Reinicia Contador*/
    iContadorBuzzer = 0;
    /*Reinicia Flag Liga Buzzer*/
    fLigaBuzzer = false;
  }
}

/**
 * @Func       : Inicia Estrutura de Timers
 * @brief      : Inicia Estrutura de Timers do Projeto
 * @param      : pstTimers -> Ponteiro para estrutura de Timers
 * @retval     : N.A
 */
void IniciaTimers(st_timers_t *pstTimers)
{
  /* Variáveis empregadas ao escopo */
  int i;

  /* Limpa Estrutura */
  memset(pstTimers, 0, sizeof(*pstTimers));
  /* Define o Tempo dos Timers e Inicia os mesmos */
  for (i = 0; i < MAX_TIMERS; i++)
  {
    pstTimers->ulTimers[i] = gviTempoTimers[i];
    pstTimers->fTimerExpirado[i] = false;
  }
}

/**
 * @Func       : IniciaVariaveis
 * @brief      : Inicia Variáveis (Globais) e Estruturas Projeto
 * @param      : N.A
 * @retval     : N.A
 */
void IniciaVariaveis()
{
  memset(&gstDadosTemperatura, 0, sizeof(gstDadosTemperatura));
  memset(&gstEventos, 0, sizeof(gstEventos));
}

/**
 * @Func       : AtualizaTimers
 * @brief      : Atualiza o estado dos timers com base no tempo atual
 * @param      : pstTimers - Ponteiro para a estrutura de dados dos timers
 * @retval     :  N.A
 */
void AtualizaTimers(st_timers_t *pstTimers)
{
  int i;
  unsigned long ulCurrentTime = millis();

  for (i = 0; i < MAX_TIMERS; i++)
  {
    if (!pstTimers->fTimerExpirado[i] && (ulCurrentTime >= pstTimers->ulTimers[i]))
    {
      pstTimers->fTimerExpirado[i] = true;
    }
  }
}

/**
 * @Func       : ResetaTimers
 * @brief      : Reinicia um timer específico para o seu valor inicial
 * @param      : pstTimers - Ponteiro para a estrutura de dados dos timers
 *             : eTimeout - O tipo de timer a ser reiniciado
 * @retval     :  N.A
 */
void ResetaTimers(st_timers_t *pstTimers, e_timeout_t eTimeout)
{
  pstTimers->ulTimers[eTimeout] = millis() + gviTempoTimers[eTimeout];
  pstTimers->fTimerExpirado[eTimeout] = false;
}

/**
 * @Func       : fEstouroTimer
 * @brief      : Verifica se um timer específico expirou
 * @param      : pstTimers - Ponteiro para a estrutura de dados dos timers
 *             : eTimeout - O tipo de timer a ser verificado
 * @retval     : true se o timer expirou, false caso contrário
 */
bool fEstouroTimer(st_timers_t *pstTimers, e_timeout_t eTimeout)
{
  return pstTimers->fTimerExpirado[eTimeout];
}

/**
 * @Func       : fLeituraTemperatura
 * @brief      :  Obtêm leitura de tempereratura do sensor TMP36
 * @param      : iPin - Pino vinculado ao sensor
 * @retval     : fTemperaturaAtual - Tempetura lida
 */
float fLeituraTemperatura(const int iPin)
{
  return ((analogRead(iPin) * (5 / 1024.0)) - 0.5) * 100;
}

/**
 * @Func       : AtualizaTemperatura
 * @brief      : Atualiza temperatura média, maior, menor obtidos pelo sensor TMP36
 * @param      : pstTempData - Estrutura Dados Temperatura
 *             : fUltimaLeitura - Ultimo valor lido pelo sensor TMP36
 * @retval     : N.A
 */
void AtualizaTemperatura(st_tempdata_t *pstTempData, float fUltimaLeitura)
{
  float fSomaLeituras = 0.0;
  static bool fCicloCompleto = false;
  static int iIndex = 0;
  int i;

  /* Verifica se leitura já foi iniciada */
  if (!pstTempData->fIniciaLeituraTemp)
  {
    pstTempData->fTempMin = fUltimaLeitura;
    pstTempData->fTempMax = fUltimaLeitura;
    pstTempData->fUltimoValor = fUltimaLeitura;
    pstTempData->iContadorMedia = 0;
    pstTempData->fLeiturasTemp[0] = fUltimaLeitura;
    pstTempData->fTempMedia = fUltimaLeitura;
    pstTempData->fIniciaLeituraTemp = true;
  }
  else
  {
    /* Atualiza Ultima Leitura */
    pstTempData->fUltimoValor = fUltimaLeitura;

    /* Adiciona a última temperatura lida ao vetor de Leituras */
    pstTempData->fLeiturasTemp[pstTempData->iContadorMedia] = fUltimaLeitura;

    /* Incrementa o contador de Leituras */
    pstTempData->iContadorMedia++;

    /* Verifica se contador alcançou o limite */
    (fCicloCompleto) ? (iIndex = MAX_LEITURAS) : (iIndex = pstTempData->iContadorMedia);

    /* Verifica se contador alcançou o limite e reinicia se necessário */
    if (pstTempData->iContadorMedia >= MAX_LEITURAS)
    {
      fCicloCompleto = true;
      pstTempData->iContadorMedia = 0;
    }

    /* Recalcula Média */
    for (i = 0; i < iIndex; i++)
    {
      /* Calcula Média somente de Valores lidos */
      fSomaLeituras += pstTempData->fLeiturasTemp[i];
    }

    /* Atualiza Media */
    pstTempData->fTempMedia = fSomaLeituras / iIndex;

    /* Atualiza a menor temperatura obtida nas últimas leituras */
    if (fUltimaLeitura < pstTempData->fTempMin)
    {
      pstTempData->fTempMin = fUltimaLeitura;
    }

    /* Atualiza a maior temperatura obtida nas últimas leituras */
    if (fUltimaLeitura > pstTempData->fTempMax)
    {
      pstTempData->fTempMax = fUltimaLeitura;
    }

    /*Imprime informações*/
   /* 
    Serial.println("Temperatura Atual: " + String(fUltimaLeitura) + "C");
    Serial.println("Temperatura Media: " + String(pstTempData->fTempMedia) + "C");
    Serial.println("Temperatura Min: " + String(pstTempData->fTempMin) + "C");
    Serial.println("Temperatura Max: " + String(pstTempData->fTempMax) + "C");
   */ 
  }
}

/**
 * @Func       : fTemperaturaLimite
 * @brief      : Verifica se temperatura lida está acima do limite
 * @param      : pstTempData - Estrutura Dados Temperatura
 * @retval     : true - Temperatura acima do limite
 *             : false - Temperatura abaixo do limite
 */
bool fTemperaturaLimite(st_tempdata_t *pstTempData)
{
  return (pstTempData->fUltimoValor > MAX_TEMPERATURA) ? true : false;
}

/**
 * @Func       : AtualizaContador
 * @brief      : Atualiza contador e flag para reiniciar contador
 * @param      : pstEventos - Estrutura Dados Eventos
 * @retval     : N.A
 */
void AtualizaContador(st_eventos_t *pstEventos)
{

  /*Verifica fReiniciaContador*/
  if (pstEventos->fReiniciaContador)
  {
    return;
  }

  /* Incrementa contador */
  pstEventos->iContador++;

  /* Verifica se contador alcançou o limite */
  if (pstEventos->iContador >= MAX_CONTADOR)
  {
    /* Atualiza flag para reiniciar contador */
    pstEventos->fReiniciaContador = true;
    return;
  }

  /*Atualiza saídas LEDs com valor contador*/
  if (!pstEventos->fReiniciaContador)
  {
    SetLedsOutput(&gstLeds, pstEventos->iContador);
  }
}

/**
 * @Func       : ReiniciaContador
 * @brief      : Reinicia contador e LEDs
 * @param      : pstEventos - Estrutura Dados Eventos
 * @retval     : N.A
 */
void ReiniciaContador(st_eventos_t *pstEventos)
{
  static int iContadorReinicia = 0;
  static int iBlink = MAX_CONTADOR;

  /*Verifica Timer Blink LED*/
  if (fEstouroTimer(&gstTimers, TIMER_BLINK_LED))
  {
    /*Inverte estado LEDS*/
    SetLedsOutput(&gstLeds, iBlink);
    /*Reinicia Timer*/
    ResetaTimers(&gstTimers, TIMER_BLINK_LED);
    /*Incrementa contador*/
    iContadorReinicia++;

    /*Altera Valor de iBLink*/
    (iContadorReinicia % 2 == 0) ? (iBlink = MAX_CONTADOR) : (iBlink = 0);

    /*Verifica se contador alcançou o limite*/
    if (iContadorReinicia == 6)
    {
      /*Desliga todos os LEDs*/
      SetLedsOutput(&gstLeds, 0);
      /*Reinicia contador*/
      iContadorReinicia = 0;
      /*Reinicia iBlink*/
      iBlink = MAX_CONTADOR;
      /* Reinicia contador */
      pstEventos->iContador = 0;
      /* Atualiza flag para reiniciar contador */
      pstEventos->fReiniciaContador = false;
    }
  }
}