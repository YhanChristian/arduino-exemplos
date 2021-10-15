/*
  WebsServer com nodemcu
  Utilizado: Nodemcu, Led
  Autor: Yhan Christian Souza Silva - Data: 03/08/2017
*/

// --- Bibliotecas Auxiliares ---

#include <ESP8266WiFi.h>

// --- Definições de rede ---

const char* ssid = "xxx";
const char* password = "xxx";

// --- Variável pin led 13 = pino D7 Nodemcu ---
// --- Escutar porta 80 ---

int ledPin = 13;
WiFiServer server(80);

// --- Setup ---
void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println();
  Serial.println();
  Serial.print("Conectando a rede: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

// --- Verifica e aguarda conexão ---
// --- Printa ip atribuido ao Nodemcu ---   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  server.begin();
  Serial.println("Web server started");
  Serial.print("Utilize a seguinte url ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

// --- Loop --- 

void loop() {

// --- Verifica conexão com servidor ---
  WiFiClient client = server.available();
  if (!client) return;

// --- Aguardar envio de dados --- 
  Serial.println("Novo cliente...");
  while (!client.available()) delay(1);
  
// -- Realiza leitura da primeira linha da request ---
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  int value = LOW;

// --- Verifica request e atualiza valor led ---
  if (request.indexOf("/LED=LIGADO") != -1)  {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=DESLIGADO") != -1)  {
    digitalWrite(ledPin, LOW);
    value = LOW;
  }

 // -- Retorna  resposta ---
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1>Primeiro Webserver c/ Nodemcu</h1>");
  client.print("O led esta: ");
  if (value == HIGH) client.print("LIGADO");
  else client.print("DESLIGADO");
  client.println("<br><br>");
  client.println("<a href=\"/LED=LIGADO\"\"><button> LIGAR </button></a>");
  client.println("<a href=\"/LED=DESLIGADO\"\"><button> DESLIGAR </button></a><br />");
  client.println("</html>");
  delay(1);
  Serial.println("Cliente desconectado");
  Serial.println("");
}
