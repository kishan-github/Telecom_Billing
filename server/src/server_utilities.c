#include <socket.h>
#include <server_utilities.h>
#include <init.h>
#include <database.h>

extern int socket_fd;

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

	//Get client phone number.
	READ(new_sockfd, buffer);

	// Add number in the database.
	add_number_in_database(new_sockfd, buffer);

	while(1)
	{
		READ(new_sockfd, buffer);
		PRINT("Message received : %s", buffer);
	}

	pthread_exit(NULL);
}
