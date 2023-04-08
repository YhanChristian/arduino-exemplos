/**
  ******************************************************************************
  * @Company    : Yhan Christian Souza Silva
  * @file       : README.txt
  * @author     : Yhan Christian Souza Silva
  * @date       : 08/04/2023
  * @brief      : Exemplo simples de função para FadeIn / FadeOut LED (saída PWM)
  ******************************************************************************
*/

//Defines
#define LED_PWM 9

//Protótipo de Funções Auxiliares
void fadeLed(int pin, int fadeTime, bool state);

//Setup
void setup()
{
  pinMode(LED_PWM, OUTPUT);
  digitalWrite(LED_PWM, LOW);
  Serial.begin(9600);
}

// Loop
void loop()
{
  static bool firstLeitura = true;
  static bool leituraLed = false;

  if (firstLeitura) {
    leituraLed = digitalRead(LED_PWM);
    firstLeitura = false;
  }
  fadeLed(LED_PWM, 5000, leituraLed);
}

// Funções Auxilires
void fadeLed(int pin, int fadeTime, bool state) 
{
  static bool fadeIn = false;
  static bool fadeOut = false;
  static bool firstTime = true;
  static unsigned long lastUpdate = 0;
  const int timeInterval = fadeTime / 255;
  static int brightness = 0;

  if (firstTime) {
    if (state) {
      fadeIn = false;
      fadeOut = true;
      brightness = 255;
    }
    else {
      fadeIn = true;
      fadeOut = false;
      brightness = 0;
    }
    firstTime = false;
  }

  if ((unsigned long)millis() - lastUpdate >= timeInterval) {
    if (fadeIn && !fadeOut) {
      ++brightness;
      if (brightness >= 255) {
        brightness = 255;
        fadeIn = false;
        fadeOut = true;
      }
    } else if (!fadeIn && fadeOut) {
      --brightness;
      if (brightness <= 0) {
        brightness = 0;
        fadeIn = true;
        fadeOut = false;
      }
    }
    Serial.println("Brightness = " + String(brightness));
    analogWrite(pin, brightness);
    lastUpdate = millis();
  }
}
