/** =============================================================================================================
  Projeto: Controlador Fuzzy com Arduino - Atividade Disciplina: Inteligência Artificial IFSP
  Utilizado: Arduino UNO, circuito RCRC
  @file fuzzy.ino
  @author(s) Yhan Christian Souza Silva
  @date  26/06/2021
  @version V.1.0.0

          **DESCRIÇÃO DE FUNCIONAMENTO DO FIRMWARE**

  -firmware que ajusta tensão de entrada sistem VIN_SISTEMA (saída PWM 3) arduino de acordo com a leitura
  - do VOUT_SISTEMA (entrada analógica A0) que faz a leitura do valor analógico atual, compara com o valor
  - setpoint e executa algoritmo lógica Fuzzy para sistema atender o setpoint

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

// ====================================== Bibliotecas Auxiliares ==============================================

#include <Fuzzy.h>

// ========================================= Defines e Hardware =================================================

#define LEITURA_ANALOGICA A0
#define SAIDA_PWM 3

#define ADC_MINIMO 0
#define ADC_MAXIMO 1023

#define PWM_MINIMO 0
#define PWM_MAXIMO 255

#define SETPOINT 409

#define TEMPO_AMOSTRAGEM 100

// Caso n queira exibir calculo delta comentar esta linha
//#define EXIBE_DELTA

// ========================================== Instância Objeto =====================================================

Fuzzy *fuzzy = new Fuzzy();

// ================================== Variáveis e Constantes (Globais) ============================================

unsigned long ultima_atualizacao;
uint8_t pwm = PWM_MINIMO;

// ============================================== Setup ==========================================================

void setup() {
  //Inicia saída e escreve valor atual pwm = 0;
  pinMode(SAIDA_PWM, OUTPUT);
  analogWrite(SAIDA_PWM, pwm);
  Serial.begin(9600);

  //Seta controlador Fuzzy

  // Entrada Delta
  FuzzySet *delta_muito_pequeno = new FuzzySet(-512, -512, -512, -256);
  FuzzySet *delta_pequeno = new FuzzySet(-512, -256, -256, 0);
  FuzzySet *delta_setpoint = new FuzzySet(-256, 0, 0, 256);
  FuzzySet *delta_alto = new FuzzySet(0, 256, 256, 512);
  FuzzySet *delta_muito_alto = new FuzzySet(256, 512, 512, 512);

  //Saida Saida_PWM

  FuzzySet *pwm_nulo = new FuzzySet(0, 0, 0, 64);
  FuzzySet *pwm_pequeno = new FuzzySet(0, 64, 64, 128);
  FuzzySet *pwm_medio = new FuzzySet(64, 128, 128, 191);
  FuzzySet *pwm_alto = new FuzzySet(128, 191, 191, 255);
  FuzzySet *pwm_muito_alto = new FuzzySet(191, 255, 255, 255);

  //Variável de Entrada controlador Fuzzy

  FuzzyInput *delta = new FuzzyInput(1);
  delta->addFuzzySet(delta_muito_pequeno);
  delta->addFuzzySet(delta_pequeno);
  delta->addFuzzySet(delta_setpoint);
  delta->addFuzzySet(delta_alto);
  delta->addFuzzySet(delta_muito_alto);
  fuzzy->addFuzzyInput(delta);

  //Variável de Saída Controlador Fuzzy
  FuzzyOutput *saida_pwm = new FuzzyOutput(1);
  saida_pwm->addFuzzySet(pwm_nulo);
  saida_pwm->addFuzzySet(pwm_pequeno);
  saida_pwm->addFuzzySet(pwm_medio);
  saida_pwm->addFuzzySet(pwm_alto);
  saida_pwm->addFuzzySet(pwm_muito_alto);
  fuzzy->addFuzzyOutput(saida_pwm);


  //Regras Controlador Fuzzy
  /* 1. Se delta_muito_alto então pwm_nulo
     2. Se delta_alto então pwm_pequeno
     3. Se delta_setpoint então pwm_medio
     4. Se delta_pequeno então pwm_alto
     5. Se delta_muito_pequeno entao pwm_muito_alto
  */

  //Regra 01- Se delta_muito_alto então pwm_nulo
  FuzzyRuleAntecedent *se_delta_muito_alto = new FuzzyRuleAntecedent();
  se_delta_muito_alto->joinSingle(delta_muito_alto);
  FuzzyRuleConsequent *entao_pwm_nulo = new FuzzyRuleConsequent();
  entao_pwm_nulo->addOutput(pwm_nulo);
  FuzzyRule *regra_fuzzy_01 = new FuzzyRule(1, se_delta_muito_alto, entao_pwm_nulo);
  fuzzy->addFuzzyRule(regra_fuzzy_01);


  //Regra 02 - Se delta_alto então pwm_pequeno
  FuzzyRuleAntecedent *se_delta_alto = new FuzzyRuleAntecedent();
  se_delta_alto->joinSingle(delta_alto);
  FuzzyRuleConsequent *entao_pwm_pequeno = new FuzzyRuleConsequent();
  entao_pwm_pequeno->addOutput(pwm_pequeno);
  FuzzyRule *regra_fuzzy_02 = new FuzzyRule(2, se_delta_alto, entao_pwm_pequeno);
  fuzzy->addFuzzyRule(regra_fuzzy_02);

  //Regra 03 - Se delta_setpoint então pwm_medio
  FuzzyRuleAntecedent *se_delta_setpoint = new FuzzyRuleAntecedent();
  se_delta_setpoint->joinSingle(delta_setpoint);
  FuzzyRuleConsequent *entao_pwm_medio = new FuzzyRuleConsequent();
  entao_pwm_medio->addOutput(pwm_medio);
  FuzzyRule *regra_fuzzy_03 = new FuzzyRule(3, se_delta_setpoint, entao_pwm_medio);
  fuzzy->addFuzzyRule(regra_fuzzy_03);

  //Regra 04 - Se delta_pequeno então pwm_alto
  FuzzyRuleAntecedent *se_delta_pequeno = new FuzzyRuleAntecedent();
  se_delta_pequeno->joinSingle(delta_pequeno);
  FuzzyRuleConsequent *entao_pwm_alto = new FuzzyRuleConsequent();
  entao_pwm_alto->addOutput(pwm_alto);
  FuzzyRule *regra_fuzzy_04 = new FuzzyRule(4, se_delta_pequeno, entao_pwm_alto);
  fuzzy->addFuzzyRule(regra_fuzzy_04);

  //Regra 05 - Se delta_muito_pequeno entao pwm_muito_alto
  FuzzyRuleAntecedent *se_delta_muito_pequeno = new FuzzyRuleAntecedent();
  se_delta_muito_pequeno->joinSingle(delta_muito_pequeno);
  FuzzyRuleConsequent *entao_pwm_muito_alto = new FuzzyRuleConsequent();
  entao_pwm_muito_alto->addOutput(pwm_muito_alto);
  FuzzyRule *regra_fuzzy_05 = new FuzzyRule(5, se_delta_muito_pequeno, entao_pwm_muito_alto);
  fuzzy->addFuzzyRule(regra_fuzzy_05);
}

// ============================================== Loop =============================================================

void loop() {
  if ((long)millis() - ultima_atualizacao > TEMPO_AMOSTRAGEM)
  {
    //Calcula Delta e informa valor ao controlador fuzzy
    int leitura_analogica = analogRead(LEITURA_ANALOGICA);
    int calcula_delta = leitura_analogica - SETPOINT;

    //Nebulização

    fuzzy->setInput(1, calcula_delta);
    fuzzy->fuzzify();

    //Desnebulizaçao
    pwm = fuzzy->defuzzify(1);

    //Escreve na saida valor atualizado do pwm
    analogWrite(SAIDA_PWM, pwm);
    Serial.print("Setpoint: ");
    Serial.print(SETPOINT);
    Serial.print(",");
 #ifdef EXIBE_DELTA
    Serial.print(" Delta: ");
    Serial.print(calcula_delta);
    Serial.print(",");
    Serial.print(" PWM: ");
    Serial.print(pwm);
    Serial.print(",");
 #endif
    Serial.print(" ADC: ");
    Serial.println(leitura_analogica);



    ultima_atualizacao = millis();
  }
}
