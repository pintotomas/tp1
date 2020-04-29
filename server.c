#include <stdio.h>
#include "socket.h"
#include <netdb.h>

int main(int argc, char *argv[]) {
    socket_t *socket = malloc(sizeof(socket_t));
    char *port = "8080";
    if (socket_bind_and_listen(socket, port) != 0) { 
    	socket_release(socket);
    	free(socket);
    	return false;
    }

    int client_skt_fd = socket_accept(socket);
    socket_t *client_socket= malloc(sizeof(socket_t));
    socket_init(client_socket, client_skt_fd);

    char msg[5] = {0};
    //ssize_t recv = socket_receive(client_socket, msg, 5);
    socket_receive(client_socket, msg, 5);
    printf("Msg rcvd from client: %s", msg);

    char *msg_response = "Mundo\n";
    socket_send(client_socket, msg_response, 6);

    free(socket);
    free(client_socket);
    return true;
}
