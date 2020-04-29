#include <stdio.h>
#include "socket.h"
#include <netdb.h>

int main(int argc, char *argv[]) {
	socket_t *socket = malloc(sizeof(socket_t));
    char *port = "8080";
    char *host = "localhost";
    int connected = socket_connect(socket, host, port, 0);
    /*Envio un msg al server*/
 	char *msg_send = "Hola\n";
    socket_send(socket, msg_send, 5);
    /*Espero respuesta 'Mundo' del server*/

    char msg[6] = {0};
    ssize_t recv = socket_receive(socket, msg, 6);
    printf("Msg rcvd from server: %s", msg);
    free(socket);
}