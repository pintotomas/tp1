#include "protocol.h"
#include <string.h>
bool protocol_server_init(protocol_t *self, const char *port) {
    socket_t *socket = malloc(sizeof(socket_t));
    if (socket_bind_and_listen(socket, port) == -1){
        free(socket);
        return false;
    }

    self->server_socket = socket;
    self->client_socket = NULL;
    return true;
}

bool protocol_client_init(protocol_t *self, const char *host, const char *port){

	bool error = false;
    socket_t *server = malloc(sizeof(socket_t));
    if (socket_connect(server, host, port) != 0){
    	error = true;
    }

    self->server_socket = server;
    socket_t *client_socket = malloc(sizeof(socket_t));

    self->client_socket = client_socket;
    self->client_socket->fd = -1;
    if (error) {
        free(self->server_socket);
        free(self->client_socket);
        return false;
    }
    return true;
}

bool protocol_server_accept(protocol_t *self){

    int client_socket = socket_accept(self->server_socket);
    if (client_socket == -1){
        free(self->server_socket);
        return false;
    }
    self->client_socket = malloc(sizeof(socket_t));
    socket_init(self->client_socket, client_socket);
    return true;
}

ssize_t protocol_client_send(protocol_t *self, char *buffer) {

    //char *msg = "Hola\n";
    printf("SENDING SOCKET_SEND: = %s\n", buffer);
    return socket_send(self->server_socket, buffer, strlen(buffer) * sizeof(char));
}

ssize_t protocol_client_receive(protocol_t *self, char *buffer) {
    return socket_receive(self->server_socket, buffer, 5);
}

ssize_t protocol_server_receive(protocol_t *self, char *buffer) {
    return socket_receive(self->client_socket, buffer, 5);
}

ssize_t protocol_server_send(protocol_t *self, char *buffer) {
    //char *msg = "Mundo\n";
    return socket_send(self->client_socket, buffer, 5);
}

void protocol_destroy(protocol_t *self) {
    if (!self) return;
    printf("self->server_socket->fd = %d\n", self->server_socket->fd);
    socket_release(self->server_socket);
    free(self->server_socket);
    printf("self->client_socket->fd = %d\n", self->client_socket->fd);
    socket_release(self->client_socket);
    free(self->client_socket);
}