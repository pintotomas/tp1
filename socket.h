#ifndef TP1_SOCKET_H
#define TP1_SOCKET_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>

#define SOCK_FAMILY AF_INET
#define SOCK_TYPE SOCK_STREAM
#define SOCK_SERVER_FLAGS AI_PASSIVE
#define SOCK_CLIENT_FLAGS 0
#define ACCEPT_QUEUE_LEN 10

typedef struct {
    int fd;
} socket_t;

void socket_init(socket_t *self, int fd);

//bool socket_connect(socket_t *self, const char *host, const char *service);

int socket_bind_and_listen(socket_t *self, const char *service, int flags);

int socket_accept(socket_t *self);

ssize_t socket_send(socket_t *self, const void *message, size_t length);

ssize_t socket_receive(socket_t *self, void *response, size_t length);

void socket_release(socket_t *self);


#endif //TP1_SOCKET_H