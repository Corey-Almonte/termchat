#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

int create_server_socket(uint16_t port);
void handle_client_sockets(unsigned int server_socket);
void *_process_client(void *client_socket); 






#endif //SERVER_H
