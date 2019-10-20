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
#define CREATE_USER_STATUS_TABLE "CREATE TABLE user_status (user_id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, status INT(6))"
#define DELETE_USER_DETAILS_TABLE "DROP TABLE user_details"
#define DELETE_USER_STATUS_TABLE "DROP TABLE user_status"

#define PRINT(msg, ...)	\
		printf("\n%s : %d : "msg"\n", __func__, __LINE__, ##__VA_ARGS__);\

typedef enum user_status
{
	USER_AVAILABLE,
	USER_BUSY,
	USER_SWITCHED_OFF,
	USER_UNKNOWN
}user_status_t;

int init_database();
int deinit_database();
int create_tables();
int add_number_in_database(int socket_fd, char *phone_number);
int is_number_already_in_database(char *phone_number, bool *is_available);
int get_user_status(int user_id, user_status_t *status);
int get_user_id(char *number, int *user_id);
int get_user_connection_fd(int user_id, int *receiver_connfd);

#endif
