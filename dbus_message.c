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
	printf("Received message id: %d\n", self->id_mensaje);
}


