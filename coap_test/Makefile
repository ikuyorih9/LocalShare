CC := g++
CCFLAGS := -std=c++11 -Wall -I/usr/local/include/coap3
LDFLAGS := -L/usr/local/lib -lcoap-3

SERVER_DIR := ./server
CLIENT_DIR := ./client

SERVER := $(SERVER_DIR)/coap_server.out
CLIENT := $(CLIENT_DIR)/coap_client.out

SERVER_SRC := $(SERVER_DIR)/coap_server.cpp
CLIENT_SRC := $(CLIENT_DIR)/coap_client.cpp

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CCFLAGS) $(SERVER_SRC) -o $(SERVER) $(LDFLAGS)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CCFLAGS) $(CLIENT_SRC) -o $(CLIENT) $(LDFLAGS)


run_client: $(CLIENT)
	cd $(CLIENT_DIR) && .$(CLIENT) ./example.txt

run_server: $(SERVER)
	cd $(SERVER_DIR) && .$(SERVER)

clean:
	rm -f $(SERVER) $(CLIENT)

.PHONY: all clean run_client run_server