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
#include <sys/epoll.h>
#include <fcntl.h>

#define MAX_EVENTS 2

/**
	server socket created, bound to port. Listens for connections, client socket connects in client.c, 
	server accepts and creates a socket with client info in server.c
	server sends client a response to their connection hello
	client receives and proccesses it
**/


int create_server_socket(uint16_t port) {
	struct sockaddr_in server_info = {0}; 
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(port);

	socklen_t server_info_len = sizeof(server_info); 

	const int option = 1;
	int optval;
	socklen_t optlen = sizeof(optval);
	int server_fd = 0;

	if((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("server socket failed to be created ");
		close(server_fd);
		return -1;
	}
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,  &option, sizeof(option));
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
	printf("SO_REUSEADDR is %s\n", (optval ? "enabled" : "disabled"));
	
	if(getsockopt(server_fd, SOL_SOCKET, SO_LINGER,  &optval, &optlen) < 0) {
		perror("getsockopt error: SO_LINGER");
		close(server_fd);
		exit(1);
	}
	printf("SO_LINGER is %s\n", (optval ? "enabled" : "disabled"));

	if(listen(server_fd, 0) < 0) {
		perror("Server socket failed to listen");
		close(server_fd);
		return -1;
	}
	return server_fd;
}

char *receive_data(int client_socket) {
	static char buffer[100];
	memset(buffer, 0, sizeof(buffer));
	int num_bytes_read = 0;	
	if(num_bytes_read = recv(client_socket, buffer, sizeof(buffer), 0) == 0) {
		perror("failed to read message");
		exit(1);
	}
	return buffer;	
}

void send_data(int client_socket, char *buffer) {
	if(send(client_socket, buffer, strlen(buffer), 0) < 0) {
		perror("Message to server failed to send");
		exit(1);
	}
}

void handle_client_sockets(unsigned int server_socket) {
	struct sockaddr_in client_info = {0};
	socklen_t client_info_len = sizeof(client_info);
	
	int client_sockets[2] = {0};
	int client_socket_index = 0;
	while(1) {
		client_sockets[client_socket_index] = accept(server_socket, 
			(struct sockaddr *) &client_info, &client_info_len); 
		
		if(client_sockets[client_socket_index] < 0) {
			perror("Server rejects client connection");
			exit(1);
		}

		// Non blocking I/O
		fcntl(client_sockets[client_socket_index], F_SETFL, O_NONBLOCK);

		char *buffer = "You are now connected to the server.\n"; 
		if(send(client_sockets[client_socket_index], (void *) buffer, strlen(buffer), 0) < 0) {
			perror("Message to server failed to send");
			exit(1);
		}

		client_socket_index++;	
		printf("client %d of 2\n", client_socket_index);
		if(client_socket_index > 1) {
			break;	
		}
	}
	int client_socket_count = client_socket_index;
	_process_clients(client_sockets, client_socket_count);
}
	
void _process_clients(int *client_sockets, int client_socket_count) {
	
	struct epoll_event event;
	struct epoll_event events[MAX_EVENTS];
	int epoll_fd = epoll_create1(0);

	for(int i = 0; i < 2; i++) {
		event.data.fd = client_sockets[i];
		event.events = EPOLLIN | EPOLLOUT;
		if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockets[i], &event) == -1) {
			perror("epoll_ctl:");
			exit(1);
		}
		printf("client%d file descripter: %d\n", i, event.data.fd);
		printf("Is it a correct file descriptor? %s\n\n", (fcntl(event.data.fd, F_GETFL) != -1)  ? "yes" : "no");
	}

	char buffer[100];
	while(1) {
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if(num_events < 0) {
			perror("Failed epoll_wait: ");
			exit(1);
		}
		//printf("Number of triggered events: %d\n", num_events);
		for(int i = 0; i < num_events; i++) {
			/** printf("epoll_wait loop: client socket %d, %s\n", event.data.fd, 
				events[i].events & EPOLLERR? "error" : "no errors");
			printf("epoll_wait loop: client socket %d, %s socket\n", event.data.fd, 
				events[i].events&EPOLLIN? "readable":"unreadable");
			printf("epoll_wait loop: client socket %d, %s socket\n\n", event.data.fd, 
				events[i].events&EPOLLOUT? "writable":"unwrtiable");
			**/
			if(events[i].events & EPOLLIN) {
				//printf("Client socket %d entered EPOLLIN if statement \n", events[i].data.fd);
				
				//buffer = receive_data(events[i].data.fd);
				//printf("buffer in epoll loop says: %s\n", buffer);
				//printf("%s\n", buffer);
				if(buffer[0] == '\0') {

					strcpy(buffer, receive_data(events[i].data.fd));
				}
			}
			if(events[i].events & EPOLLOUT) {
				//printf("Client socket %d entered EPOLLOUT if statement \n", events[i].data.fd);
				if(buffer[0] == '\0') {
					continue;
				}
				(events[i].data.fd == 4) ? send_data(5, buffer) : send_data(4, buffer);
				memset(buffer, 0, sizeof(buffer));
			}
		}
	}
		close(epoll_fd);
}
