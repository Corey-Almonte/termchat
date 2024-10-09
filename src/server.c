#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
/**
	server socket created, bound to port. Listens for connections, client socket connects in client.c, 
	server accepts and creates a socket with client info in server.c
	server sends client a response to their connection hello
	client receives and proccesses it
**/


int main(int argc, char** argv) {

	struct sockaddr_in server_info = {0}; 
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(1337);

	socklen_t server_info_len = sizeof(server_info); 


	int server_fd = 0;
	if((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("server socket failed to be created ");
		return -1;
	}
	const int option = 2;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_LINGER,  &option, sizeof(option));
	if(bind(server_fd, (const struct sockaddr *)&server_info, server_info_len) < 0) {
		perror("Server socket failed to bind");
		return -1;
	}
	if(listen(server_fd, 0) < 0) {
		perror("Server socket failed to listen");
		return -1;
	}

	struct sockaddr_in client_info = {0};
	socklen_t client_info_len = sizeof(client_info);

	int client_fd = accept(server_fd,(struct sockaddr *) &client_info, &client_info_len); 
	if(client_fd < 0) {
		perror("Server rejects client connection");
		return -1;
	}

	char *buffer = "You are now connected to the server.\n"; 
	if(send(client_fd, (void *) buffer, strlen(buffer), 0) < 0) {
		perror("Message to server failed to send");
		return -1;
	}

	// While client is connected to the chat application, let client send messages to server. Server prints them
	// 	Creating a new thread in server that observes the connected client sockets.
	//	If client socket sets state to DISCONNECTED (use enum), decrement number of clients connected on server.
	//	Create the whole observer pattern schema. As we may want multiple observers (Server, Logging)
	//	If num_clients == 0, stop while loop.
	int num_bytes_read = 0;
	while(1) {
		char new_buffer[100];
		if(num_bytes_read = recv(client_fd, new_buffer, sizeof(new_buffer), 0) == 0) {
			perror("failed to read message");
			return -1;
		}
	
		printf("%s", new_buffer);
	}
	close(server_fd);
	close(client_fd);
}

   
























