
#ifndef MATALA3_PROACTOR_H
#define MATALA3_PROACTOR_H

#include <pthread.h>

typedef void *(*ClientHandler)(void * arg);

void Proactor(int server_socket ,ClientHandler clientHandler);

int *clients = NULL;
int num_clients = 0;
int capacity = 0;


#endif //MATALA3_PROACTOR_H
