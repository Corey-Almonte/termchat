#include "server.h"
#include "client.h"
#include "common.h"
// While client is connected to the chat application, let client send messages to server. Server prints them
	// 	Creating a new thread in server that observes the connected client sockets.
	//	If client socket sets state to DISCONNECTED (use enum), decrement number of clients connected on server.
	//	Create the whole observer pattern schema. As we may want multiple observers (Server, Logging)
	//	If num_clients == 0, stop while loop.

int main(int argc, char** argv) {
	int server_fd = create_server_socket(PORT);
	int client_fd = create_client_socket(PORT);
	handle_client_socket(client_fd, server_fd);
	start_client_application(client_fd);

	return 0;
}
