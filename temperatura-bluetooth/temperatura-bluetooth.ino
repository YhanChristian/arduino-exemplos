/* 
  Leitura de Temperatura - Via Bluetooth
  Utilizado: Arduino Uno, DHT 11 e Módulo Bluetooth HC-05
  Autor: Yhan Christian Souza Silva - Data: 10/03/2016
 */

// --- Bibliotecas Auxiliares ---

#include <LiquidCrystal.h>              
#include <DHT.h>                
#include <SoftwareSerial.h>

// --- Hardware ---

SoftwareSerial bluetoothSerial(2, 3);
#define pinoSensor A3                  
#define sensorTemperatura DHT11

float temp = 0;
DHT dht(pinoSensor, sensorTemperatura);     

 // --- Configurações Iniciais ---
 
void setup()
{
  Serial.begin(9600);
   bluetoothSerial.begin(115200); 
}

// --- Loop Infinito ---

void loop()
{
  temp = dht.readTemperature();
  bluetoothSerial.println(temp);
  delay(1000);        
}
