#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>

typedef struct {
  const int client_fd;
  MESSAGE_STATE message_state;
  // TODO: At some point, this buffer will copy the deseriaized message data from a binary protocol
  // this buffer will be used to do further processing (validation, decoding, buffering, etc..)
  char message[MAX_MESSAGE_SIZE];
  // TODO:For now assume MAX_MESSAGE_SIZE, but protocol will inititialize this later
  size_t message_len = MAX_MESSAGE_SIZE;
} Client;

int create_client_socket(uint16_t port);
void start_client_application(int client_socket);





#endif // CLIENT_H
