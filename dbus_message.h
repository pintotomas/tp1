#ifndef _DBUS_MESSAGE_H
#define _DBUS_MESSAGE_H
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

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
} dbus_message_t;


//Devuelve la cantidad de bytes restantes a leer del header+body
//recibe un unsigned char de 16 bytes con informacion de header+body
void dbus_message_destroy(dbus_message_t *self);

void dbus_message_str_representation(dbus_message_t *self);

//Decodifica el mensaje binario siguiendo el protocolo dbus
//Devuelve un struct dbus_message_t que el usuario debe destruir 
//Luego de utilizarlo
void dbus_message_str_representation(dbus_message_t *self);


#endif