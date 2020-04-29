#define _POSIX_C_SOURCE 200112L
#include "socket.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

void socket_init(socket_t *self, int fd) {
    self->fd = fd;
}


static struct addrinfo *_get_addrinfo(socket_t *self,
                          const char *host, const char *service, int flags) {
    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = flags;
    int addr_err = getaddrinfo(host, service, &hints, &result);
    if (addr_err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addr_err));
        return NULL;
    }
    return result;
}

static int _bind_or_accept(struct addrinfo *result, int (*f)(int , const struct sockaddr *,
                socklen_t )){

    /* Itera los resultados de _get_addrfinfo y aplica la funcion _bind o _connect a cada uno de los resultados.
    Se debe pasar _bind o _connect como parametro f

    Retorna -1 en caso de no poder bindear ni conectar e imrpime en stderr, si no, se devuelve
    el resultado positivo de accept o bind  */

    int sfd;
    struct addrinfo *rp;
    //Itero la lista de resultados posibles
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }
        if ((*f)(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break; /* Success */   
        }
        close(sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return -1;
    }
    return sfd;
}

int socket_connect(socket_t *self, const char *host, const char *service){
    /*Devuelve 0 en caso de exito, 1 en caso de error (Y queda en manos del usuario del
    TDA liberar a self) */

    struct addrinfo *result;
    result = _get_addrinfo(self, host, service, 0);
    int sfd = _bind_or_accept(result, connect);

    self->fd = sfd;
    freeaddrinfo(result); 
    return 0;

}

int socket_bind_and_listen(socket_t *self, const char *service) {

    struct addrinfo *result;
    result = _get_addrinfo(self, NULL, service, AI_PASSIVE);
    int sfd = _bind_or_accept(result, bind);
    self->fd = sfd;
    freeaddrinfo(result);
    return listen(self->fd, ACCEPT_QUEUE_LEN);

}

int socket_accept(socket_t *self) {
    char addressBuf[INET_ADDRSTRLEN];
    struct sockaddr_in address;
    socklen_t addressLength = (socklen_t) sizeof(address);
    int newsockfd = accept(self->fd, (struct sockaddr *)&address, &addressLength);
    int val = 1;
    setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (newsockfd < 0) {
        return 1;
    }
    inet_ntop(AF_INET, &(address.sin_addr), addressBuf, INET_ADDRSTRLEN);
    return newsockfd;
}

ssize_t socket_send(socket_t *self, const void *message, size_t length) {
    if (length == 0) return 0;
    int remaining_bytes = length;
    int bytes_sent = 0;
    ssize_t bytes_rcvd = 0; //los bytes que recibe el cliente
    const char *send_message = message;
    while (bytes_sent < length) {
        bytes_rcvd = send(self->fd, &send_message[bytes_sent],
                     remaining_bytes, MSG_NOSIGNAL);
        if (bytes_rcvd == -1) {
            bytes_sent = -1;
            fprintf(stderr, "sending bytes error: %s\n", strerror(errno));
            break;
        }
        if (bytes_rcvd == 0) break;
        bytes_sent += bytes_rcvd;
        remaining_bytes -= bytes_rcvd;
    }
    return bytes_sent;
}



ssize_t socket_receive(socket_t *self, void *message, size_t length){
    if (length == 0) return 0;
    int remaining_bytes = length;
    int bytes_rcvd = 0;
    ssize_t last_bytes_rcvd = 0;
    char *recv_message = message;
    while (bytes_rcvd < length) {
        last_bytes_rcvd = recv(self->fd, &recv_message[bytes_rcvd],
                     remaining_bytes, 0);
        if (last_bytes_rcvd == -1) {
            bytes_rcvd = -1;
            fprintf(stderr, "socket_receive failed at recv: %s\n", strerror(errno));
            break;
        }
        if (last_bytes_rcvd == 0) break;
        bytes_rcvd += last_bytes_rcvd;
        remaining_bytes -= last_bytes_rcvd;
    }
    return bytes_rcvd;
}

void socket_release(socket_t *self) {
    if (!self) return;
    if(!self->fd) return;
    if(self->fd == -1) return;
    if (shutdown(self->fd, SHUT_RDWR) == -1) {
        fprintf(stderr, "socket_destroy failed at shutdown: %s. FD is %d\n", strerror(errno), self->fd);
    }
    if (close(self->fd) == -1) {
        fprintf(stderr, "socket_destroy failed at close: %s\n", strerror(errno));
    }
}