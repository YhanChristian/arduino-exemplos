
/*
 Copyright (C) 2014 - A. Khorshidi

 This file is licensed under the Creative Commons
 Attribution-Share Alike 4.0 International license.
 
*/
#include <PID_v1.h>

#define SensorPin A0
#define ActuatorPin 3
double Setpoint, Input, Output;
float inByte;
double Kp = 1, Ki= 1 , Kd = 1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup(){
  
  pinMode(ActuatorPin, OUTPUT);
  Serial.begin(9600);
  myPID.SetMode(AUTOMATIC);
}

void loop(){
  Setpoint = 50;
  Input = (float(analogRead(SensorPin)) * 5 / (1023)) / 0.01;
  if(!Serial.available()){
  Serial.println(Input,1);
  myPID.Compute();
  delay(1000);
  }
  while (Serial.available() > 0) {
      inByte = Serial.read();
      if (inByte == 'H')
      digitalWrite(ActuatorPin, HIGH);
      if (inByte == 'L')
      digitalWrite(ActuatorPin, LOW);
  }
}
