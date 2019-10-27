#include <client_utilities.h>
#include <socket.h>

// Variable to store the socket id of the link.
int socket_fd = 0;
bool is_call_connected = false;
caller_status_t caller_status = CALLER_UNKNOWN;

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

	if(pthread_mutex_init(&call_connected_mutex, NULL) != 0)
	{
		printf("\ncall_connected_mutex initialization failed");
		return FAILURE;
	}

	if(pthread_mutex_init(&called_number_status_mutex, NULL) != 0)
	{
		printf("\ncall_connected_mutex initialization failed");
		return FAILURE;
	}

	if(pthread_cond_init(&call_connected_cond, NULL) != 0)
	{
		printf("\ncall_connected_mutex initialization failed");
		return FAILURE;
	}

	if(pthread_cond_init(&called_number_status_cond, NULL) != 0)
	{
		printf("\ncall_connected_mutex initialization failed");
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

	status = pthread_create(&thread_id_receive, NULL, get_server_reponse, NULL);
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
	//pthread_join(thread_id_receive,NULL);
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

			lock_call_mutex();
			if(is_call_connected)
			{
				pthread_cond_wait(&call_connected_cond, &call_connected_mutex);
				unlock_call_mutex();
				continue;
				//send_message(NULL);
			}
			unlock_call_mutex();
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
				switch_off();
				pthread_exit(NULL);
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

	lock_status_mutex();
	pthread_cond_wait(&called_number_status_cond, &called_number_status_mutex);
	status = caller_status;
	unlock_status_mutex();

	if(status != CALLER_AVAILABLE)
	{
		display_status_message(status);
		return FAILURE;
	}

	lock_call_mutex();
	is_call_connected = true;
	unlock_call_mutex();
	// Start sending message.
	//send_message(NULL);
	create_sender_receiver_threads();

	return SUCCESS;
}

void* send_message(void *arg)
{
	char buffer[MAX_LEN];

	while(1)
	{
		printf("\nEnter message : ");
		fgets(buffer, sizeof(buffer), stdin);
		//remove_newline_from_string(buffer);

		//if(!is_call_connected)
			//break;

		WRITE(socket_fd, buffer);
		if(!strcmp(buffer, EXIT))
		{
			lock_call_mutex();
			is_call_connected = false;
			pthread_cond_signal(&call_connected_cond);
			unlock_call_mutex();
			break;
		}
	}

	return NULL;//pthread_exit(NULL);
}

void* get_server_reponse(void *arg)
{
	char buffer[MAX_LEN];
	server_response_t response = RESPONSE_UNKNOWN;

	while(1)
	{
		// Receive the server response.
		READ(socket_fd, buffer);
		response = atoi(buffer);

		printf("\nServer response = %d", response);

		switch(response)
		{
			case RECEIVE_CALL:
			{
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
	}

	pthread_exit(NULL);
}

int receive_call()
{
	accept_call_request();

	receive_message(NULL);

	return SUCCESS;
}

void * receive_message(void *arg)
{
	char buffer[MAX_LEN];

	while(1)
	{
		READ(socket_fd, buffer);
		if(atoi(buffer) == DISCONNECT_CALL)
		{
			lock_call_mutex();
			is_call_connected = false;
			pthread_cond_signal(&call_connected_cond);
			unlock_call_mutex();
			break;
		}
		printf("\nMessage received : %s", buffer);
	}

	pthread_exit(NULL);
}

int receive_status()
{
	char buffer[MAX_LEN];
	caller_status_t status = CALLER_UNKNOWN;

	READ(socket_fd, buffer);
	status = atoi(buffer);
	printf("\nStatus received = %d", status);
	lock_status_mutex();
	caller_status = status;
	pthread_cond_signal(&called_number_status_cond);
	unlock_status_mutex();

	if(status == CALLER_AVAILABLE)
	{
		lock_call_mutex();
		pthread_cond_wait(&call_connected_cond, &call_connected_mutex);
		unlock_call_mutex();
	}

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

int switch_off()
{
	char buffer[MAX_LEN];

	sprintf(buffer, "%d", SWITCH_OFF);
	WRITE(socket_fd, buffer);

	return SUCCESS;
}

int accept_call_request()
{
	char buffer[MAX_LEN];
	int status = 0;
	pthread_t thread_id_send;

	READ(socket_fd, buffer);

	printf("\nReceiving a call from %s", buffer);
	sprintf(buffer, "%d", ACCEPT_CALL);
	WRITE(socket_fd, buffer);

	status = pthread_create(&thread_id_send, NULL, send_message, NULL);
	if(0 != status)
	{
		printf("\n%s : %d : Thread creation failed.", __func__, __LINE__);
		return FAILURE;
	}

	// Update the is_call_connected variable.
	lock_call_mutex();
	is_call_connected = true;
	unlock_call_mutex();

	return SUCCESS;
}

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

// Create threads for sending and receiving data.
int create_sender_receiver_threads()
{
	pthread_t thread_id_send;
	pthread_t thread_id_receive;
	int status = 0;

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
