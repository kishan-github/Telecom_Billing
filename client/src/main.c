#include <socket.h>

int main(int argc, char *argv[])
{
	int socket_fd = 0;

	if(argc != 3)
	{
		printf("\nEnter proper command line arguments.");
		printf("\n<exe> <port no> <mobile no>");
		return 0;
	}

	socket_fd = create_socket(argv);
	if(socket_fd != FAILURE)
	{
		printf("\nSocket created.");
	}

	close(socket_fd);

	return 0;
}
