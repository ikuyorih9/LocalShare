#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h> // Adicionar esta linha

#pragma comment(lib, "ws2_32.lib")

#define PORT 1234
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return EXIT_FAILURE;
    }

    // Criar socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Could not create socket" << std::endl;
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Configurar endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Conectar ao servidor
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(client_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Receber mensagem do servidor
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received == SOCKET_ERROR) {
        std::cerr << "Receive failed" << std::endl;
    } else {
        buffer[bytes_received] = '\0';
        std::cout << "Message from server: " << buffer << std::endl;
    }

    // Fechar o socket do cliente
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
