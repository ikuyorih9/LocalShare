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
    // Processa a mensagem recebida
    String message = String((char*)data).substring(0, len);
    Serial.printf("Mensagem recebida: %s\n", message.c_str());

    // Deserializa a mensagem JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, message);

    if (!error) {
      String messageType = doc["type"];

      if (messageType == "login") {
        String username = doc["username"];
        String password = doc["password"];

        // Lógica fictícia de validação de login
        if (username == "admin" && password == "admin") {
          client->text("Login bem-sucedido");
        } else {
          client->text("Login falhou");
        }
      }
    } else {
      client->text("Erro no formato da mensagem");
    }
  }
}
