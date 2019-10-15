#ifndef SOCKET_H
#define SOCKET_H

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define FAILURE 0
#define SUCCESS 1
#define MAX_CLIENT 5

#define ERROR_CHECK(status, msg)	\
		if(status < 0)		\
		{			\
			perror(msg);	\
			return FAILURE;	\
		}

#define READ(sock_fd, buffer)		\
		if(read(sock_fd, buffer, sizeof(buffer)) < 0)	\
		{	\
			printf("\nRead failed.");	\
			return FAILURE;	\
		}

#define WRITE(sock_fd, buffer)		\
		if(write(sock_fd, buffer, sizeof(buffer)) < 0)	\
		{	\
			printf("\nWrite failed.");	\
			return FAILURE;	\
		}

int create_socket(char *argv);

#endif
