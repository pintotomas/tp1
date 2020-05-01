#include <stdio.h>
#include "client.h"
#include <netdb.h>

int main(int argc, char *argv[]) {
    if ((argc > 4) || (argc < 3) ) {
        fprintf(stderr, "Uso: ./client <host> <puerto> [<archivo de entrada>]\n");
        return 1;
    }
 	client_t client;
    bool success_init = true;
    if (argc == 4) success_init &= client_init(&client, argv[1], argv[2], argv[3]);
    else success_init &= client_init(&client, argv[1], argv[2], NULL);
    if (!success_init) return 1;
    // if (argc == 4) (&client)->filename = argv[3];
    // else (&client)->filename = NULL;
    // if (!client_init(&client, argv[1], argv[2])) return 1;
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

bool client_init(client_t *self, char *host, char *port, char *filename) {
    protocol_t *protocol = malloc(sizeof(protocol_t));
    if (!protocol || !protocol_client_init(protocol, host, port)) {
        free(protocol);
        return false;
    }
    self->protocol = protocol;
    if (filename != NULL) self->input = fopen(filename, "rb"); 
    else self->input = stdin;
    //self->filename = filename;
    //printf("Filename: %s\n", self->filename);
    return true;
}

void client_run(client_t *self) {
    while (true) {
        /* TO DO: Buffer deberia ser dinamico ya que al leer lineas nunca sabemos hasta donde lleguen */
        /*MALLOC Y REALLOC*/
        char buffer[5] = {0};
        if (!fgets(buffer, 5, self->input)) break;
        /* TO DO: Antes de enviar, procesar lo recibido y armar la cadena correspondiente segun al enunciado */
        ssize_t send = protocol_client_send(self->protocol, buffer);
        if (send == -1) break;
        else if (send == 0) continue;
        char output[6] = {0};
        ssize_t received = protocol_client_receive(self->protocol, output);
        if (received <= 0) break;
        printf("%s", output);
    }
}


void client_destroy(client_t *self) {
    if (!self) return;
    protocol_destroy(self->protocol);
    free(self->protocol);
    fclose(self->input);
}