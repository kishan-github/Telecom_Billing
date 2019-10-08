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
int start_client_app(char *ph_no)
{
	pthread_t thread_id_send;
	pthread_t thread_id_receive;
	int status = 0;

	if(register_with_server(ph_no) == FAILURE)
		return FAILURE;

	status = pthread_create(&thread_id_send, NULL, send_message, NULL);
	if(0 != status)
	{
		printf("\n%s : %d : Thread creation failed.", __func__, __LINE__);
		return FAILURE;
	}

	status = pthread_create(&thread_id_receive, NULL, receive_message, NULL);
	if(0 != status)
	{
		printf("\n%s : %d : Thread creation failed.", __func__, __LINE__);
		return FAILURE;
	}

	pthread_join(thread_id_send,NULL);
	pthread_join(thread_id_receive,NULL);
	return SUCCESS;
}

int register_with_server(char *ph_no)
{
	char buffer[MAX_LEN];

	if(validate_number(ph_no) == FAILURE)
		return FAILURE;

	strcpy(buffer, ph_no);
	WRITE(socket_fd, buffer);

	return SUCCESS;
}

int validate_number(char *ph_no)
{
	int i = 0;

	if(!(PHONE_NO_LENGTH == strlen(ph_no)))
	{
		printf("\nPhone number should be of 10 digits.");
		return FAILURE;
	}

	for(;i < PHONE_NO_LENGTH; i++)
	{
		if(!isdigit(ph_no[i]))
		{
			printf("\nPhone number cannot be alpha numeric.");
			return FAILURE;
		}
	}

	return SUCCESS;
}

void* send_message(void *arg)
{
	char buffer[MAX_LEN];

	while(1)
	{
		printf("\nEnter message : ");
		fgets(buffer, sizeof(buffer), stdin);
		WRITE(socket_fd, buffer);
	}
}

void* receive_message(void *arg)
{
	char buffer[MAX_LEN];

	while(1)
	{
		READ(socket_fd, buffer);
		printf("\nMessage received : %s", buffer);
	}
}
