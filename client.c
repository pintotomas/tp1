#include <stdio.h>
#include "client.h"
#include <netdb.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: ./tp client <host> <puerto>\n");
        return 1;
    }
 	client_t client;
    if (!client_init(&client, argv[2], argv[3])) return 1;
    client_run(&client);
    client_destroy(&client);
    return 0;
	// printf("HI; IM MAIN");
	// socket_t *socket = malloc(sizeof(socket_t));
 //    char *port = "8080";
 //    char *host = "localhost";
 //    //int connected = socket_connect(socket, host, port);
 //    printf("CONNECTING!!");

 //    socket_connect(socket, host, port);
 //    /*Envio un msg al server*/
 //    printf("SENDING MESSAGE!!");
 // 	char *msg_send = "Hola\n";
 //    socket_send(socket, msg_send, 5);
 //    printf("MSG SENT!!");
 //    /*Espero respuesta 'Mundo' del server*/
 //    char msg[6] = {0};
 //    //ssize_t recv = socket_receive(socket, msg, 6);
 //    socket_receive(socket, msg, 6);
 //    printf("Msg rcvd from server: %s", msg);
 //    socket_release(socket);
 //    free(socket);
}

bool client_init(client_t *self, char *host, char *port) {
    protocol_t *protocol = malloc(sizeof(protocol_t));
    if (!protocol || !protocol_client_init(protocol, host, port)) {
        free(protocol);
        return false;
    }
    self->protocol = protocol;
    return true;
}

void client_run(client_t *self) {
    while (true) {
        char buffer[5] = {0};
        if (!fgets(buffer, 5, stdin)) break;
        ssize_t send = protocol_client_send(self->protocol, buffer);
        if (send == -1) break;
        else if (send == 0) continue;
        char output[5] = {0};
        ssize_t received = protocol_client_receive(self->protocol, output);
        if (received <= 0) break;
        printf("%s", output);
    }
}


void client_destroy(client_t *self) {
    if (!self) return;
    protocol_destroy(self->protocol);
    free(self->protocol);
}