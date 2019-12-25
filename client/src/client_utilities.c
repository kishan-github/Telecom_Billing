#include <client_utilities.h>
#include <socket.h>

// Variable to store the socket id of the link.
int socket_fd = 0;
int client_app_process_id = 0;

// Mutex and condition variables.
pthread_mutex_t call_connected_mutex;
pthread_mutex_t called_number_status_mutex;
pthread_cond_t call_connected_cond;
pthread_cond_t called_number_status_cond;

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

	client_app_process_id = getpid();	// Store the process id of parent process which will be used during switch off case.
	signal(SIGUSR1, terminate_process);

	do
	{
		// Create a process to handle user input and server response.
		pid = fork();
		if(pid == 0)
		{
			pid = fork();

			if(pid == 0)	// To handle server response.
			{
				//child
				signal(SIGUSR1, terminate_process);
				get_server_reponse();

			}
			else if(pid > 0)	// To handle user input.
			{
				// parent
				signal(SIGUSR1, terminate_process);
				select_option(pid);
				kill(getpid(), SIGUSR1);
			}
			else
			{
				printf("Process creation failed.");
			}
		}
		wait(NULL);
	}while(1);
	kill(getpid(), SIGUSR1);
}

// Add the number in the database.
int register_with_server(char *ph_no)
{
	char buffer[MAX_LEN];

	// Check if the number is valid or not.
	if(validate_number(ph_no) == FAILURE)
		return FAILURE;

	strcpy(buffer, ph_no);
	WRITE(socket_fd, buffer);

	return SUCCESS;
}

// Check the validity of the number.
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

// Wait for the server response.
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

	kill(getpid(), SIGUSR1);
	return SUCCESS;
}

// Receive the call request from the server.
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

// Accept the call request. Note - Currently all calls are accepted by default.
int accept_call_request()
{
	char buffer[MAX_LEN];

	READ(socket_fd, buffer);

	printf("\nReceiving a call from %s", buffer);
	sprintf(buffer, "%d", ACCEPT_CALL);
	WRITE(socket_fd, buffer);

	return SUCCESS;
}

// Receive the status of the number.
int receive_status()
{
	char buffer[MAX_LEN];
	caller_status_t status = CALLER_UNKNOWN;

	READ(socket_fd, buffer);
	status = atoi(buffer);
	printf("\nStatus received = %d", status);

	return SUCCESS;
}

// Wait for user input.
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

// Terminate the client app.
int switch_off()
{
	char buffer[MAX_LEN];

	sprintf(buffer, "%d", SWITCH_OFF);
	WRITE(socket_fd, buffer);

	kill(client_app_process_id, SIGUSR1);

	return SUCCESS;
}

// Setup the call.
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

	// Receive the status of the called number.
	READ(socket_fd, buffer);
	status = atoi(buffer);

	// Display the appropriate error message.
	if(status != CALLER_AVAILABLE)
	{
		display_status_message(status);
		return FAILURE;
	}

	// Create master process for handling send and receive.
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
		signal(SIGUSR1, notify_server_and_terminate_process);
		send_message();
	}
	else if(pid > 0)
	{
		// parent
		signal(SIGUSR2, notify_server_and_terminate_process);
		receive_message(pid);
	}
	else
	{
		printf("Creation of processes failed");
	}

	return SUCCESS;
}

// Send messages to server.
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
			kill(getppid(), SIGUSR2);
			break;
		}
	}
	exit(0);
}

// Receive messages from server.
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

// This method notify the server to that call is finished and kill the current process.
void notify_server_and_terminate_process(int sig_no)
{
	char buffer[MAX_LEN];

	wait(NULL);	// Wait for all the child process to finish.

	if(sig_no == SIGUSR1)
	{
		strcpy(buffer, EXIT);
		write(socket_fd, buffer, sizeof(buffer));	//Notify server to dismiss call.
	}
	else if(sig_no == SIGUSR2)
	{
		read(socket_fd, buffer, sizeof(buffer));	//Receive the garbage message.
	}
	exit(0);
}

// This method terminates the current process.
void terminate_process(int sig_no)
{
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
