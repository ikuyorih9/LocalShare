#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "esp_task_wdt.h"
#include "frtos.hpp"
#include "i2c.hpp"
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

// Definindo o servidor WebSocket e a porta
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Definindo o WiFiClient
QueueHandle_t DataToSend;
QueueHandle_t DataToReceive;
WiFiClient client;

// Função de setup
void setup() {
  // Desabilita watchdog para controle manual
  esp_task_wdt_deinit();
  Serial.begin(115200);
  
  // Inicializa comunicação I2C
  Wire.begin(0x08);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);

  // Conexão WiFi
  WiFi.begin(SSID, PASSWORD); // Inicia conexão Wi-Fi.
  while (WiFi.status() != WL_CONNECTED) { // Enquanto não conecta
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Task espera 1 segundo.
  }

  Serial.println("WiFi conectado!");
  Serial.print("IP da ESP32: ");
  Serial.println(WiFi.localIP());

  // Criação das filas
  DataToSend = xQueueCreate(32, sizeof(uint8_t));
  if (DataToSend == NULL) {
    Serial.println("Erro ao criar a fila");
    while (1);
  }

  DataToReceive = xQueueCreate(32, sizeof(uint8_t));
  if (DataToReceive == NULL) {
    Serial.println("Erro ao criar a fila");
    while (1);
  }

  // Configuração do servidor WebSocket
  ws.onEvent(onWebSocketEvent); // Evento de WebSocket
  server.addHandler(&ws); // Adiciona o WebSocket ao servidor

  // Rota HTTP simples para teste
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Servidor WebSocket da ESP32 está rodando!");
  });

  // Inicia o servidor
  server.begin();
  Serial.println("Servidor WebSocket iniciado!");

  // Criação das tarefas
  xTaskCreatePinnedToCore(Tarefa1, "Tarefa1", 4096, NULL, 1, NULL, 0);  // Tarefa 1 no Núcleo 0
  xTaskCreatePinnedToCore(Tarefa2, "Tarefa2", 4096, NULL, 1, NULL, 1);  // Tarefa 2 no Núcleo 1
}

// Função de loop principal
void loop() {
  // O servidor WebSocket funciona de forma assíncrona
  // O loop fica livre para outras tarefas ou monitoramento
  if(client.connect(IP, PORTA)){
    Serial.println("Cliente conectado!");
  }
  while(client.connected()) {
    // A lógica de cliente já é tratada em Tarefa1 e Tarefa2
  }
  client.stop();
  Serial.println("Desconectado do servidor!");
  delay(100);
}

// Função de manipulação do evento WebSocket
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    // Verifica se os dados são suficientes
    if (len < 1) {
      client->text("Dados insuficientes.");
      return;
    }

    // Lê o comando (primeiro byte da mensagem)
    uint8_t command = data[0];
    Serial.printf("Comando recebido do WebSocket: 0x%02X\n", command);

    if (command == 0x02 || command == 0x03) {
      // Comandos 0x02 e 0x03 não requerem mais dados
      // Espera a resposta da Raspberry Pi e repassa via WebSocket
      Wire.beginTransmission(0x08); // Endereço da Raspberry Pi
      Wire.write(command);
      Wire.endTransmission();
      
      // Aguarda resposta da Raspberry Pi
      delay(100); 
      Wire.requestFrom(0x08, 32); // Solicita até 32 bytes da Raspberry Pi
      String response = "";
      
      while (Wire.available()) {
        char c = Wire.read();
        response += c;
      }

      Serial.println("Resposta recebida da Raspberry Pi:");
      Serial.println(response);

      // Envia a resposta de volta ao cliente WebSocket
      if (!response.isEmpty()) {
        client->text(response);
      } else {
        client->text("Nenhuma resposta recebida da Raspberry Pi.");
      }
    } else if (command == 0x00) {
      // Recebe os dados adicionais para arquivo
      // Pega tamanho do nome do arquivo e do arquivo
      uint8_t fileNameSize = data[1];
      char fileName[fileNameSize + 1];  // Tamanho do nome do arquivo
      fileName[fileNameSize] = '\0';

      // Preenche o nome do arquivo
      for (size_t i = 0; i < fileNameSize; i++) {
        fileName[i] = data[2 + i];
      }
      Serial.printf("Nome do arquivo: %s\n", fileName);

      uint32_t fileSize = (data[2 + fileNameSize] << 24) | (data[3 + fileNameSize] << 16) |
                          (data[4 + fileNameSize] << 8) | data[5 + fileNameSize];
      Serial.printf("Tamanho do arquivo: %d bytes\n", fileSize);

      // Agora começa o recebimento dos dados do arquivo
      size_t offset = 6 + fileNameSize;
      size_t bytesReceived = 0;
      while (bytesReceived < fileSize) {
        size_t bytesToRead = min(static_cast<size_t>(fileSize - bytesReceived), len - offset);
        for (size_t i = 0; i < bytesToRead; i++) {
          uint8_t byte = data[offset + i];
          // Aqui você pode armazenar os bytes recebidos em um buffer ou processá-los como quiser.
          // Exemplo: escrever no arquivo ou enviar para a Raspberry Pi via I2C.
          Wire.write(byte);
        }
        bytesReceived += bytesToRead;
        offset += bytesToRead;
      }

      Serial.printf("Arquivo '%s' de %d bytes recebido.\n", fileName, bytesReceived);
      client->text("Arquivo recebido com sucesso.");
    } else {
      client->text("Comando desconhecido.");
    }
  }
}
