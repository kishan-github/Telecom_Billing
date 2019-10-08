#ifndef CLIENT_UTILITIES_H
#define CLIENT_UTILITIES_H

#include <pthread.h>
#include <ctype.h>

#define MAX_LEN 1000
#define PHONE_NO_LENGTH 10

int init(char *argv);
int start_client_app(char *ph_no);
int register_with_server(char *ph_no);
int validate_number(char *ph_no);
void* send_message(void *arg);
void* receive_message(void *arg);

#endif
