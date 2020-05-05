#ifndef _DBUS_H
#define _DBUS_H
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

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
} dbus_encoder_t;

typedef struct {
	unsigned char *encoded_message; //Contiene header+body
	uint32_t header_length;
	uint32_t body_length;
} dbus_decoder_t;


//Devuelve la cantidad de bytes restantes a leer del header+body
//recibe un unsigned char de 16 bytes con informacion de header+body
ssize_t dbus_decoder_set_descriptions(dbus_decoder_t *self, unsigned char *message);

void dbus_decoder_decode(dbus_decoder_t *self, unsigned char *message);

void dbus_decoder_init(dbus_decoder_t *self);

bool dbus_encoder_create_send_message(dbus_encoder_t *self); 

/* line debe ser un puntero no nulo */
void dbus_encoder_init(dbus_encoder_t *self, char *line);

void dbus_encoder_destroy(dbus_encoder_t *self);



#endif