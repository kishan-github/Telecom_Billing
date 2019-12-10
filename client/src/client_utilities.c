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
	int pid = 0;

	if(register_with_server(ph_no) == FAILURE)
		return FAILURE;

	do
	{
		pid = fork();
		if(pid == 0)
		{
			pid = fork();

			if(pid == 0)
			{
				//child
				signal(SIGUSR1, terminate_self);
				get_server_reponse();

			}
			else if(pid > 0)
			{
				// parent
				signal(SIGUSR1, wait_for_child_process_to_exit);
				select_option(pid);
			}
			else
			{
				printf("Process creation failed.");
			}
		}

		wait(NULL);
	}while(1);

	exit(0);
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

int get_server_reponse(void)
{
	char buffer[MAX_LEN];
	server_response_t response = RESPONSE_UNKNOWN;

	// Receive the server response.
	READ(socket_fd, buffer);
	response = atoi(buffer);

	printf("\nServer response = %d", response);

	switch(response)
	{
		case RECEIVE_CALL:
		{
			kill(getppid(), SIGUSR1);
			receive_call();
			break;
		}
		case RECEIVE_STATUS:
		{
			receive_status();
			break;
		}
		default:
			printf("\nUnknown response from server.");
	}
	exit(0);
}

int receive_call()
{
	int pid = 0;

	accept_call_request();

	pid = fork();
	if(pid == 0)
	{
		create_sender_receiver_threads();
	}

	wait(NULL);

	return SUCCESS;
}

int accept_call_request()
{
	char buffer[MAX_LEN];

	READ(socket_fd, buffer);

	printf("\nReceiving a call from %s", buffer);
	sprintf(buffer, "%d", ACCEPT_CALL);
	WRITE(socket_fd, buffer);

	return SUCCESS;
}

int receive_status()
{
	char buffer[MAX_LEN];
	caller_status_t status = CALLER_UNKNOWN;

	READ(socket_fd, buffer);
	status = atoi(buffer);
	printf("\nStatus received = %d", status);

	return SUCCESS;
}

void select_option(int pid)
{
	int option = 0;
	int input = 0;

	printf("\nSelect from the options given below:-");
	printf("\n1 : To make a call");
	printf("\n0 : Exit");

	do
	{
		input = scanf("%d", &option);
		printf("entered input = %d", option);

		if(input != 1)
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
			kill(pid, SIGUSR1);
			switch_off();
			break;
		}
		case 1:
		{
			kill(pid, SIGUSR1);
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

int switch_off()
{
	char buffer[MAX_LEN];

	sprintf(buffer, "%d", SWITCH_OFF);
	WRITE(socket_fd, buffer);

	return SUCCESS;
}

int make_a_call()
{
	int pid = 0;
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

	READ(socket_fd, buffer);
	status = atoi(buffer);

	if(status != CALLER_AVAILABLE)
	{
		display_status_message(status);
		return FAILURE;
	}

	pid = fork();
	if(pid == 0)
	{
		create_sender_receiver_threads();
	}

	wait(NULL);

	return SUCCESS;
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

// Create threads for sending and receiving data.
int create_sender_receiver_threads()
{
	int pid = 0;

	pid = fork();

	if(pid == 0)
	{
		//child
		printf("\nsend message pid = %d", getpid());
		signal(SIGUSR1, terminate_self);
		send_message();
	}
	else if(pid > 0)
	{
		// parent
		printf("\nreceive message pid = %d", getpid());
		signal(SIGUSR1, wait_for_child_process_to_exit);
		receive_message(pid);
	}
	else
	{
		printf("Creation of processes failed");
	}

	return SUCCESS;
}

int send_message(void)
{
	char buffer[MAX_LEN];

	while(1)
	{
		printf("\nEnter message : ");
		fgets(buffer, sizeof(buffer), stdin);

		WRITE(socket_fd, buffer);
		if(!strcmp(buffer, EXIT))
		{
			kill(getppid(), SIGUSR1);
			break;
		}
	}

	exit(0);
}

int receive_message(int pid)
{
	char buffer[MAX_LEN];

	while(1)
	{
		READ(socket_fd, buffer);
		if(atoi(buffer) == DISCONNECT_CALL)
		{
			kill(pid, SIGUSR1);
			break;
		}
		printf("\nMessage received : %s", buffer);
	}

	exit(0);
}

void terminate_self(int sig_no)
{
	//printf("Signal received for ending call. pid = %d", getpid());
	exit(0);
}

void wait_for_child_process_to_exit(int sig_no)
{
	//printf("Wait for child process to exit. pid = %d", getpid());
	wait(NULL);
	exit(0);
}
#if 0
void lock_call_mutex()
{
	int result = 0;

	result = pthread_mutex_lock(&call_connected_mutex);

	/*if(result == 0)
		printf("\ncall_connected_mutex mutex acquired");
	else
		printf("\ncall_connected_mutex mutex lock failed");*/
}

void unlock_call_mutex()
{
	int result = 0;

	result = pthread_mutex_unlock(&call_connected_mutex);

	/*if(result == 0)
		printf("\ncall_connected_mutex mutex released");
	else
		printf("\ncall_connected_mutex unlock failed");*/
}

void remove_newline_from_string(char *str)
{
	char *pos = NULL;
	if ((pos=strchr(str, '\n')) != NULL)
	    *pos = '\0';
}

void lock_status_mutex()
{
	int result = 0;

	result = pthread_mutex_lock(&called_number_status_mutex);

	/*if(result == 0)
		printf("\ncalled_number_status_mutex mutex acquired");
	else
		printf("\ncalled_number_status_mutex mutex lock failed");*/
}

void unlock_status_mutex()
{
	int result = 0;

	result = pthread_mutex_unlock(&called_number_status_mutex);

	/*if(result == 0)
		printf("\ncalled_number_status_mutex mutex released");
	else
		printf("\ncalled_number_status_mutex unlock failed");*/
}
#endif
