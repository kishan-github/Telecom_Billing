#ifndef SERVER_UTILITIES_H
#define SERVER_UTILITIES_H

#include <pthread.h>
#include <database.h>

#define MAX_LEN 1000
#define FREE "FREE"
#define BUSY "BUSY"
#define OFF "SWITCHED_OFF"
#define WRONG_NUMBER "WRONG_NUMBER"
#define UNKNOWN "UNKNOWN"
#define EXIT "EXIT"

#define PRINT(msg, ...)	\
		printf("\n%s : %d : "msg"\n", __func__, __LINE__, ##__VA_ARGS__);\

typedef struct connection{
	int sender_connfd;
	int receiver_connfd;
}connection_t;

typedef enum user_input{
	SETUP_CALL,
	ACCEPT_CALL,
	DISCONNECT_CALL,
	SWITCH_OFF,
	UNKNOWN_REQUEST
}user_input_t;

typedef enum caller_status{
	CALLER_AVAILABLE,
	CALLER_BUSY,
	CALLER_SWITCHED_OFF,
	CALLER_NOT_REGISTERED,
	CALLER_UNKNOWN
}caller_status_t;

typedef enum server_response{
	RECEIVE_CALL,
	RECEIVE_STATUS,
	RESPONSE_UNKNOWN
}server_response_t;

int start_server();
void *subroutine(void * connfd);
int get_user_input(int socket_fd, int caller_user_id);
int create_call(int socket_fd, int caller_user_id);
void get_status_string(user_status_t status, char *buffer);
int create_sender_receiver_threads(int sender_connfd, int receiver_connfd);
void* send_message(void *connfd);
void* receive_message(void *connfd);
caller_status_t map_user_status_to_caller(user_status_t status);
int disconnect_user(int socket_fd, int caller_user_id);

#endif
