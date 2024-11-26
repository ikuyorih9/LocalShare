#ifndef FRTOS_HPP
#define FRTOS_HPP

#define SSID "uguinho"
#define PASSWORD "bananamelao123"
#define IP "192.168.137.1"
#define PORTA 1234

uint byte4_to_int(uint8_t * byte);
void Tarefa1(void *pvParameters);
void Tarefa2(void *pvParameters);
#endif