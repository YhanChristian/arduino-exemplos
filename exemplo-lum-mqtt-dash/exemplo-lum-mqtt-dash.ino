
/*
  Integração MQTT Dash e NodeMCU
  Hardware Utilizado: Nodemcu v1.0, LDR
  Autor: Yhan Christian Souza Silva - data: 30/07/2018

*/

// -- Bibliotecas Auxiliares --

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// -- Hardware --

#define LDR A0

// -- Definições de Rede e conexão MQTT --

const char *ssid = "Seu SSID";
const char *password = "Sua Senha";

/* Dados do CloudMQTT e tópico do MQTT Dash
   Lembre-se que se um destes valores estiver incorreto
   O NodeMCU não enviará dados para MQTT Dash.
*/

const char* cloudMQTTServer = "Server da Instância CloudMQTT";
const char* cloudMQTTUser = "User da Instância CloudMQTT";
const char* cloudMQTTPassword = "Senha da Instância CloudMQTT";
const int cloudMQTTPort = 11933;
const char* MQTTDashTopic = "luminosidade"; // tópico a ser publicado, lembre-se da config. MQTT Dash

// -- Instância objetos --

WiFiClient myClient;
PubSubClient client(myClient);

// -- Variáveis e constantes --

long lastMillis;
const long interval = 30000;


// -- Escopo funções auxiliares --

void connectToWiFi();
void connectToBrokerMQTT();
void reconnectToBrokerMQTT();
void sendSensorData();


// -- Setup -- 

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  connectToBrokerMQTT();
}

// -- Loop para reconetar ao Broker e enviar dados do sensor a cada 30s --

void loop() {
  if (!client.connected()) reconnectToBrokerMQTT();
  if (millis() - lastMillis >= interval) {
    sendSensorData();
    lastMillis = millis();
  }
  client.loop();
}

// -- Funções auxiliares --


// -- Fornece as credenciais e conecta a rede -- 

void connectToWiFi() {
  Serial.println("Conectando a rede: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}


// -- Realiza a primeira conexão ao Broker -- 

void connectToBrokerMQTT() {
  client.setServer(cloudMQTTServer, cloudMQTTPort);
  while (!client.connected()) {
    Serial.println("Conectando ao broker MQTT");
    if (client.connect("NodeMCU Client", cloudMQTTUser, cloudMQTTPassword)) Serial.println("Conectado com sucesso!");
    else {
      Serial.print("Falha estado: ");
      Serial.println(client.state());
      delay(3000);
    }
  }
}


// -- Reconecta ao Broker e verifica mensagens de erro --

void reconnectToBrokerMQTT() {
  while (!client.connected()) {
    Serial.println("Tentando conexão ao broker MQTT");

    boolean connectedToBroker = strlen(cloudMQTTUser) > 0 ?
                                client.connect("NodeMCU Client", cloudMQTTUser, cloudMQTTPassword) :
                                client.connect("NodeMCU Client");

    if (connectedToBroker)  Serial.println("Conectado com sucesso!");
    else {
      Serial.println("Falha durante a conexão erro#code: ");
      Serial.println(String(client.state()).c_str());
      Serial.println("Tentando conexão novamente em 15s");
      delay(15000);
    }
  }
}

/* FAz leitura do sensor LDR converte o valor para caractere
 * publica o valor no tópico 
 */
void sendSensorData() {
  char msgLumMQTT[10];
  int readSensor = analogRead(LDR);
  Serial.print("Luminosidade: ");
  Serial.println(readSensor);
  dtostrf(readSensor, 10, 0, msgLumMQTT);
  client.publish(MQTTDashTopic, msgLumMQTT);
}

