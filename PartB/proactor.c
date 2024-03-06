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

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int *clients = NULL;
int num_clients = 0;
int capacity = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void send_to_all_clients(int sender_socket, char *message)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clients; i++)
    {
        int client_socket = clients[i];
        if (client_socket != sender_socket)
        {
            send(client_socket, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *handle_client(void *p_client_socket)
{
    int client_socket = *(int *)p_client_socket;
    free(p_client_socket);

    pthread_mutex_lock(&mutex);
    clients[num_clients++] = client_socket;
    pthread_mutex_unlock(&mutex);

    char message[BUFFER_SIZE];

    while (1)
    {
        bzero(message, BUFFER_SIZE);
        ssize_t bytes_received = recv(client_socket, message, BUFFER_SIZE, 0);
        if (bytes_received < 1)
        {
            printf("Client %d left the chat\n", client_socket);

            pthread_mutex_lock(&mutex);
            for (int i = 0; i < num_clients; i++)
            {
                if (clients[i] == client_socket)
                {
                    for (int j = i; j < num_clients - 1; j++)
                    {
                        clients[j] = clients[j + 1];
                    }
                    num_clients--;
                    capacity--;
                    clients = realloc(clients,capacity*(sizeof(int)));
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);

            close(client_socket);
            pthread_exit(NULL); // Terminate the thread
        }
        message[bytes_received - 1] = '\0';
        printf("Received %s from client %d\n", message, client_socket);
        fflush(stdout);

        // Forward the message to all other clients
        send_to_all_clients(client_socket, message);
    }
    return NULL;
}

void createThreadForClient(int client_socket)
{
    //realloc:
    capacity++;
    clients = realloc(clients,capacity*(sizeof(int)));

    printf("Accepted client %d\n", client_socket);
    fflush(stdout);
    pthread_t t;
    int *p_client_sock = malloc(sizeof(int));
    *p_client_sock = client_socket;
    pthread_create(&t, NULL, handle_client, p_client_sock);
}

