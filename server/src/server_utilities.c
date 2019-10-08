#include <socket.h>
#include <server_utilities.h>

// Variable to store the socket id of the link.
int socket_fd = 0;
MYSQL *mysql = NULL;

// Method to initialize the objects.
int init(char *argv)
{
	// create a socket for communication with client.
        if(create_socket(argv, &socket_fd) == FAILURE)
        {
                PRINT("Socket creation failed.");
                return FAILURE;
        }
        else
        	PRINT("Socket created successfully.");

        // initialize the database.
        if(init_database() == FAILURE)
        {
        	PRINT("Database initialization failed.");
        	return FAILURE;
        }

	return SUCCESS;
}

// Create the database and initialize it.
int init_database()
{
	// Init sql.
	mysql = mysql_init(NULL);

	/*connect with the database*/
	if (!mysql_real_connect(mysql, SERVER, USER, PASSWORD, DATABASE, 0, NULL, 0))
	{
		fprintf(stderr, "%s\n", mysql_error(mysql));
		mysql_close(mysql);
		return FAILURE;
	}
	else
		PRINT("Successfully connected to database.");

	return SUCCESS;
}

// Start the server.
int start_server()
{
	int index = 0;
	int status = 0;
	int connection_fd[MAX_CLIENT];
	struct 	sockaddr_in client_addr;
	socklen_t address_length;
	pthread_t tid[MAX_CLIENT];

	// Keep running the server.
	while(1)
	{
		PRINT("Waiting for client connection...\n");

		for (index = 0; index < MAX_CLIENT; index++)
		{
			// Accept the request of client that wants to connect to server.
			connection_fd[index] = accept(socket_fd, (struct sockaddr*)&client_addr, &address_length);
			if(connection_fd[index] < 0)
			{
				PRINT("Accept failed.");
				return FAILURE;
			}

			/******* Creating thread for client **********/
			status = pthread_create(&tid[index], NULL, subroutine, (void *)&connection_fd[index]);
			if (0 != status)
			{
				PRINT("thread creation failed");
				close(connection_fd[index]);
				return FAILURE;
			}
		}

		for (index = 0; index < MAX_CLIENT; index++)
		{
			// Wait for each client to complete.
			status = pthread_join(tid[index], (void**)NULL);
			if (0 != status)
			{
				PRINT("Thread join failed");
				close(connection_fd[index]);
				return FAILURE;
			}
			close(connection_fd[index]);
		}
	}
}

// Function to be called after thread creation when client is connected.
void *subroutine(void * connfd)
{
	int new_sockfd = 0;
	char buffer[MAX_LEN];

	PRINT("client connected with server");

	new_sockfd = connfd ? *(int *)connfd : 0;

	while(1)
	{
		READ(new_sockfd, buffer);
		PRINT("Message received : %s", buffer);
	}

	pthread_exit(NULL);
}
