#include "i2c.hpp"

#include <Arduino.h>
#include <Wire.h>

extern QueueHandle_t DataToSend;

void onRequest() {

  // Verifica o número de mensagens na fila
  if (uxQueueMessagesWaiting(DataToSend) == 0) {
      Serial.println("A fila está vazia.");
      Wire.write(-1);
      return;
  }

  uint8_t byte;
  if (xQueueReceive(DataToSend, &byte, portMAX_DELAY)) {
      Serial.print("Enviando o byte");
      Serial.println(byte);
      Wire.write(byte);
  }
}

void onReceive(int numBytes) {
  while (Wire.available()) {
    char c = Wire.read(); // Lê dados do Master
    Serial.print(c);
  }
  Serial.println();
}