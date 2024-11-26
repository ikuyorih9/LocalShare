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
          client.readBytes(&username_len, 1);
          
          char username[username_len + 1];
          client.readBytes(username, username_len);
          username[username_len] = '\0';
          Serial.print("USERNAME: ");
          Serial.println(username);

          uint8_t filename_len;
          client.readBytes(&filename_len, 1);
          
          char filename[filename_len + 1];
          client.readBytes(filename, filename_len);
          username[filename_len] = '\0';
          Serial.print("FILENAME: ");
          Serial.println(filename);

          uint8_t filesize_b[4];
          client.readBytes(filesize_b, 4);

          uint filesize = byte4_to_int(filesize_b);
          Serial.print("Tamanho do arquivo: ");
          Serial.println(filesize);

          for(int i = 0; i < filesize; i++){
            char byte;
            client.readBytes(&byte, 1);
            Serial.print(byte);
          }


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

uint byte4_to_int(uint8_t * byte){
  uint int_from_byte = 0;
  for(int i = 3; i >= 0; i--){
    int_from_byte += byte[i] << i*8;
  }
  return int_from_byte;
}