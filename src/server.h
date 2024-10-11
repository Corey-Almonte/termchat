#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

unsigned int create_server_socket(uint16_t port);
int handle_client_socket(unsigned int client_socket, unsigned int server_socket);

#endif //SERVER_H
