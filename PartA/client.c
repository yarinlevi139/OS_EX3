#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h> // Include pthread.h for threading support

#define BUFFER_SIZE 1024

void* keyboard_listen(void *p_client_socket)
{
    int client_socket = *((int*)p_client_socket);
    char buffer[BUFFER_SIZE];

    // Continuously get input from the keyboard and send it to the server
    while(1) {
        bzero(buffer, BUFFER_SIZE);
        printf("Enter message:\n");
        fflush(stdout);
        fgets(buffer, BUFFER_SIZE, stdin);

        // Send the input to the server
        if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
            perror("Error sending to server");
            exit(EXIT_FAILURE);
        }
    }

    return NULL;
}

int main()
{
    const char *server_address = "127.0.0.1";
    const char *server_port = "8080";

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct hostent *server = gethostbyname(server_address);
    if (server == NULL) {
        fprintf(stderr, "Error: No such host\n");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address_info;
    memset(&server_address_info, 0, sizeof(server_address_info));
    server_address_info.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address_info.sin_addr.s_addr, server->h_length);
    server_address_info.sin_port = htons(atoi(server_port));

    if (connect(client_socket, (struct sockaddr *)&server_address_info, sizeof(server_address_info)) < 0) {
        perror("Error connecting to server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Create a thread for listening to keyboard input
    pthread_t t;
    pthread_create(&t, NULL, keyboard_listen, (void*)&client_socket);

    // Main thread listens for messages from the server
    char server_response[BUFFER_SIZE];
    while(1) {
        bzero(server_response, BUFFER_SIZE);
        ssize_t bytes_received = recv(client_socket, server_response, BUFFER_SIZE, 0);
        server_response[bytes_received] ='\0';
        if (bytes_received < 0) {
            perror("Error reading from server");
            exit(EXIT_FAILURE);
        } else if (bytes_received == 0) {
            printf("Server closed the connection\n");
            break;
        }
        printf("Server: %s\n", server_response);
        fflush(stdout);
    }

    // Cleanup
    close(client_socket);
    pthread_cancel(t); // Optionally cancel the thread if it's still running
    pthread_join(t, NULL); // Wait for the thread to finish

    return 0;
}
