#include <main.h>
#include <client_utilities.h>

// Variable to store the socket id of the link.
extern int socket_fd;

// Mutex and condition variables.
extern pthread_mutex_t call_connected_mutex;
extern pthread_mutex_t called_number_status_mutex;
extern pthread_cond_t call_connected_cond;
extern pthread_cond_t called_number_status_cond;

// Main function.
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

	// Start the client app.
	if(start_client_app(argv[2]) == FAILURE)
	{
		printf("\nSomething went wrong. Closing the application.");
		close(socket_fd);
		return 0;
	}

	// Close the socket before ending.
	pthread_mutex_destroy(&call_connected_mutex);
	pthread_mutex_destroy(&called_number_status_mutex);
	pthread_cond_destroy(&call_connected_cond);
	pthread_cond_destroy(&called_number_status_cond);
	close(socket_fd);

	return 0;
}
