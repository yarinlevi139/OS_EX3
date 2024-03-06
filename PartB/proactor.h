
#ifndef MATALA3_PROACTOR_H
#define MATALA3_PROACTOR_H

void *handle_client(void *p_client_socket);
void send_to_all_clients(int sender_socket, char *message);
void *handle_client(void *p_client_socket);
void createThreadForClient(int socket_descriptor);

#endif //MATALA3_PROACTOR_H
