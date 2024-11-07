#include "client.h"
#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>

int create_client_socket(uint16_t port) {

	struct sockaddr_in server_info = {0};
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(port);
	server_info.sin_addr.s_addr = htonl(0x7f000001);
	
	int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(client_fd < 0) {
		perror("Failed to create client socket");
	}

	size_t server_info_len = sizeof(server_info);
	ssize_t connected = connect(client_fd, (const struct sockaddr *)&server_info, server_info_len);
  if(connected < 0) {
		perror("client failed to connect to server");
  }

  char initial_message[MAX_BUFFER_SIZE] = {0};
	ssize_t bytes_received = recv(client_fd, initial_message, sizeof(initial_message)-1, 0);
  if(bytes_received < 0) {
    perror("Initial Server Message Failed: ");
    return -1;
  }

  initial_message[bytes_received] = '\0';
  write(STDOUT_FILENO, initial_message, bytes_received+1);
  memset(initial_message, 0, bytes_received);

	return client_fd;
}

void *write_to_server_thread(void *arg) {
  unsigned int client_socket = * (unsigned int*) arg;
  char buffer[100];
  fgets(buffer, sizeof(buffer), stdin);
  if(buffer[0] != '\0' || buffer[0] != '\n') {
    printf("worthy to send:%s\n", buffer);
    send(client_socket, buffer, sizeof(buffer), 0);
  }
}

void start_client_application(unsigned int client_socket) {
	
  fcntl(client_socket, F_SETFL, O_NONBLOCK);
  while(1) {
    pthread_t thread;
    pthread_create(&thread, NULL, write_to_server_thread, &client_socket);
    pthread_join(thread, NULL);

    char buffer[100];
    recv(client_socket, buffer, sizeof(buffer), 0);
    if(buffer[0] == '\0') {
      continue;
    }

		printf("received: %s\n", buffer);
  }
}

int main() {
	int client_fd = create_client_socket(PORT);
	start_client_application(client_fd);

	return 0;
}
