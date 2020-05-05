#include "dbus_message.h"

void dbus_message_init(dbus_message_t *self) {
	self->ruta = NULL;
    self->destino = NULL;
    self->interfaz = NULL;
    self->metodo = NULL;
    self->parametros = NULL;
    self->cantidad_parametros = 0;
    self->id_mensaje = 0;

}

void dbus_message_destroy(dbus_message_t *self) {

	if (self->ruta) free(self->ruta);
	if (self->destino) free(self->destino);
	if (self->interfaz) free(self->interfaz);
	if (self->metodo) free(self->metodo);
	if (self->cantidad_parametros > 0) {
		for (int i = 0; i < self->cantidad_parametros; i++) {
			free(self->parametros[i]);
		}
		free(self->parametros);
	}
}

void dbus_message_str_representation(dbus_message_t *self) {
	unsigned char bytes[4];
	bytes[0] = (self->id_mensaje >> 24) & 0xFF;
	bytes[1] = (self->id_mensaje >> 16) & 0xFF;
	bytes[2] = (self->id_mensaje >> 8) & 0xFF;
	bytes[3] = self->id_mensaje & 0xFF;
	printf("* Id: 0x%02x%02x%02x%02x\n", bytes[0], bytes[1], bytes[2], bytes[3]);
	printf("* Destino: %s\n", self->destino);
	printf("* Interfaz: %s\n", self->interfaz);
	printf("* metodo: %s\n", self->metodo);
	if (self->cantidad_parametros > 0) { 
		printf("* Parámetros:\n");
		for (int i = 0; i < self->cantidad_parametros; i++) {
			printf("    * %s\n", self->parametros[i]);
		}
	}
	printf("\n");
}


