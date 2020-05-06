#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "socket.h"
#include "dbus.h"
#include <sys/types.h>

typedef struct {
    socket_t *server_socket;
    socket_t *client_socket;
} protocol_t;

// Devuelve false si falla la inicializacion
// Se conecta al servidor en el puerto 'port' del host 'host'
bool protocol_client_init(protocol_t *self, const char *host, const char *port);

// Devuelve false si falla la inicializacion
// Hace bind and listen
bool protocol_server_init(protocol_t *self, const char *port);

bool protocol_server_accept(protocol_t *self);

// Se envia el mensaje message de cliente al server
// Se devuelve la cantidad de bytes enviados
ssize_t protocol_client_send(protocol_t *self, char *buffer, int id_mensaje);

// Se recibe un mensaje del server al cliente
// Se devuelve la cantidad de bytes recibidos
ssize_t protocol_client_receive(protocol_t *self, char *buffer, int bytes);

// Devuelve el proximo mensaje del cliente decodificado 
// El mensaje devuelto debe ser destruido luego de su utilizacion
// por el usuario
dbus_message_t* protocol_server_receive(protocol_t *self);

ssize_t protocol_server_send(protocol_t *self, char *buffer, int size);

void protocol_destroy(protocol_t *self);
#endif //_PROTOCOL_H
