#ifndef _SERVER_H
#define _SERVER_H

#include "protocol.h"
#include <stdbool.h>

typedef struct {
    protocol_t *protocol;
} server_t;

// Finaliza una vez que el primer cliente se conecta con el server
// y envia todo lo que tenga que enviar
int server_main(int argc, char *argv[]);

// Se retorna false si falla algo en la inicializacion
bool server_init(server_t *self, char *port);

// Corre el servidor. Escucha peticiones del cliente.
void server_run(server_t *self);

// Cierra el servidor. Se liberan los recursos utlizados.
void server_destroy(server_t *self);

#endif //_SERVER_H