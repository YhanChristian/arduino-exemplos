/* 
  Implantação de Controle de Temperatura c/ Arduino e Scilab
  Utilizado: Arduino Uno, LM35, Motor DC
  Autor: Yhan Christian Souza Silva - Data: 14/09/2016
  Referência: https://www.scilab.org/community/news/Scilab-Arduino-low-cost-data-acquisition
 */


 #define sensor_tempratura A0 //Pino A0, sensor de temperatura
 #define motor_dc 3           //Pino 3, motor DC

 float leitura_sensor; //variavel para armazenar temperatura
 char status_motor;	// variavel com status do motor

 void setup(){
 	pinMode(motor_dc, OUTPUT); //Motor DC como saída digital
 	Serial.begin(9600);		   //Inicia comunicação 
 }

 void loop(){
 	leitura_sensor = (float(analogRead(sensor_tempratura)) * 5 / (1023)) / 0.01; //leitura da temperatura e armazena na variável leitura_sensor
 	//verifica se a serial está habilitada
 	if(!Serial.available()){
 		Serial.println(leitura_sensor, 1);
 		delay(1000);
 	}

 	// condição para acionamento do motor, se ler H - liga se receber L - desliga
 	while(Serial.available() > 0) {
 		status_motor = Serial.read();
 		if(status_motor == 'H') digitalWrite(motor_dc, HIGH);
 		else if(status_motor == 'L') digitalWrite(motor_dc, LOW); 	    
 	}
 }