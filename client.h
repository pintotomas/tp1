#ifndef _CLIENT_H
#define _CLIENT_H

#include "protocol.h"
#include <stdbool.h>


typedef struct {
    protocol_t *protocol;
} client_t;


int client_main(int argc, char *argv[]);

// Devuelve falso si falla la inicializacion del struct client_t
bool client_init(client_t *self, char *host, char *port);

//Interaccion con el server
void client_run(client_t *self);

// Se liberan los recursos asociados al cliente
void client_destroy(client_t *self);

#endif //_CLIENT_H