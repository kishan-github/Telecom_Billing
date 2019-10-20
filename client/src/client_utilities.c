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

	status = pthread_create(&thread_id_receive, NULL, receive_call, NULL);
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

int select_option()
{
	int option = 0;

	printf("\nSelect from the options given below:-");
	printf("\n1 : To make a call");
	printf("\n0 : Exit");

	while(scanf("%d", &option) != 1)
	{
		printf("\nNon numeric input is not allowed.");
		while(getchar() != '\n');
	}

	switch(option)
	{
		case 0:
		{
			printf("\nExiting....");
			break;
		}
		case 1:
		{
			make_a_call();
			break;
		}
		default:
		{
			printf("\nPlease enter valid option.");
			break;
		}
	}

	return SUCCESS;
}

int make_a_call()
{
	char calling_number[MAX_LEN];
	char buffer[MAX_LEN];

	printf("\nEnter the number you want to call : ");
	scanf("%s", calling_number);

	if(validate_number(calling_number) == FAILURE)
		return FAILURE;

	strcpy(buffer, calling_number);
	WRITE(socket_fd, buffer);

	return SUCCESS;
}

void* send_message(void *arg)
{
	char buffer[MAX_LEN];

	select_option();

	while(1)
	{
		printf("\nEnter message : ");
		fgets(buffer, sizeof(buffer), stdin);
		WRITE(socket_fd, buffer);
	}
}

void* receive_call(void *arg)
{
	char buffer[MAX_LEN];

	while(1)
	{
		READ(socket_fd, buffer);
		printf("\nMessage received : %s", buffer);
	}
}
