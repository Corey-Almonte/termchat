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
#include <poll.h>
#include <stdlib.h>

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
	ssize_t connected = connect(client_fd, (const struct sockaddr *) &server_info, server_info_len);
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
  ssize_t bytes_written = write(STDOUT_FILENO, initial_message, bytes_received+1);
  if(bytes_written < 0) {
    perror("Initial write to console failed: ");
    return -1;
  }
  memset(initial_message, 0, bytes_received);

	return client_fd;
}

void start_client_application(int client_fd) {
  fcntl(client_fd, F_SETFL, O_NONBLOCK);
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
  fcntl(STDOUT_FILENO, F_SETFL, O_NONBLOCK);

  struct pollfd poll_fds[3];
  int num_fds = sizeof(poll_fds)/sizeof(poll_fds[0]);

  poll_fds[0].fd = STDIN_FILENO;
  poll_fds[0].events = POLLIN;
  poll_fds[1].fd = client_fd;
  poll_fds[1].events = POLLIN | POLLOUT;
  poll_fds[2].fd = STDOUT_FILENO;
  poll_fds[2].events = POLLOUT;

  int user_write = 0;
  int message_write = 0;
  char buffer[MAX_BUFFER_SIZE];
  int poll_wait = -1;
  while(1) {
    poll_wait = poll(poll_fds, num_fds, -1);
    if(poll_wait < 0) {
      perror("Client: Failed to wait for poll events");
      exit(EXIT_FAILURE);
    }

    if(poll_fds[0].revents & POLLIN) {
      ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
      if(bytes_read < 0) {
        perror("Client: Failed to capture user input: ");
        exit(EXIT_FAILURE);
      }
      buffer[bytes_read] = '\0';
      user_write = 1;
    }

    if(poll_fds[1].revents & POLLOUT && user_write == 1) {
      ssize_t bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
      if(bytes_sent < 0) {
        perror("Client: Failed to send bytes over network: ");
        exit(EXIT_FAILURE);
      }
      memset(buffer, 0, sizeof(buffer));
      user_write = 0;
    }

    if(poll_fds[1].revents & POLLIN) {
      ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
      if(bytes_received < 0) {
        perror("Client: Failed to receive bytes over network: ");
        exit(EXIT_FAILURE);
      }
      buffer[bytes_received] = '\0';
      message_write = 1;
    }

    if(poll_fds[2].revents & POLLOUT && message_write == 1) {
      ssize_t bytes_written = write(STDOUT_FILENO, buffer, strlen(buffer));
      if(bytes_written < 0) {
        perror("Client: Failed tp write bytes to user console");
        exit(EXIT_FAILURE);
      }
      memset(buffer, 0, sizeof(buffer));
      message_write = 0;
    }
  }
}

int main() {
	int client_fd = create_client_socket(PORT);
	start_client_application(client_fd);

	return 0;
}
