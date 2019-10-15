#ifndef SERVER_UTILITIES_H
#define SERVER_UTILITIES_H

#include <pthread.h>

#define MAX_LEN 1000

#define PRINT(msg, ...)	\
		printf("\n%s : %d : "msg"\n", __func__, __LINE__, ##__VA_ARGS__);\

int init(char *argv);
int init_database();
int start_server();
void *subroutine(void * connfd);

#endif
