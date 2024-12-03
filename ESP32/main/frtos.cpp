#include <Arduino.h>
#include <WiFi.h>
#include "frtos.hpp"
#include "i2c.hpp"

extern QueueHandle_t DataToSend;
extern QueueHandle_t DataToReceive;
extern WiFiClient client;

void Tarefa1(void *pvParameters) {
  while(true){
    if (client.available()) {
      uint8_t byte;
      client.readBytes(&byte, 1);
      xQueueSend(DataToSend, &byte, portMAX_DELAY); //Envia o COMANDO para a fila.
      vTaskDelay(RW_DELAY / portTICK_PERIOD_MS);
    }
  }
}

// Função da Tarefa 2
void Tarefa2(void *pvParameters) {
  while(true){
    if (uxQueueMessagesWaiting(DataToReceive) != 0) {
      uint8_t byte;
      if (xQueueReceive(DataToReceive, &byte, portMAX_DELAY)) {
          Serial.print("\t\tByte: ");
          Serial.println(byte, DEC);
          client.write(byte);
          vTaskDelay(RW_DELAY / portTICK_PERIOD_MS);
      }
    }
  }
}

uint byte4_to_int(uint8_t * byte){
  uint int_from_byte = 0;
  for(int i = 0; i < 4; i++){
    int_from_byte <<= 8;
    int_from_byte += byte[i];
  }
  return int_from_byte;
}

void sendStringToQueue(char * message, int tam){
  for(int i = 0; i < tam; i++){
    int byte = message[i];
    Serial.print("\t");
    Serial.println(byte);
    xQueueSend(DataToSend, &byte, portMAX_DELAY);
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void sendBytesToQueue(uint8_t * bytes, int tam){
  for(int i = 0; i < tam; i++){
    int byte = bytes[i];
    xQueueSend(DataToSend, &byte, portMAX_DELAY);
  }
}
