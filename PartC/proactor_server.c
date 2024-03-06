#include  "../PartB/proactor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>


#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10


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



int main()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Error binding socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0)
    {
        perror("Error listening for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");
    Proactor(server_socket,&handle_client);



}