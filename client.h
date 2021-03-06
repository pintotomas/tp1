#ifndef _CLIENT_H
#define _CLIENT_H

#include "protocol.h"
#include "input_reader.h"
#include <stdbool.h>

#define MAX_INPUT_SIZE 5

typedef struct {
    protocol_t *protocol;
    input_reader_t *input_reader;
} client_t;


int main(int argc, char *argv[]);

// Devuelve falso si falla la inicializacion del struct client_t
bool client_init(client_t *self, char *host, char *port, char *filename);

//Interaccion con el server
void client_run(client_t *self);

// Se liberan los recursos asociados al cliente
void client_destroy(client_t *self);

#endif //_CLIENT_H
