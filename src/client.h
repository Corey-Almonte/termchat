#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>

int create_client_socket(uint16_t port);
void start_client_application(unsigned int client_socket);





#endif // CLIENT_H
