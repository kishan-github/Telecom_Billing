#include <main.h>
#include <socket.h>

int socket_fd = 0;

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

int init(char *argv)
{
	// Create the socket.
	if(create_socket(argv, &socket_fd) == FAILURE)
	{
		printf("\nSocket creation failed.");
		return FAILURE;
	}

	return SUCCESS;
}

int start_client_app()
{
	char buffer[MAX_LEN];

	printf("\nEnter the message : ");
	fgets(buffer, sizeof(buffer), stdin);
	WRITE(socket_fd, buffer);

	return SUCCESS;
}