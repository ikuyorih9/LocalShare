#ifndef FRTOS_HPP
#define FRTOS_HPP

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// Configurações do Wi-Fi
#define SSID "LabMicros"
#define PASSWORD "seluspeesc@"

// Propriedades do WebSocket
#define IP "192.168.137.1"
#define PORTA 1234

// Declarações das funções
void Tarefa1(void *pvParameters);
void Tarefa2(void *pvParameters);

// Declaração da função onWebSocketEvent
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

#endif
