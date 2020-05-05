#include "dbus_message.h"

void dbus_message_init(dbus_message_t *self) {
	self->ruta = NULL;
    self->destino = NULL;
    self->interfaz = NULL;
    self->metodo = NULL;
    self->parametros = NULL;
}