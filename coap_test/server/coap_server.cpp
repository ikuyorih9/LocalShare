#include <coap3/coap.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static coap_context_t *ctx = nullptr;
static coap_resource_t *resource = nullptr;
static FILE *file = nullptr;
static bool received_file_name = false;

void handle_put(coap_resource_t *resource, coap_session_t *session,
                const coap_pdu_t *request, const coap_string_t *query,
                coap_pdu_t *response) {
    size_t size;
    const uint8_t *data;

    if (coap_get_data(request, &size, &data)) {
        if (!received_file_name) {
            // Logging the received file name
            printf("Received file name: %.*s\n", (int)size, (const char *)data);

            // Use strncpy to avoid adding extra null character
            char file_name[256];
            strncpy(file_name, (const char *)data, size);
            file_name[size] = '\0';  // Null-terminate the string

            file = fopen(file_name, "wb");

            if (file) {
                printf("File created successfully: %s\n", file_name);
                coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED);
                received_file_name = true;
            } else {
                printf("Failed to create file: %s\n", file_name);
                coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
            }
        } else {
            // Logging the received file data
            printf("Received file data of size: %zu\n", size);
            printf("File data: %.*s\n", (int)size, data);

            size_t written = fwrite(data, 1, size, file);
            fflush(file);
            if (written == size) {
                printf("Data written successfully: %zu bytes\n", size);
                coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED);
            } else {
                printf("Failed to write data to file\n");
                coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
            }
        }
    } else {
        printf("No data received\n");
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
    }
}

int main() {
    coap_startup();

    ctx = coap_new_context(nullptr);
    if (!ctx) {
        fprintf(stderr, "Failed to create context\n");
        return EXIT_FAILURE;
    }

    coap_address_t serv_addr;
    coap_address_init(&serv_addr);
    serv_addr.addr.sin.sin_family = AF_INET;
    serv_addr.addr.sin.sin_port = htons(5683);
    serv_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;

    if (coap_new_endpoint(ctx, &serv_addr, COAP_PROTO_UDP) == nullptr) {
        fprintf(stderr, "Failed to create endpoint\n");
        coap_free_context(ctx);
        return EXIT_FAILURE;
    }

    resource = coap_resource_init(coap_make_str_const("file"), 0);
    if (!resource) {
        fprintf(stderr, "Failed to create resource\n");
        return EXIT_FAILURE;
    }

    coap_register_handler(resource, COAP_REQUEST_PUT, handle_put);
    coap_add_resource(ctx, resource);

    while (1) {
        coap_io_process(ctx, COAP_IO_WAIT);
    }

    if (file) {
        fclose(file);
    }

    coap_free_context(ctx);
    coap_cleanup();

    return EXIT_SUCCESS;
}
