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
      // RECEBE O COMANDO
      uint8_t command;
      client.readBytes(&command, 1);
      Serial.print("TASK1: comando = ");
      Serial.println(command);
      xQueueSend(DataToSend, &command, portMAX_DELAY);

      vTaskDelay(100 / portTICK_PERIOD_MS);

      // RECEBE O TAMANHO DO NOME DE USUÁRIO.
      uint8_t username_len;
      client.readBytes(&username_len, 1); // Recebe do servidor.
      Serial.print("TASK1: username_len = ");
      Serial.println(username_len);

      xQueueSend(DataToSend, &username_len, portMAX_DELAY); // Envia para a fila.
      
      vTaskDelay(1 / portTICK_PERIOD_MS);

      // RECEBE O NOME DE USUÁRIO.
      char username[username_len + 1];
      client.readBytes(username, username_len);
      username[username_len] = '\0';
      Serial.print("TASK1: username = ");
      Serial.println(username);

      sendStringToQueue(username, username_len);

      // RECEBE O TAMANHO DA SENHA.
      uint8_t password_len;
      client.readBytes(&password_len, 1); // Recebe do servidor.
      Serial.print("TASK1: password_len = ");
      Serial.println(password_len);
      xQueueSend(DataToSend, &password_len, portMAX_DELAY);

      vTaskDelay(1 / portTICK_PERIOD_MS);

      // RECEBE SENHA.
      char password[password_len + 1];
      client.readBytes(password, password_len);
      password[password_len] = '\0';
      Serial.print("TASK1: password = ");
      Serial.println(password);

      sendStringToQueue(password, password_len);

      if(command == 0){
        // RECEBE O TAMANHO DO ARQUIVO.
        uint8_t filename_len;
        client.readBytes(&filename_len, 1);
        xQueueSend(DataToSend, &filename_len, portMAX_DELAY);
        Serial.print("TASK1: filename_len = ");
        Serial.println(filename_len);

        char filename[filename_len + 1];
        client.readBytes(filename, filename_len);
        filename[filename_len] = '\0';
        Serial.print("TASK1: filename = ");
        Serial.println(filename);

        sendStringToQueue(filename, filename_len);

        uint8_t file_len[4];
        client.readBytes(file_len, 4);
        sendBytesToQueue(file_len, 4); 

        int ifile_len = byte4_to_int(file_len);
        Serial.print("TASK1: file_len = ");
        Serial.println(ifile_len);

        int bytes_lidos = 0;
        while(bytes_lidos < ifile_len){
          uint8_t data_byte;
          bytes_lidos += client.readBytes(&data_byte, 1);
          xQueueSend(DataToSend, &data_byte, portMAX_DELAY);
        }
      }
      else if(command == 1){
        uint8_t filename_len;
        client.readBytes(&filename_len, 1);
        xQueueSend(DataToSend, &filename_len, portMAX_DELAY);
        Serial.print("TASK1: filename_len = ");
        Serial.println(filename_len);

        char filename[filename_len + 1];
        client.readBytes(filename, filename_len);
        filename[filename_len] = '\0';
        Serial.print("TASK1: filename = ");
        Serial.println(filename);
        sendStringToQueue(filename, filename_len);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // Aguardar antes de tentar novamente
  }
}

// Função da Tarefa 2
void Tarefa2(void *pvParameters) {
  while(true){
    if (uxQueueMessagesWaiting(DataToReceive) == 0) {
        //Serial.println("2: A fila está vazia.");
        uint8_t hugo;
    }
    else{
      uint8_t byte;
      if (xQueueReceive(DataToReceive, &byte, portMAX_DELAY)) {
          Serial.print("\t\tByte: ");
          Serial.println(byte, DEC);
          client.write(byte);
          vTaskDelay(1 / portTICK_PERIOD_MS); // Aguardar antes de tentar novamente
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
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}
