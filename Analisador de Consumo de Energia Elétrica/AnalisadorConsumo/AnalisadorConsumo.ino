/* Projeto ACE² - Analisador de Consumo de Energia Elétrica
   Utilizado: NodeMCU, sensor de corrente, display nokia 5110
   Autor: Yhan Christian Souza Silva - Data: 29/05/2018 - versão 1.3
*/

// -- Bibliotecas Auxiliares --

#include <ESP8266WiFi.h>
#include <CayenneMQTTESP8266.h>
#include <EEPROM.h>
#include "EmonLib.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>



// -- Instância objetos --

EnergyMonitor currentSensor;
Adafruit_PCD8544 myDisplay = Adafruit_PCD8544(D8, D7, D6, D5, D2);


// -- Definições p/ uso geral-

#define CAYENNE_PRINT Serial


// -- Variáveis e constantes --
// Valor verificado no site da ANEEL com ranking de custo médio kWh = http://www.aneel.gov.br/ranking-das-tarifas


char ssid[] = "Seu SSID";
char password[] = "Sua senha";
const int sensorPin = A0;
const int E2PromSize = 512;
const float calibrationValue = 9.09;
const long interval01 = 5000;
int addresskWh = 0, readVoltage = 110;
float kWh = 0, activePower = 0, cost = 0, costkWh = 0.47;
long lastMillis01 = 0;

// -- Autenticação Cayenne --

char cayenneUsername[] = "seu username Cayenne";
char cayennePassword[] = "seu password Cayenne";
char cayenneClientID[] = "seu client ID Cayenne";



// -- Escopo de funções --

float readAverageCurrent();
int setVoltage(int voltage);
double setAveragekWhCost(double averageCostkWh);
void energyData();
void writeE2PromData(float x, int p);
float readE2PromData();
CAYENNE_IN_DEFAULT();
void clearE2PromData();


void setup() {
  Serial.begin(9600);

  /*Calculo baseado no seguinte link: http://tyler.anairo.com/projects/open-energy-monitor-calculator
     Delimitada corrente máxima a ser medida em 10A resistor de carga 220R, sensor tem capacidade de 100A
     Para ler até este valor usar resistor de carga de 22R.
     Calibration value = 2000 / 220 = 9.09
  */
  currentSensor.current(sensorPin, calibrationValue);
  EEPROM.begin(E2PromSize);
  kWh = readE2PromData();

  Cayenne.begin(cayenneUsername, cayennePassword, cayenneClientID, ssid, password);

  myDisplay.begin();
  myDisplay.setContrast(50);
  myDisplay.clearDisplay();
  myDisplay.setTextSize(1);
  myDisplay.setTextColor(BLACK, WHITE);
  myDisplay.setCursor(0, 0);
  myDisplay.println("Aguarde inicio do sistema");
  Serial.println("Aguarde inicio do sistema");
  Serial.print("Conectado a rede: ");
  Serial.println(ssid);
  myDisplay.display();
  delay(1000);
}

void loop() {
  Cayenne.loop();
  if (millis() - lastMillis01 >= interval01) {
    energyData();
    lastMillis01 = millis();
  }
}


// -- Funções Auxiliares --

float readAverageCurrent() {
  float currentValue = currentSensor.calcIrms(1480);
  float averageCurrent;
  int averageValue = 1000;
  for (int i = 0; i < averageValue; i++) averageCurrent += currentValue;
  averageCurrent /= averageValue;
  return averageCurrent;
}

int setVoltage(int voltage) {
  return voltage;
}

CAYENNE_IN(1) {
  int voltage = getValue.asInt();
  if (voltage) readVoltage = setVoltage(220);
  else  readVoltage = setVoltage(110);
}

double setAveragekWhCost(double averageCostkWh) {
  return averageCostkWh;
}

