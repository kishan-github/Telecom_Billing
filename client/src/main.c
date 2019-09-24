#include <main.h>
#include <client_utilities.h>

extern int socket_fd;

int main(int argc, char *argv[])
{
	printf("\n /************** Client Window ***************/");

	if(argc != 3)
	{
		printf("\nEnter proper command line arguments.");
		printf("\n<exe> <port no> <mobile no>");
		return 0;
	}

	// Initialize the objects.
	if(init(argv[1]) == FAILURE)
	{
		printf("\nInitialization failed.");
		close(socket_fd);
		return 0;
	}

	start_client_app();

	close(socket_fd);

	return 0;
}
