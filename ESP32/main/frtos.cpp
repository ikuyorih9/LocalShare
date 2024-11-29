#include <Arduino.h>
#include <WiFi.h>
#include <HardwareSerial.h>
#include "frtos.hpp"
#include "i2c.hpp"

HardwareSerial SerialPort(1);
extern QueueHandle_t DataToSend;
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
          // RECEBE O COMANDO
          uint8_t command;
          client.readBytes(&command, 1);
          Serial.print("TASK1: comando = ");
          Serial.println(command);
          xQueueSend(DataToSend, command, portMAX_DELAY);

          // RECEBE O TAMANHO DO NOME DE USUÁRIO.
          uint8_t username_len;
          client.readBytes(&username_len, 1); // Recebe do servidor.
          Serial.print("TASK1: username_len = ");
          Serial.println(username_len);

          xQueueSend(DataToSend, &username_len, portMAX_DELAY); // Envia para a fila.
          
          // RECEBE O NOME DE USUÁRIO.
          char username[username_len + 1];
          client.readBytes(username, username_len);
          username[username_len] = '\0';
          Serial.print("TASK1: username = ");
          Serial.println(username);

          xQueueSend(DataToSend, username, portMAX_DELAY);

          // RECEBE O TAMANHO DA SENHA.
          uint8_t password_len;
          client.readBytes(&password_len, 1); // Recebe do servidor.
          Serial.print("TASK1: password_len = ");
          Serial.println(password_len);
          xQueueSend(DataToSend, &password_len, portMAX_DELAY); // Envia para a fila.

          // RECEBE SENHA.
          char password[password_len + 1];
          client.readBytes(password, password_len);
          password[password_len] = '\0';
          Serial.print("TASK1: password = ");
          Serial.println(password);

          xQueueSend(DataToSend, password, portMAX_DELAY);

          if(command == 0 || command == 1){
            // RECEBE O TAMANHO DO ARQUIVO.
            uint8_t filename_len;
            client.readBytes(&filename_len, 1);
            xQueueSend(DataToSend, &filename_len, portMAX_DELAY);

            char filename[filename_len + 1];
            client.readBytes(filename, filename_len);
            filename[filename_len] = '\0';
            Serial.print("FILENAME: ");
            Serial.println(filename);

            xQueueSend(DataToSend, filename, portMAX_DELAY);
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
  while (1) {
      uint8_t * command = (uint8_t *)malloc(sizeof(uint8_t));
      if (xQueueReceive(DataToSend, &command, portMAX_DELAY)) {
          Serial.print("TASK2: comando = ");
          Serial.println((uint8_t) &command);
      }
      free(command)

      uint8_t tamUser;
      if (xQueueReceive(DataToSend, &tamUser, portMAX_DELAY)) {
          Serial.print("TASK2: username_len = ");
          Serial.println(tamUser);
      }

      char username[tamUser+1];
      if (xQueueReceive(DataToSend, username, portMAX_DELAY)) {
          username[tamUser] = '\0';
          Serial.print("TASK2: username = ");
          Serial.println(username);
      }

      uint8_t password_len;
      if (xQueueReceive(DataToSend, &password_len, portMAX_DELAY)) {
          Serial.print("TASK2: password_len = ");
          Serial.println(password_len);
      }

      char password[password_len+1];
      if (xQueueReceive(DataToSend, password, portMAX_DELAY)) {
          password[password_len] = '\0';
          Serial.print("TASK2: password = ");
          Serial.println(password);
      }

      if(command == 0 || command == 1){
        uint8_t tamFilename;
        if (xQueueReceive(DataToSend, &tamFilename, portMAX_DELAY)) {
            Serial.print("TASK2: filename_tam = ");
            Serial.println(tamFilename);
        }

        char filename[tamFilename+1];
        if (xQueueReceive(DataToSend, filename, portMAX_DELAY)) {
            filename[tamFilename] = '\0';
            Serial.print("TASK2: filename = ");
            Serial.println(filename);
        }
      }

  }
  // SerialPort.begin(115200, SERIAL_8N1, 16, 17);
  // while (true){
  //   char c = 'H';
  //   Serial.println("Enviando: " + String(c));
  //   SerialPort.println(c); // Envia via UART1
  //   vTaskDelay(1000 / portTICK_PERIOD_MS);
  // }


}

uint byte4_to_int(uint8_t * byte){
  uint int_from_byte = 0;
  for(int i = 3; i >= 0; i--){
    int_from_byte += byte[i] << i*8;
  }
  return int_from_byte;
}

void sendStringToQueue(char * message, uint8_t tam){
  for(uint8_t i = 0; i < tam; i++){
    uint8_t byte = message[i];
    xQueueSend(DataToSend, &byte, portMAX_DELAY);
  }
}