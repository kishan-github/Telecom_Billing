#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FAILURE 0
#define SUCCESS 1
#define MAX_LEN 1000

#define SERVER "localhost"
#define USER "root"
#define PASSWORD ""
#define DATABASE "Client_Details"
#define CREATE_USER_DETAILS_TABLE "CREATE TABLE user_details (user_id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, ph_no VARCHAR(10) NOT NULL, connfd INT(6) NOT NULL)"

#define PRINT(msg, ...)	\
		printf("\n%s : %d : "msg"\n", __func__, __LINE__, ##__VA_ARGS__);\

int init_database();
int add_number_in_database(int socket_fd, char *phone_number);
int is_number_already_in_database(char *phone_number, bool *is_available);

#endif
