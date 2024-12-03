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

// Variáveis para armazenar o arquivo recebido
bool receivingFile = false;
String receivedFileName = "";
int receivedFileSize = 0;
int receivedBytesCount = 0;
uint8_t* fileBuffer = nullptr;

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

// Função de evento WebSocket
void webSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_TEXT) {
    String message = String((char*)payload);

    if (!receivingFile) {
      // Primeira parte: comando e informações do arquivo
      if (message[0] == 0x00) {
        receivingFile = true;
        int fileNameSize = message[1];  // Tamanho do nome do arquivo
        receivedFileName = message.substring(2, 2 + fileNameSize);  // Nome do arquivo
        receivedFileSize = (message[2 + fileNameSize] << 24) |
                           (message[3 + fileNameSize] << 16) |
                           (message[4 + fileNameSize] << 8) |
                           message[5 + fileNameSize];  // Tamanho do arquivo

        // Inicializa o buffer para armazenar os dados do arquivo
        fileBuffer = (uint8_t*)malloc(receivedFileSize);
        receivedBytesCount = 0;

        Serial.println("Iniciando recepção do arquivo...");
        Serial.print("Nome do arquivo: ");
        Serial.println(receivedFileName);
        Serial.print("Tamanho do arquivo: ");
        Serial.println(receivedFileSize);
      }
    } else {
      // Recebendo os bytes do arquivo
      for (size_t i = 0; i < length; i++) {
        if (receivedBytesCount < receivedFileSize) {
          fileBuffer[receivedBytesCount++] = payload[i];
          
          // Envia o byte via I2C assim que ele for recebido
          sendByteToI2C(fileBuffer[receivedBytesCount - 1]);
        }
      }

      Serial.print("Recebido: ");
      Serial.print(receivedBytesCount);
      Serial.print("/");
      Serial.println(receivedFileSize);

      // Quando o arquivo estiver completo
      if (receivedBytesCount >= receivedFileSize) {
        Serial.println("Arquivo completo enviado.");
        resetFileReception();  // Reseta as variáveis de recepção de arquivo
      }
    }
  }
}

// Função para enviar um byte via I2C
void sendByteToI2C(uint8_t byte) {
  Wire.beginTransmission(I2C_ADDRESS);  // Inicia a transmissão para o endereço I2C da Raspberry Pi
  Wire.write(byte);  // Envia o byte
  Wire.endTransmission();  // Finaliza a transmissão
}

void resetFileReception() {
  receivingFile = false;
  receivedFileName = "";
  receivedFileSize = 0;
  receivedBytesCount = 0;
  if (fileBuffer) {
    free(fileBuffer);
    fileBuffer = nullptr;
  }
}