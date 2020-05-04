#ifndef _DBUS_H
#define _DBUS_H
#include <stdbool.h>
#include <string.h>
/* line es una linea que recibe el cliente
Devuelve la tira de bytes que debe ser enviada al servidor
para que lo procese 

Devuelve NULL si la linea es invalida (No tiene 4 argumentos)*/

typedef struct {
    unsigned char *body;
    unsigned char *header;
    char *line_to_encode;
    int header_length; //Contando los ultimos bytes de padding
    int body_length;
} dbus_t;


//unsigned char* dbus_create_send_message(dbus_t *self, char *line, int *body_length); 
//unsigned char* dbus_create_send_message(dbus_t *self, char *line); 

bool dbus_encoder_create_send_message(dbus_t *self); 

/* line debe ser un puntero no nulo */
void dbus_encoder_init(dbus_t *self, char *line);

void dbus_encoder_destroy(dbus_t *self);



#endif