#ifndef CLIENT_UTILITIES_H
#define CLIENT_UTILITIES_H

#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LEN 1000
#define PHONE_NO_LENGTH 10
#define CALL "CALL"

typedef enum user_input{
	SETUP_CALL,
	ACCEPT_CALL,
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

int init(char *argv);
int start_client_app(char *ph_no);
int register_with_server(char *ph_no);
int validate_number(char *ph_no);
void *select_option(void *arg);
int make_a_call();
int send_message();
void* receive_call(void *arg);
void display_status_message(caller_status_t status);
int switch_off();

#endif
