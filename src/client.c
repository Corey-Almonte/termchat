#include "client.h"
#include "common.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> 

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
	if(connect(client_fd, (const struct sockaddr *)&server_info, server_info_len) > 0) {
		perror("client failed to connect to server");
	}
	// printing the server message
	char buffer[100];
	recv(client_fd, buffer, sizeof(buffer), 0);
	printf("%s", buffer);

	return client_fd;
}

void start_client_application(unsigned int client_socket) {
	// sending messages to server
	char buffer[100];
	while(1)  {
		fgets(buffer, sizeof(buffer), stdin);
		send(client_socket, buffer, sizeof(buffer), 0);
	}
}

int main() {
	int client_fd = create_client_socket(PORT);
	start_client_application(client_fd);

	return 0;
}
