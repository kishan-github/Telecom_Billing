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

#define PRINT(msg, ...)	\
		printf("\n%s : %d : "msg"\n", __func__, __LINE__, ##__VA_ARGS__);\

typedef struct connection{
	int sender_connfd;
	int receiver_connfd;
}connection_t;

int start_server();
void *subroutine(void * connfd);
int get_user_input(int socket_fd, int caller_user_id);
int create_call(char *calling_number, int socket_fd, int caller_user_id);
void get_status_string(user_status_t status, char *buffer);
int create_sender_receiver_threads(int sender_connfd, int receiver_connfd);
void* send_message(void *connfd);
void* receive_message(void *connfd);

#endif
