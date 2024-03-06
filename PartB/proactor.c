#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include "proactor.h"


//Proactor creates a new thread that handles the function



void Proactor(int server_socket ,ClientHandler clientHandler){

    while (1) { // The main thread listens for incoming connections:
        int client_socket = accept(server_socket, NULL, NULL);
        capacity++;
        clients = realloc(clients,capacity*(sizeof(int)));

        printf("Accepted client %d\n", client_socket);
        fflush(stdout);

        int *p_client_sock = malloc(sizeof(int));
        *p_client_sock = client_socket;

        pthread_t t;
        pthread_create(&t, NULL, clientHandler, p_client_sock);
    }

}
