#include <socket.h>
#include <server_utilities.h>

int socket_fd = 0;

int init(char *argv)
{
	// create a socket for communication with client.
        if(create_socket(argv, &socket_fd) == FAILURE)
        {
                printf("\nSocket creation failed.");
                return FAILURE;
        }

	return SUCCESS;
}

int start_server()
{
	int index = 0;
	int status = 0;
	int connection_fd[MAX_CLIENT];
	struct 	sockaddr_in client_addr;
	socklen_t address_length;
	pthread_t tid[MAX_CLIENT];

	while(1)
	{
		printf("\nWaiting for client connection...\n");

		for (index = 0; index < MAX_CLIENT; index++)
		{
			connection_fd[index] = accept(socket_fd, (struct sockaddr*)&client_addr, &address_length);
			if(connection_fd[index] < 0)
			{
				printf("\nAccept failed.");
				return FAILURE;
			}

			/******* Creating thread for client **********/
			status = pthread_create(&tid[index], NULL, subroutine, (void *)&connection_fd[index]);
			if (0 != status)
			{
				printf("thread creation failed\n");
				close(connection_fd[index]);
				return FAILURE;
			}
		}

		for (index = 0; index < MAX_CLIENT; index++)
		{
			status = pthread_join(tid[index], (void**)NULL);
			if (0 != status)
			{
				printf("Thread join failed\n");
				close(connection_fd[index]);
				return FAILURE;
			}
			close(connection_fd[index]);
		}
	}
}

void *subroutine(void * connfd)
{
	int new_sockfd = 0;
	char buffer[MAX_LEN];

	printf("client connected with server\n");

	new_sockfd = connfd ? *(int *)connfd : 0;

	READ(new_sockfd, buffer);
	printf("\nMessage received : %s", buffer);

	pthread_exit(NULL);
}
