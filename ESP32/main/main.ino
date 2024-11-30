// ESP32: main.cpp

#include <Arduino.h>
#include <Wire.h>
#include "esp_task_wdt.h"
#include "frtos.hpp"
#include "i2c.hpp"

QueueHandle_t DataToSend;

void setup() {
  esp_task_wdt_deinit();
  Serial.begin(115200);

  Wire.begin(0x08);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);

  DataToSend = xQueueCreate(32, sizeof(uint8_t));  // Criação da fila com capacidade de 10 elementos
  if (DataToSend == NULL) {
      Serial.println("Erro ao criar a fila");
      while (1);
  }
  
  xTaskCreatePinnedToCore(
    Tarefa1,           // Função da tarefa
    "Tarefa1",         // Nome da tarefa
    4096,              // Tamanho da pilha
    NULL,              // Parâmetro (se necessário)
    1,                 // Prioridade
    NULL,              // Handle (opcional)
    0                  // Núcleo onde a tarefa será executada (0 para o núcleo 0)
  );

  // Tarefa2 executando no Núcleo 1
  // xTaskCreatePinnedToCore(
  //   Tarefa2,           // Função da tarefa
  //   "Tarefa2",         // Nome da tarefa
  //   4096,              // Tamanho da pilha
  //   NULL,              // Parâmetro
  //   1,                 // Prioridade
  //   NULL,              // Handle (opcional)
  //   1                  // Núcleo onde a tarefa será executada (1 para o núcleo 1)
  // );

}

void loop() {
  delay(100);
}
