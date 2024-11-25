// ESP32: main.cpp

#include <Arduino.h>
#include "frtos.hpp"
#include "i2c.hpp"

void setup() {
  Wire.begin(I2C_ADDRESS); // Configura ESP32 como Slave
  Wire.onRequest(onRequest); // Callback quando o Master solicita dados
  Wire.onReceive(onReceive); // Callback quando o Master envia dados
  Serial.begin(115200);

  xTaskCreate(
    Tarefa1,         // Função da tarefa
    "Tarefa1",       // Nome da tarefa
    1024,            // Tamanho da pilha (em palavras)
    NULL,            // Parâmetro passado para a tarefa
    1,               // Prioridade
    NULL             // Handle (opcional)
  );

  xTaskCreate(
    Tarefa2,         // Função da tarefa
    "Tarefa2",       // Nome da tarefa
    1024,            // Tamanho da pilha
    NULL,            // Parâmetro
    1,               // Prioridade
    NULL             // Handle (opcional)
  );
}

void loop() {
  delay(100);

}
