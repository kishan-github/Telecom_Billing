#ifndef CLIENT_UTILITIES_H
#define CLIENT_UTILITIES_H

#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/wait.h>

#define MAX_LEN 1000
#define PHONE_NO_LENGTH 10
#define CALL "CALL"
#define EXIT "EXIT\n"

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

int init(char *argv);
int start_client_app(char *ph_no);
int register_with_server(char *ph_no);
int validate_number(char *ph_no);
void select_option(int pid);
int make_a_call();
int get_server_reponse(void);
int receive_call();
int receive_status();
void display_status_message(caller_status_t status);
int switch_off();
int accept_call_request();
void lock_call_mutex();
void unlock_call_mutex();
void remove_newline_from_string(char *str);
void lock_status_mutex();
void unlock_status_mutex();
int create_sender_receiver_threads();
int send_message(void);
int receive_message(int pid);
void terminate_process(int sig_no);
void notify_server_and_terminate_process(int sig_no);

#endif
