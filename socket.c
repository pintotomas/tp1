#define _POSIX_C_SOURCE 200112L
#include "socket.h"
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

void socket_init(socket_t *self, int fd) {
    self->fd = fd;
}

bool socket_connect(socket_t *self, const char *host, const char *service){
    return true;
 }

int socket_bind_and_listen(socket_t *self, const char *service, int flags) {

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* Allow IPv4 */
    hints.ai_socktype = SOCK_STREAM; /* sequenced, reliable, two-way, connection-based byte  streams. */
    hints.ai_flags = flags;
    //int s = getaddrinfo(NULL, argv[1], &hints, &result);
    int s = getaddrinfo(NULL, service, &hints, &result);

    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return 1;
    }
    //Itero la lista de resultados posibles
    int sfd;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break; /* Success */   
        }
        close(sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        return 1;
    }
    self->fd = sfd;
    freeaddrinfo(result);           /* No longer needed */
    return listen(self->fd, ACCEPT_QUEUE_LEN);

}

int socket_accept(socket_t *self) {
    char addressBuf[INET_ADDRSTRLEN];
    struct sockaddr_in address;
    socklen_t addressLength = (socklen_t) sizeof(address);
    int newsockfd = accept(self->fd, (struct sockaddr *)&address, &addressLength);

    if (newsockfd < 0) {
        return 1;
    }
    inet_ntop(AF_INET, &(address.sin_addr), addressBuf, INET_ADDRSTRLEN);
    printf("Se conectó un usuario: %s\n", addressBuf);
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

void socket_destroy(socket_t *self) {
    if (!self) return;
    if (shutdown(self->fd, SHUT_RDWR) == -1) {
        fprintf(stderr, "socket_destroy failed at shutdown: %s\n", strerror(errno));
    }
    if (close(self->fd) == -1) {
        fprintf(stderr, "socket_destroy failed at close: %s\n", strerror(errno));
    }
}