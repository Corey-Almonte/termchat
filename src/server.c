#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>

#define MAX_EVENTS 2

// TODO: 
// Fill all of serverside with server instance (server.h, server.c, main.c),
// server will be declared in main.c, passed as a refference to server functions in main.c
// struct declared in server.h
// server server = {0} <- write in main.c
// Once connection functionality has been restored, finish io_message_thread stuff
// Remember to manage state in io thread. All else if part of main thread should do is create thread.
void create_server_socket(uint16_t port, Server server) {
	struct sockaddr_in server_info = {0}; 
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(port);

	socklen_t server_info_len = sizeof(server_info); 
	const int option = 1;
	int optval;
	socklen_t optlen = sizeof(optval);
	
  int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(server_fd < 0) {
		perror("server socket failed to be created ");
		return -1;
	}
	
  if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,  &option, sizeof(option)) < 0) {
    perror("Failed to set socket opetion SO_REUSEADDR");
    return -1;
  }

  if(bind(server_fd, (const struct sockaddr *)&server_info, server_info_len) < 0) {
		perror("Server socket failed to bind");
		close(server_fd);
		return -1;
	}
	
	if(getsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,  &optval, &optlen) < 0) {
		perror("getsockopt error: REUSEADDR");
		close(server_fd);
		exit(1);
	}

	if(listen(server_fd, 0) < 0) {
		perror("Server socket failed to listen");
		close(server_fd);
		return -1;
	}
  
  server.server_fd = server_fd;
  printf("Starting server...\n");
}

void read_client_message(const int client_fd, char *message, const int message_len) {
  if(client_fd < 0) {
    perror("read_client_message given bad fd: %d", client_fd);
  }

  int bytes received = recv(client_fd, message, message_len, 0);
  if(bytes_received < 0) {
    perror("read_client_message failed to receive message");
  }

  message[bytes_received] = '\0'
  printf("received from (%d): %s\n", client_fd, message);
}

void broadcast_client_message(const int sender_fd, const int *client_fds, const int num_clients, 
                         const char *message, const int message_len) {
  for(int i = 0; i < num_clients; i++) {
    if(sender_fd == client_fds[i]) {
      continue;
    }
    
    int bytes_sent = send(client_fds[i], (void *) message, size_t message_len, 0);
    if(bytes_sent < 0) {
      perror("send_client_message failed to send message");
    }

    memset(message, 0, message_len);
    printf("sent to (%d): %s\n", client_fds[i], message);
  }
}


void *io_message_thread(void *arg) {
  
}

void handle_client_sockets(Server server) {
	struct sockaddr_in client_info = {0};
	socklen_t client_info_len = sizeof(client_info);
// TODO:: concurrent hashmap instead of array for storing client connections.
	[MAX_CLIENT_CONNECTIONS] = {0};

  struct epoll_event event;
  struct epoll_event events[MAX_EVENTS];
  server.epoll_fd = epoll_create1(0);

  event.data.fd = server.server_fd;
  event.events = EPOLLIN;
	if(epoll_ctl(server.epoll_fd, EPOLL_CTL_ADD, server.server_fd, &event) == -1) {
			perror("epoll_ctl:");
			close(server.epoll_fd);
      exit(EXIT_FAILURE);
	}

  int num_fds = 0;
  int client_count = 0;
	while(1) {
    num_fds = epoll_wait(server.epoll_fd, events, MAX_EVENTS, -1);
		if(num_fds < 0) {
			perror("Failed epoll_wait: ");
			exit(EXIT_FAILURE);
    }

    for(int i = 0; i < num_fds; i++) {
		  if(events[i].data.fd == server.server_fd && events[i].events & EPOLLIN) {
        client_fds[i] = accept(server.server_fd, (struct sockaddr *) &client_info, &client_info_len); 
		    if(client_fds[i] < 0) {
			    perror("Server rejects client connection");
			    exit(EXIT_FAILURE);
		    }

		    fcntl(client_fds[i], F_SETFL, O_NONBLOCK);
        event.data.fd = client_fds[i];
		    event.events = EPOLLIN | EPOLLOUT; // Add EPOLLET at some point
		    if(epoll_ctl(server.epoll_fd, EPOLL_CTL_ADD, client_fds[i], &event) == -1) {
			    perror("epoll_ctl:");
			    close(epoll_fd);
          exit(EXIT_FAILURE);
		    }
        
        printf("client fd: %d connected to server\n", client_fd);
        client_count+=1;

		    char buffer[] = "You are now connected to the server.\n"; 
		    if(send(server.client_fds[i], (void *) buffer, strlen(buffer), 0) < 0) {
			    perror("Message to server failed to send");
		      close(client_fds[i]);
          exit(EXIT_FAILURE);
        }
        memset(buffer, 0, sizeof(buffer));
		  } else if(events[i].events & EPOLLIN){
        // in future, this section deserializes protocol 
        // and detects type of task to do: database, encryption, etc..
        int client_fd = events[i].data.fd;

        // WARN: Without hashmap, no way to quickly get current fd to discern state of a client
        int client_idx = 0;
        while(client_index < client_socket_count) {
          if(server.clients[client_idx] == events[i].data.fd) {
            server.clients[client_idx] = events[i].data.fd;
            break;
          }
          client_idx+=1;
        }
        
        // TODO: Create a thread function, that depending on state of buffer, read or write buffer
        pthread_t message_thread;
        if(pthread_create(&message_thread, NULL, io_message_thread,))
      }
    }
  }
}


