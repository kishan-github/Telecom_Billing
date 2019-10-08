#ifndef SERVER_UTILITIES_H
#define SERVER_UTILITIES_H

#include <pthread.h>
#include <mysql/mysql.h>

#define MAX_LEN 1000
#define SERVER "localhost"
#define USER "root"
#define PASSWORD ""
#define DATABASE "Client_Details"
#define CREATE_USER_DETAILS_TABLE "CREATE TABLE user_details (user_id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, ph_no VARCHAR(10) NOT NULL, connfd INT(6) NOT NULL)"

#define PRINT(msg, ...)	\
		printf("\n%s : %d : "msg"\n", __func__, __LINE__, ##__VA_ARGS__);\

int init(char *argv);
int init_database();
int start_server();
int check_number_in_database(int new_sockfd);
void *subroutine(void * connfd);

#endif
