#include <Arduino.h>

#include "frtos.hpp"

void Tarefa1(void *pvParameters) {
  while (true) {
    Serial.println("Tarefa 1 executando...");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Atraso de 1 segundo
  }
}

// Função da Tarefa 2
void Tarefa2(void *pvParameters) {
  while (true) {
    Serial.println("Tarefa 2 executando...");
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Atraso de 2 segundos
  }
}