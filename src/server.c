
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

	const int option = 2;
	int server_fd = 0;

	if((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("server socket failed to be created ");
		close(server_fd);
		return -1;
	}
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_LINGER,  &option, sizeof(option));
	if(bind(server_fd, (const struct sockaddr *)&server_info, server_info_len) < 0) {
		perror("Server socket failed to bind");
		close(server_fd);
		return -1;
	}

	if(listen(server_fd, 0) < 0) {
		perror("Server socket failed to listen");
		close(server_fd);
		return -1;
	}
	return server_fd;
}

void handle_client_socket(unsigned int server_socket) {
	struct sockaddr_in client_info = {0};
	socklen_t client_info_len = sizeof(client_info);

	while(1) {
	int client_socket = accept(server_socket,(struct sockaddr *) &client_info, &client_info_len); 
	
	if(client_socket < 0) {
		perror("Server rejects client connection");
		exit(1);
	}

	char *buffer = "You are now connected to the server.\n"; 
	if(send(client_socket, (void *) buffer, strlen(buffer), 0) < 0) {
		perror("Message to server failed to send");
		exit(1);
	}

	pthread_t client_thread;
	pthread_create(&client_thread, NULL,  _process_client, (void *)&client_socket);
}
}
	
void* _process_client(void *client_socket) {
	int *client_socket_p = (int*) (client_socket);

	int num_bytes_read = 0;
	while(1) {
		char new_buffer[100];
		if(num_bytes_read = recv(*client_socket_p, new_buffer, sizeof(new_buffer), 0) == 0) {
			perror("failed to read message");
			exit(1);
		}
		printf("%s\n", new_buffer);
	}
}
