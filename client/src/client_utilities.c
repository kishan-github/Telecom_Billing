#include <client_utilities.h>
#include <socket.h>

// Variable to store the socket id of the link.
int socket_fd = 0;

// Initialize the objects.
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

// Start the client app.
int start_client_app()
{
	char buffer[MAX_LEN];

	printf("\nEnter the message : ");
	fgets(buffer, sizeof(buffer), stdin);
	WRITE(socket_fd, buffer);

	return SUCCESS;
}
