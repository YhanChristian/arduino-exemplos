/* Projeto Automação residencial com Cayenne
   Utilizado: NodeMCU, módulo relé 2 canais e lâmpadas de LED AC
   Autor: Yhan Christian Souza Silva - Data: 04/08/2018
*/

// -- Bibliotecas Auxiliares  --

#include <ESP8266WiFi.h>
#include <CayenneMQTTESP8266.h>

// -- Definições de Hardware / Debug --

#define lamp01 D4
#define lamp02 D5
#define CAYENNE_PRINT Serial


// -- Credenciais de rede e de acesso ao Cayenne

char ssid[] = "Seu SSID";
char password[] = "Sua Senha";
char cayenneUsername[] = "seu username Cayenne";
char cayennePassword[] = "seu password Cayenne";
char cayenneClientID[] = "seu client ID Cayenne";

// -- Setup --

/* O módulo relé trabalha acionando a carga com nível baixo
   Para garantir que a carga iniciará desligada, no setup deixamos em nível HIGH.
*/

void setup() {
  Serial.begin(115200);
  Cayenne.begin(cayenneUsername, cayennePassword, cayenneClientID, ssid, password);
  pinMode(lamp01, OUTPUT);
  pinMode(lamp02, OUTPUT);
  digitalWrite(lamp01, HIGH);
  digitalWrite(lamp02, HIGH);
  delay(500);
}

// -- Chama função Cayenne loop --

void loop() {
  Cayenne.loop();
}

// -- Pino virtual 4 Cayenne lampada 01, ao "clicar em ligar" joga LOW para o módulo relé acionando a carga --

CAYENNE_IN(4) {
  int readLamp01 = getValue.asInt();
  if (readLamp01) digitalWrite(lamp01, LOW);
  else digitalWrite(lamp01, HIGH);
}

// -- Pino virtual 5 Cayenne lampada 02, ao "clicar em ligar" joga LOW para o módulo relé acionando a carga --

CAYENNE_IN(5) {
  int readLamp02 = getValue.asInt();
  if (readLamp02) digitalWrite(lamp02, LOW);
  else digitalWrite(lamp02, HIGH);
}

// -- Log de erros -- 

CAYENNE_IN_DEFAULT() {
  CAYENNE_LOG("Canal %u, valor %s", request.channel, getValue.asString());
}

