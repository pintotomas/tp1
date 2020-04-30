#include <stdio.h>
#include "server.h"
#include <netdb.h>

int main(int argc, char *argv[]) {

	if (argc != 3) {
    	fprintf(stderr, "Uso: ./tp server <puerto> (Ej: ./tp server 8080)\n");
    	return 1;
    }
    server_t server;
    if (!server_init(&server, argv[2])) return 1;
    server_run(&server);
    server_destroy(&server);
    return 0;
 //    socket_t *socket = malloc(sizeof(socket_t));
 //    char *port = "8080";
 //    if (socket_bind_and_listen(socket, port) != 0) { 
 //    	printf("HOLA");
 //    	socket_release(socket);
 //    	free(socket);
 //    	return false;
 //    }
 //    int client_skt_fd = socket_accept(socket);
 //    socket_t *client_socket= malloc(sizeof(socket_t));
 //    socket_init(client_socket, client_skt_fd);

 //    char msg[5] = {0};
 //    //ssize_t recv = socket_receive(client_socket, msg, 5);
 //    socket_receive(client_socket, msg, 5);
 //    printf("Msg rcvd from client: %s", msg);

 //    char *msg_response = "Mundo\n";
 //    socket_send(client_socket, msg_response, 6);
	// socket_release(socket);
	// socket_release(client_socket);
 //    free(socket);
 //    free(client_socket);
 //    return true;
}

bool server_init(server_t *self, char *port){

    protocol_t *protocol = malloc(sizeof(protocol_t));
    if (protocol != NULL) {
	    if (protocol_server_init(protocol, port)){
		    self->protocol = protocol;
		    return true;
	     }
	     else { 
	     	server_destroy(self); 
	     	return false;
	     }
     }

    return false;
}

void server_run(server_t *self){
    protocol_server_accept(self->protocol);
    while (true) {
        char buffer[5] = {0};
        if (protocol_server_receive(self->protocol, buffer) <= 0) break; //Termino si no recibo nada mas del cliente
        char *buffer2 = "Mundo\n";
        protocol_server_send(self->protocol, buffer2);
    }
}

void server_destroy(server_t *self) {
    if (!self) return;
    protocol_destroy(self->protocol);
    free(self->protocol);
}