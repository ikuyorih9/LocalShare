#include <Arduino.h>
#include <Wire.h>
#include "frtos.hpp"
#include "i2c.hpp"

QueueHandle_t dataQueue;

void setup() {
  Serial.begin(115200);

  // Configura I2C
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(onRequest);
  Wire.onReceive(onReceive);

  // Cria a fila
  dataQueue = xQueueCreate(10, sizeof(char) * 128);
  if (dataQueue == NULL) {
    Serial.println("Falha ao criar a fila!");
  }

  // Cria a tarefa WebSocket
  xTaskCreate(
    Tarefa1,         // Função da tarefa
    "Tarefa1",       // Nome da tarefa
    8192,            // Tamanho da pilha
    NULL,            // Parâmetro passado para a tarefa
    1,               // Prioridade
    NULL             // Handle
  );

  // Cria a tarefa I2C
  xTaskCreate(
    Tarefa2,         // Função da tarefa
    "Tarefa2",       // Nome da tarefa
    4096,            // Tamanho da pilha
    NULL,            // Parâmetro passado para a tarefa
    1,               // Prioridade
    NULL             // Handle
  );
}

void loop() {
  // Nada a ser feito aqui
}
