#include <coap3/coap.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *file_name = argv[1];
    printf("Reading file: %s\n", file_name);

    std::ifstream file(file_name, std::ios::binary);
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", file_name);
        return EXIT_FAILURE;
    }

    coap_startup();

    coap_context_t *ctx = coap_new_context(nullptr);
    if (!ctx) {
        fprintf(stderr, "Failed to create context\n");
        return EXIT_FAILURE;
    }

    coap_address_t dst_addr;
    coap_address_init(&dst_addr);
    dst_addr.addr.sin.sin_family = AF_INET;
    dst_addr.addr.sin.sin_port = htons(5683);
    inet_pton(AF_INET, "192.168.195.51", &dst_addr.addr.sin.sin_addr);  // Atualize o IP

    coap_session_t *session = coap_new_client_session(ctx, nullptr, &dst_addr, COAP_PROTO_UDP);
    if (!session) {
        fprintf(stderr, "Failed to create client session\n");
        return EXIT_FAILURE;
    }

    // Obter o tamanho do arquivo
    file.seekg(0, std::ios::end);
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Ler o conteúdo do arquivo
    std::vector<char> file_data(file_size);
    if (!file.read(file_data.data(), file_size)) {
        fprintf(stderr, "Failed to read file: %s\n", file_name);
        return EXIT_FAILURE;
    }

    // Enviar nome do arquivo
    printf("Sending file name: %s\n", file_name);
    coap_pdu_t *pdu = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_PUT, coap_new_message_id(session), coap_session_max_pdu_size(session));
    coap_add_option(pdu, COAP_OPTION_URI_PATH, 4, (const uint8_t *)"file");
    coap_add_data(pdu, strlen(file_name), (const uint8_t *)file_name);
    coap_send(session, pdu);

    // Pausa para garantir que o nome do arquivo seja processado
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Enviar dados do arquivo
    printf("Sending file data of size: %ld\n", file_size);
    pdu = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_PUT, coap_new_message_id(session), coap_session_max_pdu_size(session));
    coap_add_option(pdu, COAP_OPTION_URI_PATH, 4, (const uint8_t *)"file");
    coap_add_data(pdu, file_size, reinterpret_cast<const uint8_t *>(file_data.data()));
    coap_send(session, pdu);

    coap_io_process(ctx, COAP_IO_WAIT);

    coap_session_release(session);
    coap_free_context(ctx);
    coap_cleanup();

    return EXIT_SUCCESS;
}
