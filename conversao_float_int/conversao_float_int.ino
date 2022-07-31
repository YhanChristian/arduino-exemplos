float h;

typedef struct teste  {
  uint16_t h_int;
};


teste new_teste;

void setup() {
  Serial.begin(9600);

}

void loop() {
  h += 0.3;
  uint8_t value[2];

  // Pego valor float e passo pra inteiro em 2 registros
  value[1] = (uint8_t) h;
  value[0] = uint8_t((h - uint8_t(h)) * 10); 

  //Jogo o valor do registro na estrutura
  new_teste.h_int = *((uint16_t*)value);
  //new_teste.h_int = value[0] + (value[1] << 8);
  //*(uint16_t*) (new_teste.h_int) = (unsigned)value[1] << 8 | (unsigned)value[0];
  
  Serial.print("Valor float: ");
  Serial.print(h);
  Serial.print(" Valor value[0]: ");
  Serial.print(value[0]);
  Serial.print(" Valor value[1]: ");
  Serial.print(value[1]);
  Serial.print(" Valor int: ");
  Serial.println(new_teste.h_int);

  delay(3000);

}
