#include <stdio.h>
#include "server.h"
#include <netdb.h>

int main(int argc, char *argv[]) {

	if (argc != 2) {
    	fprintf(stderr, "Uso: ./server <puerto> (Ej: ./server 8080)\n");
    	return 1;
    }
    server_t server;
    if (!server_init(&server, argv[1])) return 1;
    server_run(&server);
    server_destroy(&server);
    return 0;
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
        char buffer[120] = {0};
        if (protocol_server_receive(self->protocol, buffer) <= 0) break; //Termino si no recibo nada mas del cliente
        printf("*******RECEIVED MESSAGE***********\n");
        printf("MESSAGE: %s", buffer);
        char *buffer2 = "Mund\n";
        fflush(stdin);
        protocol_server_send(self->protocol, buffer2);
    }
}

void server_destroy(server_t *self) {
    if (!self) return;
    protocol_destroy(self->protocol);
    free(self->protocol);
}