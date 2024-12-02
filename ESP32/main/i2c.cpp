#include "i2c.hpp"

#include <Arduino.h>
#include <Wire.h>

extern QueueHandle_t DataToSend;
extern QueueHandle_t DataToReceive;


void onRequest() {

  // Verifica o número de mensagens na fila
  if (uxQueueMessagesWaiting(DataToSend) == 0) {
      // Serial.println("1: A fila está vazia.");
      Wire.write(-1);
      return;
  }

  uint8_t byte;
  if (xQueueReceive(DataToSend, &byte, portMAX_DELAY)) {
      Wire.write(byte);
  }
}

void onReceive(int numBytes) {
  while (Wire.available()) {
    uint8_t reg = Wire.read(); // Lê dados do Master
    if (reg == 0x01){
      uint8_t byte = Wire.read(); // Lê dados do Master
      xQueueSend(DataToReceive, &byte, portMAX_DELAY);
    }
    else if (reg == 0x02){
      for(int i = 0; i < 4; i++){
        uint8_t byte = Wire.read(); // Lê dados do Master
        xQueueSend(DataToReceive, &byte, portMAX_DELAY);
      }
    }
  }
}