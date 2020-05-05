#ifndef _DBUS_MESSAGE_H
#define _DBUS_MESSAGE_H
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

/* line es una linea que recibe el cliente
Devuelve la tira de bytes que debe ser enviada al servidor
para que lo procese 

Devuelve NULL si la linea es invalida (No tiene 4 argumentos)*/

typedef struct {
    char *ruta;
    char *destino;
    char *interfaz;
    char *metodo;
    char **parametros;
    int cantidad_parametros;
    int id_mensaje;

} dbus_message_t;

void dbus_message_init(dbus_message_t *self);

void dbus_message_destroy(dbus_message_t *self);

/* Imprime por stdout la representacion de este mensaje */
void dbus_message_str_representation(dbus_message_t *self);

#endif
