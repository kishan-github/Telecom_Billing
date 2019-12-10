#include <socket.h>
#include <server_utilities.h>
#include <init.h>

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
	int caller_user_id = 0;

	PRINT("Client connected with server");

	new_sockfd = connfd ? *(int *)connfd : 0;

	//Get client phone number.
	READ(new_sockfd, buffer);

	// Add number in the database.
	if(add_number_in_database(new_sockfd, buffer) != SUCCESS)
	{
		PRINT("Failed in add_number_in_database.");
	}

	// Get caller's user id from database.
	if(get_user_id(buffer, &caller_user_id) != SUCCESS)
	{
		PRINT("Failed in get_user_id.");
	}

	// Wait for user input.
	if(get_user_input(new_sockfd, caller_user_id) != SUCCESS)
	{
		PRINT("Failed in get_user_input.");
	}

	pthread_exit(NULL);
}

int get_user_input(int socket_fd, int caller_user_id)
{
	char buffer[MAX_LEN];
	user_input_t input = UNKNOWN_REQUEST;

	while(1)
	{
		// Get the user request.
		READ(socket_fd, buffer);
		input = atoi(buffer);

		switch(input)
		{
			case SETUP_CALL:
			{
				// Setup the call.
				if(create_call(socket_fd, caller_user_id) != SUCCESS)
				{
					PRINT("Failed in create_call");
					set_user_status(caller_user_id, USER_AVAILABLE);
					return FAILURE;
				}
				break;
			}
			case SWITCH_OFF:
			{
				disconnect_user(socket_fd, caller_user_id);
				return SUCCESS;
				break;
			}
			case ACCEPT_CALL:
			{
				printf("\nUser accepted the call.");
				while(1);
			}
			default:
				break;
		}
	}

	return SUCCESS;
}

int create_call(int socket_fd, int caller_user_id)
{
	user_status_t status = USER_UNKNOWN;
	caller_status_t c_status = CALLER_UNKNOWN;
	int receiver_user_id = 0;
	char buffer[MAX_LEN];
	char calling_number[MAX_LEN];
	int receiver_connfd = 0;

	// Set user status to busy
	set_user_status(caller_user_id, USER_BUSY);

	// Get the number user wants to call.
	READ(socket_fd, calling_number);

	// Send the status to caller about the calling number.
	//sprintf(buffer, "%d", RECEIVE_STATUS);
	//WRITE(socket_fd, buffer);

	// Get the user id of the calling number.
	if(get_user_id(calling_number, &receiver_user_id) != SUCCESS)
	{
		PRINT("Calling number is not available in database.");
		sprintf(buffer, "%d", CALLER_NOT_REGISTERED);
		WRITE(socket_fd, buffer);
		return FAILURE;
	}

	// Get status of the calling number.
	if(get_user_status(receiver_user_id, &status) != SUCCESS)
	{
		PRINT("Failed in get_user_status.");
		sprintf(buffer, "%d", CALLER_UNKNOWN);
		WRITE(socket_fd, buffer);
		return FAILURE;
	}

	c_status = map_user_status_to_caller(status);
	sprintf(buffer, "%d", c_status);
	WRITE(socket_fd, buffer);

	//Check if receiver is available to take call.
	if(status == USER_AVAILABLE)
	{
		// Set receiver status to busy.
		set_user_status(receiver_user_id, USER_BUSY);

		// Get connection fd of receiver to send calling request.
		get_user_connection_fd(receiver_user_id, &receiver_connfd);

		// Send a ring to the user.
		sprintf(buffer, "%d", RECEIVE_CALL);
		WRITE(receiver_connfd, buffer);

		// Get user number.
		get_user_number(caller_user_id, buffer);

		WRITE(receiver_connfd, buffer);

		// Create thread for sending and receiving messages.
		create_sender_receiver_threads(socket_fd, receiver_connfd);
	}
	else
	{
		//TO-DO
		set_user_status(caller_user_id, USER_AVAILABLE);
	}

	return SUCCESS;
}

void get_status_string(user_status_t status, char *buffer)
{
	switch(status)
	{
		case USER_AVAILABLE:
			strcpy(buffer, FREE);
			break;
		case USER_BUSY:
			strcpy(buffer, BUSY);
			break;
		case USER_SWITCHED_OFF:
			strcpy(buffer, OFF);
			break;
		case USER_UNKNOWN:
			strcpy(buffer, WRONG_NUMBER);
			break;
		default:
			PRINT("Wrong status.");
	}
}

// Create threads for sending and receiving data.
int create_sender_receiver_threads(int sender_connfd, int receiver_connfd)
{
	pthread_t thread_id_send;
	pthread_t thread_id_receive;
	int status = 0;
	connection_t connfd;

	memset(&connfd, 0, sizeof(connfd));
	connfd.sender_connfd = sender_connfd;
	connfd.receiver_connfd = receiver_connfd;

	status = pthread_create(&thread_id_send, NULL, send_message, (void*)&connfd);
	if(0 != status)
	{
		printf("\n%s : %d : Thread creation failed.", __func__, __LINE__);
		return FAILURE;
	}

	status = pthread_create(&thread_id_receive, NULL, receive_message, (void*)&connfd);
	if(0 != status)
	{
		printf("\n%s : %d : Thread creation failed.", __func__, __LINE__);
		return FAILURE;
	}

	pthread_join(thread_id_send,NULL);
	pthread_join(thread_id_receive,NULL);
	return SUCCESS;
}

void* send_message(void *connfd)
{
	char buffer[MAX_LEN];
	connection_t connection_fd;
	int sender_connfd = 0;
	int receiver_connfd = 0;

	connection_fd = *(connection_t *)connfd;
	sender_connfd = connection_fd.sender_connfd;
	receiver_connfd = connection_fd.receiver_connfd;

	while(1)
	{
		READ(sender_connfd, buffer);
		if(!strcmp(buffer, EXIT))
		{
			sprintf(buffer, "%d", DISCONNECT_CALL);
			WRITE(receiver_connfd, buffer);
			break;
		}
		WRITE(receiver_connfd, buffer);
	}

	pthread_exit(NULL);
}

void* receive_message(void *connfd)
{
	char buffer[MAX_LEN];
	connection_t connection_fd;
	int sender_connfd = 0;
	int receiver_connfd = 0;

	connection_fd = *(connection_t *)connfd;
	sender_connfd = connection_fd.sender_connfd;
	receiver_connfd = connection_fd.receiver_connfd;

	while(1)
	{
		READ(receiver_connfd, buffer);
		if(!strcmp(buffer, EXIT))
		{
			sprintf(buffer, "%d", DISCONNECT_CALL);
			WRITE(sender_connfd, buffer);
			break;
		}
		WRITE(sender_connfd, buffer);
	}

	pthread_exit(NULL);
}

caller_status_t map_user_status_to_caller(user_status_t status)
{
	switch(status)
	{
		case USER_AVAILABLE : return CALLER_AVAILABLE;
		case USER_BUSY : return CALLER_BUSY;
		case USER_SWITCHED_OFF : return CALLER_SWITCHED_OFF;
		case USER_UNKNOWN : return CALLER_NOT_REGISTERED;
		default : return CALLER_UNKNOWN;
	}
}

int disconnect_user(int socket_fd, int caller_user_id)
{
	if(set_user_status(caller_user_id, USER_SWITCHED_OFF) != SUCCESS)
		return FAILURE;

	return SUCCESS;
}
