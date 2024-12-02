#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include "frtos.hpp"
#include <Wire.h>
#include "i2c.hpp"

// Configurações do Wi-Fi
const char* ssid = "LabMicros";
const char* password = "seluspeesc@";

// Servidor e WebSocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Fila global para comunicação entre tarefas
extern QueueHandle_t dataQueue;

// Tarefa 1: Configuração do WebSocket
void Tarefa1(void *pvParameters) {
  // Conexão Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Conectado ao Wi-Fi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Configuração do WebSocket
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Rota para teste
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Servidor WebSocket da ESP32 está rodando!");
  });

  // Inicia o servidor
  server.begin();
  Serial.println("Servidor WebSocket iniciado!");

  // Loop da tarefa
  while (true) {
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Mantém a tarefa viva
  }
}

// Manipulador do WebSocket
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    // Processa a mensagem recebida
    String message = String((char*)data).substring(0, len);
    Serial.printf("Mensagem recebida: %s\n", message.c_str());

    // Deserializa mensagem JSON
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

          // Envia dados para a fila
          String payload = "LOGIN: " + username + "," + password;
          if (xQueueSend(dataQueue, payload.c_str(), 0) != pdPASS) {
            Serial.println("Fila cheia. Não foi possível enviar os dados.");
          }
        } else {
          client->text("Login falhou");
        }
      }
    } else {
      client->text("Erro no formato da mensagem");
    }
  }
}

// Tarefa 2: Comunicação via I2C
void Tarefa2(void *pvParameters) {
  char buffer[128]; // Buffer para armazenar os dados recebidos da fila

  while (true) {
    // Aguarda dados na fila
    if (xQueueReceive(dataQueue, buffer, portMAX_DELAY) == pdPASS) {
      Serial.print("Enviando para I2C: ");
      Serial.println(buffer);

      // Envia os dados via I2C
      Wire.beginTransmission(I2C_ADDRESS);
      Wire.write((uint8_t*)buffer, strlen(buffer));
      Wire.endTransmission();
    }

    vTaskDelay(100 / portTICK_PERIOD_MS); // Aguarda antes de checar novamente
  }
}
