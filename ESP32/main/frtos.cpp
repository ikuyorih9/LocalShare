#include <Arduino.h>
#include <WiFi.h>
#include "frtos.hpp"

void Tarefa1(void *pvParameters) {
  // CONEXÃO WIFI
  WiFi.begin(SSID, PASSWORD); // Inicia conexão wifi.
  while (WiFi.status() != WL_CONNECTED) { // Enquanto não conecta:
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Task espera 1 segundo.
  }
  
  while(true){
    // CONEXÃO COM O SERVIDOR.
    WiFiClient client;
    if (client.connect(IP, PORTA)){ // Tenta conectar ao servidor
      client.println("CLIENTE pronto para RECEBER do servidor!");

      while (client.connected()) {
        if (client.available()) {
          uint8_t username_len;
          client.readBytes(username_len, 1)

          uint8_t filename_len;
          client.readBytes(filename_len, 1)
          
          String response = client.readStringUntil('\n');
          Serial.println("Resposta do servidor: " + response);
          Serial.println("Tarefa 1 executando...");
        }
      }

      client.stop();
      Serial.println("Desconectado do servidor!");
    }
    else{ // Falha ao conectar ao servidor
      Serial.println("Falha ao conectar ao servidor!");
    }

    vTaskDelay(2500 / portTICK_PERIOD_MS); // Aguardar antes de tentar novamente
  }
}

// Função da Tarefa 2
void Tarefa2(void *pvParameters) {
  while (true) {
    Serial.println("Tarefa 2 executando...");
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Atraso de 2 segundos
  }
}