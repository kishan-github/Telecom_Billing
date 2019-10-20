#include <client_utilities.h>
#include <socket.h>

// Variable to store the socket id of the link.
int socket_fd = 0;
bool is_call_connected = false;

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

	status = pthread_create(&thread_id_receive, NULL, receive_call, NULL);
	if(0 != status)
	{
		printf("\n%s : %d : Thread creation failed.", __func__, __LINE__);
		return FAILURE;
	}

	status = pthread_create(&thread_id_send, NULL, select_option, NULL);
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

void *select_option(void *arg)
{
	int option = 0;
	int input = 0;

	while(1)
	{
		printf("\nSelect from the options given below:-");
		printf("\n1 : To make a call");
		printf("\n0 : Exit");

		do
		{
			input = scanf("%d", &option);
			printf("entered input = %d", option);
			if(is_call_connected)
			{
				send_message();
			}
			else if(input != 1)
			{
				printf("\nNon numeric input is not allowed.");
				while(getchar() != '\n');
			}
			else
				break;
		}while(1);

		switch(option)
		{
			case 0:
			{
				printf("\nExiting....");
				switch_off();
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
	}

	pthread_exit(NULL);
}

int make_a_call()
{
	char calling_number[MAX_LEN];
	char buffer[MAX_LEN];
	caller_status_t status = CALLER_UNKNOWN;

	// Tell server that user wants to make a call.
	sprintf(buffer, "%d", SETUP_CALL);
	WRITE(socket_fd, buffer);

	printf("\nEnter the number you want to call : ");
	scanf("%s", calling_number);
	while(getchar() != '\n');

	// Validate the number.
	if(validate_number(calling_number) == FAILURE)
		return FAILURE;

	// Send the calling number to server to check its status.
	strcpy(buffer, calling_number);
	WRITE(socket_fd, buffer);

	// Read the status of the calling number.
	READ(socket_fd, buffer);
	status = atoi(buffer);

	printf("\nStatus received = %d", status);

	if(status != CALLER_AVAILABLE)
	{
		display_status_message(status);
		return FAILURE;
	}

	is_call_connected = true;
	// Start sending message.
	send_message();

	return SUCCESS;
}

int send_message()
{
	char buffer[MAX_LEN];

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

	READ(socket_fd, buffer);

	if(!is_call_connected)
	{
		printf("Receiving a call from %s", buffer);
		sprintf(buffer, "%d", ACCEPT_CALL);
		WRITE(socket_fd, buffer);
		is_call_connected = true;
	}
	else
		printf("\nMessage received : %s", buffer);
	while(1)
	{
		READ(socket_fd, buffer);
		printf("\nMessage received : %s", buffer);
	}
}

void display_status_message(caller_status_t status)
{
	switch(status)
	{
		case CALLER_BUSY:
		{
			printf("\nCalled number is busy on another call.");
			break;
		}
		case CALLER_SWITCHED_OFF:
		{
			printf("\nCalled number is switched off.");
			break;
		}
		case CALLER_NOT_REGISTERED:
		{
			printf("\nCalled number does not exist.");
			break;
		}
		case CALLER_UNKNOWN:
		{
			printf("\nCaller unknown.");
			break;
		}
		default:
		{
			printf("\nSomething went wrong....");
		}
	}
}

int switch_off()
{
	char buffer[MAX_LEN];

	sprintf(buffer, "%d", CALLER_SWITCHED_OFF);
	WRITE(socket_fd, buffer);

	return SUCCESS;
}
