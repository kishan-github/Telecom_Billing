#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>

#define MAX_LEN 1000

int init(char *argv);
int start_server();
void *subroutine(void * connfd);

#endif
