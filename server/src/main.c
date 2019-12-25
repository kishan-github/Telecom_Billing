#include <main.h>
#include <init.h>
#include <server_utilities.h>
#include <database.h>

// Variable to store the socket id used to create link.
extern int socket_fd;
extern MYSQL *mysql;

// Main function.
int main(int argc, char *argv[])
{
	printf("\n/************** Server Window *************/");

	// Add a signal handler.
	signal(SIGINT, handle_sigint);

	if(argc != 2)
	{
		printf("\nEnter proper command line arguments.");
		printf("\n<exe> <port no>");
		return 0;
	}

	// initialize the required objects.
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

	deinit_database();

        // Deinit mutex and condition variables.
	if(deinit_mutex_cond_variables() == FAILURE)
	{
		PRINT("Mutex/Condition deinit failed.");
		return 0;
	}

	// Close the socket before finishing.
	close(socket_fd);
	mysql_close(mysql);

	return 0;
}

void handle_sigint(int sig)
{
    printf("Caught signal %d\n", sig);

    deinit_database();

    // Deinit mutex and condition variables.
    if(deinit_mutex_cond_variables() == FAILURE)
    {
	    PRINT("Mutex/Condition deinit failed.");
    }

    // Close the socket before finishing.
    close(socket_fd);
    mysql_close(mysql);
}