CAYENNE_IN(6) {
  double setCostkWh = getValue.asDouble();
  double readverageCostkWh = setAveragekWhCost(costkWh);
  if (setCostkWh > 0.40)  costkWh = setAveragekWhCost(setCostkWh);
  else {
    setCostkWh = 0.47;
    costkWh = setAveragekWhCost(setCostkWh);
  }
}


void energyData() {
  float readCurrent = readAverageCurrent();
  float auxkWh;
  if (readCurrent < 0.1) readCurrent = 0;
  activePower = readVoltage * readCurrent;
  kWh += (activePower / 3600000);
  auxkWh = readE2PromData();
  if (kWh != auxkWh) writeE2PromData(addresskWh, kWh);
  cost = setAveragekWhCost(costkWh) * kWh;

  Cayenne.virtualWrite(0, readCurrent);
  Cayenne.virtualWrite(2, kWh);
  Cayenne.virtualWrite(3, activePower);
  Cayenne.virtualWrite(4, cost);


  myDisplay.clearDisplay();
  myDisplay.setTextSize(1);
  myDisplay.setTextColor(WHITE, BLACK);
  myDisplay.setCursor(0, 0);
  myDisplay.println("Consumo atual");
  myDisplay.setTextColor(BLACK, WHITE);
  myDisplay.setCursor(0, 10);
  myDisplay.print("(A): ");
  myDisplay.println(readCurrent);
  myDisplay.setCursor(0, 20);
  myDisplay.print("(W): ");
  myDisplay.println(activePower);
  myDisplay.setCursor(0, 30);
  myDisplay.print("(KWh): ");
  myDisplay.println(kWh, 5);
  myDisplay.setCursor(0, 40);
  myDisplay.print("(R$): ");
  myDisplay.println(cost, 5);

  // -- Serial monitor para acompanhar informações --

  Serial.print("Corrente (A): ");
  Serial.print(readCurrent);
  Serial.print(" | ");
  Serial.print("Tensão (V): ");
  Serial.print(readVoltage);
  Serial.print(" | ");
  Serial.print("Potência (W): ");
  Serial.print(activePower);
  Serial.print(" | ");
  Serial.print("Valor (KWh): ");
  Serial.print(kWh, 5);
  Serial.print(" | ");
  Serial.print("Valor (R$ x kWh): ");
  Serial.print(setAveragekWhCost(costkWh));
  Serial.print(" | ");
  Serial.print("Custo (R$): ");
  Serial.println(cost, 5);
  myDisplay.display();
}

void writeE2PromData(int p, float x) {
  EEPROM.write(p, int(x));
  EEPROM.write(p + 1, int((x - int(x)) * 100));
  EEPROM.write(p + 2, int((x - int(x)) * 1000));
  EEPROM.write(p + 3, int((x - int(x)) * 10000));
  EEPROM.commit();
}

float readE2PromData() {
  int p;
  return float(EEPROM.read(p) + float(EEPROM.read(p + 1)) / 100 + float(EEPROM.read(p + 2)) / 1000 + float(EEPROM.read(p + 3)) / 10000);
}

// -- Log para verificação de erros --

CAYENNE_IN_DEFAULT() {
  CAYENNE_LOG("Canal %u, valor %s", request.channel, getValue.asString());
}

void clearE2PromData() {
  for (int i = 0; i < E2PromSize; i++) {
    if (EEPROM.read(i) != 0) {
      float x = 0;
      EEPROM.write(i, 0);
      EEPROM.write(i + 1, int((x - int(x)) * 100));
      EEPROM.write(i + 2, int((x - int(x)) * 1000));
      EEPROM.write(i + 3, int((x - int(x)) * 10000));
      EEPROM.commit();
    }
  }
  Serial.print("EEPROM Limpa - iniciando novo ciclo");
  addresskWh = 0;
  ESP.restart();
  delay(500);
}

CAYENNE_IN(5) {
  float clearE2Prom = getValue.asInt();
  if (clearE2Prom) clearE2PromData();
}



