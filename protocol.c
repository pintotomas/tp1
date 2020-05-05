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
    if (client_socket == -1) {
        free(self->server_socket);
        return false;
    }
    self->client_socket = malloc(sizeof(socket_t));
    socket_init(self->client_socket, client_socket);
    return true;
}

ssize_t protocol_client_send(protocol_t *self, char *buffer) {

    //char *msg = "Hola\n";
    //int body_length;
    dbus_encoder_t *dbus_encoder = malloc(sizeof(dbus_encoder_t));
    dbus_encoder_init(dbus_encoder, buffer);
    //unsigned char* body = create_send_message(buffer, &body_length);
    dbus_encoder_create_send_message(dbus_encoder);
    // //printf("*************AT PROTOCOL********************\n");
    printf("Header total size: %d\n", dbus_encoder->header_length);
    // printf("@@@@@@@Sending 10 bytes@@@@@@\n");
    // // for (int j = 0; j < body_length; j++) {
    // //     printf("Current byte: %x\n", body[j]);   
    // //     //printf("Current byte: %x\n", body[j]);    
    // // } 
    // for (int j = 0; j < dbus_encoder->body_length; j++) {
    //     printf("Current byte: %x\n", dbus_encoder->body[j]);  
    // } 
    ssize_t bytes_sent = socket_send(self->server_socket, dbus_encoder->header, dbus_encoder->header_length);
    bytes_sent = socket_send(self->server_socket, dbus_encoder->body, dbus_encoder->body_length);

    //free(body);
    dbus_encoder_destroy(dbus_encoder);
    free(dbus_encoder);
    return bytes_sent;
    
}

ssize_t protocol_client_receive(protocol_t *self, char *buffer) {
    return socket_receive(self->server_socket, buffer, 5);
}

//ssize_t protocol_server_receive(protocol_t *self, char *buffer) {
ssize_t protocol_server_receive(protocol_t *self) {    
    dbus_decoder_t *dbus_decoder = malloc(sizeof(dbus_decoder_t));
    dbus_decoder_init(dbus_decoder);
    //Recibo primeros 16 bytes con descripcion del header y body
    unsigned char buffer[16];
    ssize_t received = socket_receive(self->client_socket, buffer, 16);

    ssize_t remaining_bytes = dbus_decoder_set_descriptions(dbus_decoder, buffer);
    unsigned char buffer2[remaining_bytes];
    printf("Waiting to receive: %ld bytes\n", remaining_bytes);
    received = socket_receive(self->client_socket, buffer2, remaining_bytes);
    printf("Received: %ld bytes\n", received);

    // for (int j = 0; j < remaining_bytes; j++) {
    //     printf("Current byte received: %x, (char): %c\n", buffer2[j], buffer2[j]);   
    //     //printf("Current byte: %x\n", body[j]);    
    // } 
    dbus_message_t * message = dbus_decoder_decode(dbus_decoder, buffer2);
    printf("Ruta: %s\n", message->ruta); 
    printf("Destino: %s\n", message->destino); 
    printf("Metodo: %s\n", message->metodo); 
    printf("Interfaz: %s\n", message->interfaz); 
    printf("@@@PARAMETROS@@@: \n");
    for (int u = 0; u < message->cantidad_parametros; u++) {
        printf("Param[%d]: %s\n", u, message->parametros[u]); 
    }
    free(dbus_decoder);
    //Esto lo deberia hacer el server despues
    dbus_message_destroy(message);
    free(message);
    return 0;
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