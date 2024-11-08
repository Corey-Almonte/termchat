#include "server.h"
#include "common.h"
#include <stdio.h>
// While client is connected to the chat application, let client send messages to server. Server prints them
	// 	Creating a new thread in server that observes the connected client sockets.
	//	If client socket sets state to DISCONNECTED (use enum), decrement number of clients connected on server.
	//	Create the whole observer pattern schema. As we may want multiple observers (Server, Logging)
	//	If num_clients == 0, stop while loop.

int main(void) {
	int server_fd = create_server_socket(PORT);
	handle_client_sockets(server_fd);

	return 0;
}
