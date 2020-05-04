#ifndef _DBUS_H
#define _DBUS_H
#include <stdbool.h>
#include <string.h>
/* line es una linea que recibe el cliente
Devuelve la tira de bytes que debe ser enviada al servidor
para que lo procese 

Devuelve NULL si la linea es invalida (No tiene 4 argumentos)*/

// typedef struct {
//     unsigned char *body;
//     int body_length;
// } dbus_t;


unsigned char* create_send_message(char *line, int *body_length); 

#endif