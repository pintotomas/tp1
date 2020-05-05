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

static void _print_client_response(char *msg, int id) {

    unsigned char bytes[4];
    bytes[0] = (id >> 24) & 0xFF;
    bytes[1] = (id >> 16) & 0xFF;
    bytes[2] = (id >> 8) & 0xFF;
    bytes[3] = id & 0xFF;
    printf("* Id: 0x%02x%02x%02x%02x: %s", bytes[0], bytes[1], bytes[2], bytes[3], msg);

}

void client_run(client_t *self) {
	char *buffer = {0};
    int id_mensaje = 1;
    while (true) {

        if (buffer != NULL) free(buffer);
    	buffer = input_reader_get_next_line(self->input_reader);
    	if (buffer == NULL) return;
        ssize_t send = protocol_client_send(self->protocol, buffer, id_mensaje);
        id_mensaje++;
        
        if (send == -1) { 
            free(buffer);
            break;
        }
        else if (send == 0) continue;
        char response[3] = {0};
        ssize_t rcvd_bytes = protocol_client_receive(self->protocol, response, 3);
        if (rcvd_bytes <= 0) {
            fprintf(stderr, "Server closed/down\n");
        }
        _print_client_response(response, id_mensaje);
    }

}


void client_destroy(client_t *self) {
    if (!self) return;
    protocol_destroy(self->protocol);
    input_reader_destroy(self->input_reader);
    free(self->input_reader);
    free(self->protocol);
}