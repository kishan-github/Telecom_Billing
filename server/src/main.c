#include <main.h>
#include <socket.h>

int main(int argc, char *argv[])
{
	int socket_fd = 0;

	if(argc != 2)
	{
		printf("\nEnter proper command line arguments.");
		printf("\n<exe> <port no>");
		return 0;
	}

	// create a socket for communication with server.
	socket_fd = create_socket(argv);
	if(socket_fd != FAILURE)
	{
		printf("\nSocket creation successful.");
	}

	close(socket_fd);

	return 0;
}
