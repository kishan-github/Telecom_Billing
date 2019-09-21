#ifndef SOCKET_H
#define SOCKET_H

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

#define FAILURE -1
#define MAX_CLIENT 5

#define ERROR_CHECK(status, msg)	\
		if(status < 0)		\
		{			\
			perror(msg);	\
			return FAILURE;	\
		}

int create_socket(char *argv[]);

#endif
