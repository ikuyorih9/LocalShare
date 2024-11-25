#include "i2c.hpp"

#include <Wire.h>

void onRequest() {
  const char* message = "Hello Pi!";
  Wire.write((uint8_t*)message, strlen(message)); // Converte para uint8_t* e envia o tamanho da mensagem
}
void onReceive(int numBytes) {
  while (Wire.available()) {
    char c = Wire.read(); // Lê dados do Master
    Serial.print(c);
  }
  Serial.println();
}