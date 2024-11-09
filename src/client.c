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
#include <sys/epoll.h>
#include <stdlib.h>

#define MAX_EVENTS 3

int create_client_socket(uint16_t port) {

	struct sockaddr_in server_info = {0};
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(port);
	server_info.sin_addr.s_addr = htonl(0x7f000001);
	
	int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(client_fd < 0) {
		perror("Failed to create client socket");
	  return -1;
  }

	size_t server_info_len = sizeof(server_info);
  if(connect(client_fd, (const struct sockaddr *) &server_info, server_info_len) < 0) {
    perror("client failed to connect to server");
		close(client_fd);
    return -1;
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
// TODO: Change buffer flags to use io state machine 
void start_client_application(int client_fd) {
  fcntl(client_fd, F_SETFL, O_NONBLOCK);
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
  fcntl(STDOUT_FILENO, F_SETFL, O_NONBLOCK);

  struct epoll_event event;
  struct epoll_event events[MAX_EVENTS];
  int epoll_fd = epoll_create1(0);
  if(epoll_fd < 0) {
    perror("Could not create epoll instance");
    exit(EXIT_FAILURE);
  }

  int user_write = 0;
  int message_write = 0;
  char buffer[MAX_BUFFER_SIZE];

  event.events = EPOLLIN;
  event.data.fd = STDIN_FILENO;
  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event) == -1) {
        perror("epoll_ctl failed for STDIN_FILENO");
        close(epoll_fd);
        exit(EXIT_FAILURE);
  }

  event.events = EPOLLIN | EPOLLOUT;
  event.data.fd = client_fd;
  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
        perror("epoll_ctl failed for client_fd");
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    event.events = EPOLLOUT;
    event.data.fd = STDOUT_FILENO;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDOUT_FILENO, &event) == -1) {
        perror("epoll_ctl failed for STDOUT_FILENO");
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

  while(1) {
    int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if(num_fds < 0) {
      perror("Client: Failed to wait for epoll events");
      close(epoll_fd);
      exit(EXIT_FAILURE);
    }

    for(int i = 0; i < num_fds; i++) {
      int fd = events[i].data.fd;

      if(fd == STDIN_FILENO && events[i].events & EPOLLIN) {
        // WARN: sizeof buffer for read calls in terminals will be when newline character is entered
        ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if(bytes_read < 0) {
          perror("Client: Failed to capture user input: ");
          close(epoll_fd);
          exit(EXIT_FAILURE);
        }
        buffer[bytes_read] = '\0';
        user_write = 1;
      }

      if(fd == client_fd && events[i].events & EPOLLOUT && user_write == 1) {
        ssize_t bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
        if(bytes_sent < 0) {
          perror("Client: Failed to send bytes over network: ");
          close(epoll_fd);
          exit(EXIT_FAILURE);
        }
        memset(buffer, 0, sizeof(buffer));
        user_write = 0;
      }

      if(fd == client_fd && events[i].events & EPOLLIN) {
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if(bytes_received < 0) {
          perror("Client: Failed to receive bytes over network: ");
          close(epoll_fd);
          exit(EXIT_FAILURE);
        }
        buffer[bytes_received] = '\0';
        message_write = 1;
      }

      if(fd == STDOUT_FILENO && events[i].events & EPOLLOUT && message_write == 1) {
        ssize_t bytes_written = write(STDOUT_FILENO, buffer, strlen(buffer));
        if(bytes_written < 0) {
          perror("Client: Failed tp write bytes to user console");
          close(epoll_fd);
          exit(EXIT_FAILURE);
        }
        memset(buffer, 0, sizeof(buffer));
        message_write = 0;
      }
    }
  }
}

int main() {
	int client_fd = create_client_socket(PORT);
	start_client_application(client_fd);

	return 0;
}
