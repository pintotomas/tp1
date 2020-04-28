#include <stdio.h>
#include "socket.h"
#include <netdb.h>

int main(int argc, char *argv[]) {
    socket_t *socket = malloc(sizeof(socket_t));
    char *port = "8080";
    printf("HI");
    if (socket_bind_and_listen(socket, port, AI_PASSIVE) != 0) return false;

    int client_skt_fd = socket_accept(socket);
    socket_t *client_socket= malloc(sizeof(socket_t));
    socket_init(client_socket, client_skt_fd);

    char msg[10] = {0};
    printf("Receiving bytes from client\n");
    ssize_t recv = socket_receive(client_socket, msg, 10);
    printf("Bytes received!\n");
    printf("Msg rcvd: %s\n", msg);

    char *msg_response = "Message received\n";
    socket_send(client_socket, msg_response, 17);
    socket_send(client_socket, msg_response, 17);
    socket_send(client_socket, msg_response, 17);
    
    free(socket);
    free(client_socket);
    return true;
}