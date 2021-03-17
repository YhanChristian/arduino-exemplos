/** =============================================================================================================
  Projeto: Controlador PI com Arduino - Atividade da disciplina Sistemas de Contorle
  Utilizado: Arduino UNO, circuito RCRC
  @file trabalho_sistemas_controle.ino
  @author(s) Yhan Christian Souza Silva | Simone Leite da Silva Gimenes
  @date  12/03/2021
  @version V.1.0.0

          **DESCRIÇÃO DE FUNCIONAMENTO DO FIRMWARE**

  -firmware que ajusta tensão de entrada sistem VIN_SISTEMA (saída PWM 3) arduino de acordo com a leitura
  - do VOUT_SISTEMA (entrada analógica A0) que faz a leitura do valor analógico atual, compara com o valor
  - setpoint e executa algoritmo PID para manter valor de acordo com o setpoint

          **TABELA SETPOINT VALORES DE SETPOINT CONVERTIDOS EM TENSAO**

                                Valor (ADC)  Tensão
                                      0         0
                                     102       0.5
                                     205        1
                                     307       1.5
                                     409        2
                                     512       2.5

  - O circuito trata-se de um RCRC com divisor resistivo, mesmo que a saída PWM seja 255 (5V) na entrada do
  circuito. A saída ficará em aproximadamente 2.5V.

  ================================================================================================================ */

// ========================================= Defines e Hardware =================================================

#define VIN_SISTEMA 3
#define VOUT_SISTEMA A0

#define PWM_MINIMO 0
#define PWM_MAXIMO 255

#define ADC_MINIMO 0
#define ADC_MAXIMO 1023

#define SETPOINT 409
#define TEMPO_AMOSTRAGEM 250E-3
#define TEMPO_ATUALIZACAO (TEMPO_AMOSTRAGEM * 1000)

// ================================== Variáveis e Constantes (Globais) ============================================

uint16_t pwm = PWM_MINIMO;
double kp = 0.5, ki = 1.0, kd = 0.0, erroMedido, proporcional, integral, derivativo,
       pid, medida, ultimaMedida, leituraSaidaSistema, setpoint = SETPOINT;
unsigned long ultimaAtualizacao;

// =================================== Protótipo Funções Auxiliares  ===============================================

void controlePID();
void exibeDadosSerial();
// ============================================== Setup ==========================================================

void setup()
{
  pinMode(VIN_SISTEMA, OUTPUT);
  analogWrite(VIN_SISTEMA, pwm);
  Serial.begin(9600);
}

// ============================================== Loop =============================================================

void loop()
{

  //Executa PID a cada estouro TIMER

  if ((long)millis() - ultimaAtualizacao >= TEMPO_ATUALIZACAO)
  {

    leituraSaidaSistema = analogRead(VOUT_SISTEMA);
    controlePID();
    analogWrite(VIN_SISTEMA, pwm);

    Serial.print(float(SETPOINT));
    Serial.print(",");
    Serial.println(leituraSaidaSistema);
    ultimaAtualizacao = millis();
  }
}


// ========================================= Funções Auxiliares=====================================================

//Realiza algoritmo PID

void controlePID()
{
  medida = leituraSaidaSistema;

  erroMedido = SETPOINT - medida;
  
  proporcional = erroMedido * kp;

  integral += (erroMedido * ki) * TEMPO_AMOSTRAGEM;

  //Filtro anti-windup

  if(integral > ADC_MAXIMO)
  {
    integral = ADC_MAXIMO;
  }
  
  else if (integral < ADC_MINIMO)
  {
    integral = ADC_MINIMO; 
  }

  derivativo = ((ultimaMedida - medida) * kd) / TEMPO_AMOSTRAGEM;

  pid = proporcional + integral + derivativo;

  pwm = pid / 4;

//Limita entre valores máximo e minimo
  if (pwm > PWM_MAXIMO)
  {
    pwm = PWM_MAXIMO;
  }
  ultimaMedida = medida;
}
