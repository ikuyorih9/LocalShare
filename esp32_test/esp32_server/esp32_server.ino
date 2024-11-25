#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

// Configurações do WiFi
const char* ssid = "ISS";
const char* password = "Nbsdpt11";

// Variáveis globais
WiFiUDP udp;
Coap coap(udp);
bool received_file_name = false;

// Função para lidar com as requisições PUT
void handle_put(CoapPacket &packet, IPAddress ip, int port) {
    if (!received_file_name) {
        // Nome do arquivo recebido
        String file_name = String((char*)packet.payload, packet.payloadlen);
        Serial.printf("Received file name: %s\n", file_name.c_str());
        
        // Enviar resposta ao cliente
        coap.sendResponse(ip, port, packet.messageid, "File name received", strlen("File name received"));
        received_file_name = true;
    } else {
        // Dados do arquivo recebidos
        Serial.printf("Received file data of size: %d bytes\n", packet.payloadlen);
        Serial.printf("File data: %.*s\n", packet.payloadlen, packet.payload);
        
        // Enviar resposta ao cliente
        coap.sendResponse(ip, port, packet.messageid, "Data received", strlen("Data received"));
    }
}

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

    // Inicializar o servidor CoAP
    coap.server(handle_put, "file");
    coap.start();
}

void loop() {
    coap.loop();
}
