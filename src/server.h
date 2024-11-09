#ifndef SERVER_H
#define SERVER_H

#define MAX_CLIENT_CONNECTIONS 5

#include <stdint.h>
#include <sys/epoll.h>



typedef struct {
  // TODO: Create a server state enum (startings, running, maintenance, limited, shutting down, error)
  int server_fd;
  int epoll_fd;
  // TODO: Change to a handrolled concurrent hashmap
  Client clients[MAX_CLIENT_CONNECTIONS];
} Server;

void create_server_socket(uint16_t port, Server server);
void handle_client_sockets(int server_fd);
void process_clients(int *client_sockets, int client_socket_count); 






#endif //SERVER_H
