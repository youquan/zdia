#include "common.h"
#include "tcp.h"

int tcp_create_bind(int *sock, struct sockaddr *sa, socklen_t len) {
    if ((*sock = socket(sa->sa_family, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        LOG_ERROR("fail to create tcp socket: %s.", strerror(errno));
        return -1;
    }

    int reuse = 1;
    if (setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        LOG_ERROR("fail to set tcp socket %d option SO_REUSEADDR: %s.", *sock, strerror(errno));
        return -1;
    }

    struct timeval tv = {0, 0};
    tv.tv_usec = 100000L;   /* 100ms */
    if (setsockopt(*sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        LOG_ERROR("fail to set tcp socket %d option SO_RCVTIMEO: %s.", *sock, strerror(errno));
        return -1;
    }

    if (setsockopt(*sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        LOG_ERROR("fail to set tcp socket %d option SO_SNDTIMEO: %s.", *sock, strerror(errno));
        return -1;
    }

    if (bind(*sock, sa, len) < 0) {
        LOG_ERROR("fail to bind tcp socket %d: %s.", *sock, strerror(errno));
        return -1;
    }

    return 0;
}

int tcp_listen(int sock) {
    if (listen(sock, 5) < 0) {
        LOG_ERROR("fail to listen on tcp socket %d: %s.", sock, strerror(errno));
        return -1;
    }

    return 0;
}
