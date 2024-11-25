#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <ws2tcpip.h> // Adicionar esta linha

#pragma comment(lib, "ws2_32.lib")

#define PORT 1234
#define BUFFER_SIZE 1024

void handle_client(SOCKET client_socket) {
    // Enviar uma mensagem para o cliente
    std::string message = "Hello from server";
    send(client_socket, message.c_str(), message.size(), 0);

    // Fechar a conexão com o cliente
    closesocket(client_socket);
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);
    std::vector<std::thread> threads;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return EXIT_FAILURE;
    }

    // Criar socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Could not create socket" << std::endl;
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Obter o nome do host
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        std::cerr << "Error getting hostname" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Obter o endereço IP do host
    struct hostent* host = gethostbyname(hostname);
    if (host == nullptr) {
        std::cerr << "Error getting host by name" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Configurar endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Imprimir o endereço IP do servidor
    std::cout << "Server IP: " << inet_ntoa(*(struct in_addr*)host->h_addr) << std::endl;

    // Vincular o socket ao endereço e porta
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Escutar por conexões
    if (listen(server_socket, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }
    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        // Aceitar uma conexão de cliente
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Client connection failed" << std::endl;
            continue;
        }
        std::cout << "Connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

        // Criar uma nova thread para lidar com o cliente
        threads.emplace_back(handle_client, client_socket);
    }

    // Fechar o socket do servidor
    closesocket(server_socket);
    WSACleanup();

    // Esperar todas as threads terminarem
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return 0;
}
