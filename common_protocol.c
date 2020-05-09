#include "protocol.h"
#include <string.h>

bool protocol_server_init(protocol_t *self, const char *port) {
    // uso innecesario del heap
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
    // separar en protocolo_cliente y protocolo_server
    // uso innecesario del heap
    // si la funcion dice cliente por que creas un server aca?
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
    if (client_socket == -1) {
        free(self->server_socket);
        return false;
    }
    self->client_socket = malloc(sizeof(socket_t));
    socket_init(self->client_socket, client_socket);
    return true;
}

ssize_t protocol_client_send(protocol_t *self, char *buffer, int id_mensaje) {
    // uso innesario del heap
    dbus_encoder_t *dbus_encoder = malloc(sizeof(dbus_encoder_t));
    dbus_encoder_init(dbus_encoder, buffer, id_mensaje);
    dbus_encoder_encode(dbus_encoder);
    ssize_t bytes_sent = socket_send(self->server_socket,
         dbus_encoder->header, dbus_encoder->header_length);
    bytes_sent += socket_send(self->server_socket,
         dbus_encoder->body, dbus_encoder->body_length);
    dbus_encoder_destroy(dbus_encoder);
    free(dbus_encoder);
    return bytes_sent; 
}

ssize_t protocol_client_receive(protocol_t *self, char *buffer, int bytes) {
    return socket_receive(self->server_socket, buffer, bytes);
}

dbus_message_t* protocol_server_receive(protocol_t *self) {    
    dbus_decoder_t *dbus_decoder = malloc(sizeof(dbus_decoder_t));
    dbus_decoder_init(dbus_decoder);
    //Recibo primeros 16 bytes con descripcion del header y body
    unsigned char buffer[16];
    ssize_t received = socket_receive(self->client_socket, buffer, 16);
    if (received <= 0) {
        free(dbus_decoder->decoded_message);
        free(dbus_decoder);
        return NULL;
    }
    ssize_t remaining_bytes =
     dbus_decoder_set_descriptions(dbus_decoder, buffer);
    unsigned char buffer2[remaining_bytes];
    received = socket_receive(self->client_socket, buffer2, remaining_bytes);
    if (received <= 0) {
        free(dbus_decoder->decoded_message);
        free(dbus_decoder);
        return NULL;
    }
    dbus_message_t * message = dbus_decoder_decode(dbus_decoder, buffer2);
    free(dbus_decoder);
    return message;
}

ssize_t protocol_server_send(protocol_t *self, char *buffer, int size) {
    return socket_send(self->client_socket, buffer, size);
}

void protocol_destroy(protocol_t *self) {
    if (!self) return;
    socket_release(self->server_socket);
    free(self->server_socket);
    socket_release(self->client_socket);
    free(self->client_socket);
}
