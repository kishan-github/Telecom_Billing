#include <main.h>
#include <server_utilities.h>

extern int socket_fd;

int main(int argc, char *argv[])
{
	printf("\n/************** Server Window *************/");

	if(argc != 2)
	{
		printf("\nEnter proper command line arguments.");
		printf("\n<exe> <port no>");
		return 0;
	}

	// initialize the required things.
	if(init(argv[1]) == FAILURE)
	{
		printf("\nInitialization of objects failed.");
		close(socket_fd);
		return 0;
	}

	// Start the server.
	if(start_server() == FAILURE)
	{
		close(socket_fd);
		return 0;
	}

	close(socket_fd);

	return 0;
}
