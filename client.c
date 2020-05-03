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
    client_run(&client);
    client_destroy(&client);
    return 0;
}

bool client_init(client_t *self, char *host, char *port, char *filename) {
    protocol_t *protocol = malloc(sizeof(protocol_t));
    if (!protocol || !protocol_client_init(protocol, host, port)) {
        free(protocol);
        return false;
    }
    input_reader_t *input_reader = malloc(sizeof(input_reader_t));
    if (!input_reader || !input_reader_init(input_reader, filename)) {
        free(input_reader);
        protocol_destroy(protocol);
        free(protocol);
        return false;
    }
    self->protocol = protocol;
    self->input_reader = input_reader;
    return true;
}

void client_run(client_t *self) {
	char *buffer = {0};
    while (true) {
        /* TO DO: Buffer deberia ser dinamico ya que al leer lineas nunca sabemos hasta donde lleguen */
        /*MALLOC Y REALLOC*/

        // char buffer[MAX_INPUT_SIZE] = {0};
        // if (!fgets(buffer, MAX_INPUT_SIZE, self->input)) break;
    	buffer = input_reader_get_next_line(self->input_reader);
    	if (buffer == NULL) return;
        printf("\nNEW LINE: %s\n", buffer);
        /* TO DO: Antes de enviar, procesar lo recibido y armar la cadena correspondiente segun al enunciado */
        ssize_t send = protocol_client_send(self->protocol, buffer);
        printf("SENTD %ld BYTES\n", send);
        if (send == -1) { 
            free(buffer);
            break;
        }
        else if (send == 0) continue;
        char output[MAX_INPUT_SIZE] = {0};
        ssize_t received = protocol_client_receive(self->protocol, output);
        if (received <= 0) break;
        printf("%s", output);
        fflush(stdin);
    	free(buffer);
    }

}


void client_destroy(client_t *self) {
    if (!self) return;
    protocol_destroy(self->protocol);
    input_reader_destroy(self->input_reader);
    free(self->input_reader);
    free(self->protocol);
}