#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

int create_server_socket(uint16_t port);
void handle_client_socket(unsigned int server_socket);

#endif //SERVER_H
