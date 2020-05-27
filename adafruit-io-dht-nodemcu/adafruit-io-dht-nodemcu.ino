/* Leitura de temperatura e umidade e AdafruitIO
   Hardware: NodeMCU v.1.0, DHT22
   Autor: Yhan Christian Souza Silva - Data: 27/05/2020
*/

//============================ Bibliotecas Auxiliares ===============================

#include "AdafruitIO_WiFi.h"
#include <DHT.h>

//============================= Hardware e Defines ====================================

#define DHTPIN D2
#define DHTTYPE DHT22


#define WIFI_SSID       "***********"
#define WIFI_PASS       "***********"

#define IO_USERNAME    "***********"
#define IO_KEY         "***********"

#define BAUD_RATE         9600

//=============================== Instância Objetos =================================

DHT dht(DHTPIN, DHTTYPE);
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

//============================== Declaração de Feeds ================================

AdafruitIO_Feed *temperatura = io.feed("temperatura");
AdafruitIO_Feed *umidade = io.feed("umidade");

//============================ Variáveis e Constantes ===============================

const uint16_t intervaloLeitura = 10000;
long ultimoMillis = 0;

//========================= Protótipo Funções Auxiliares ============================

void conectaAdafruitIO();
void leituraSensor();

//================================== Setup ==========================================

void setup() {
  Serial.begin(BAUD_RATE);
  dht.begin();
  conectaAdafruitIO();
}

//================================== Loop ===========================================

void loop() {
  io.run();
  if (millis() - ultimoMillis > intervaloLeitura) {
    leituraSensor();
    ultimoMillis = millis();
  }
}

//============================ Funções Auxiliares ====================================


void conectaAdafruitIO() {
  Serial.print("Conectando ao Adafruit IO");
  io.connect();

  // Aguardando conexão

  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Conectado com sucesso

  Serial.println();
  Serial.println(io.statusText());

}

void leituraSensor() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("Umidade % = ");
  Serial.println(h);
  Serial.print("Temperatura ºC = ");
  Serial.println(t);
  umidade->save(h);
  temperatura->save(t);
}
