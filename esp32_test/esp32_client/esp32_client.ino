#include <WiFi.h>

// Configurações do WiFi
const char* ssid = "ISS";
const char* password = "Nbsdpt11";

// Endereço IP do servidor e porta
const char* server_ip = "192.168.195.138";  // Atualize com o IP do servidor
const uint16_t server_port = 1234;

// Tamanho do buffer
#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

void setup() {
    Serial.begin(115200);

    // Conectar ao WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Conectar ao servidor TCP
    WiFiClient client;
    if (!client.connect(server_ip, server_port)) {
        Serial.println("Connection to server failed");
        return;
    }
    Serial.println("Connected to server");

    // Receber mensagem do servidor
    int bytes_received = client.readBytes(buffer, BUFFER_SIZE);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';  // Certificar-se de que o buffer está terminado em nulo
        Serial.print("Message from server: ");
        Serial.println(buffer);
    } else {
        Serial.println("Receive failed");
    }

    // Fechar o socket do cliente
    client.stop();
    Serial.println("Disconnected from server");
}

void loop() {
    // Nada para fazer no loop
}
