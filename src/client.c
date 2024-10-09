#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> 
int main(int argc, char** argv) {
	
	struct sockaddr_in server_info = {0};
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(1337);
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
	
	// sending messages to server
	while(1)  {
		fgets(buffer, sizeof(buffer), stdin);
		send(client_fd, buffer, sizeof(buffer), 0);
	}
	close(client_fd);
	return 0;
}
