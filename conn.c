#include <malloc.h>
#include <unistd.h>

#include "conn.h"
#include "tcp.h"
#include "common.h"

conn_t *conn_new() {
    conn_t *conn = (conn_t *)md_malloc(sizeof(conn_t));
    return conn;
}

void conn_free(conn_t *conn) {
    if (conn && conn->sock) {
        shutdown(conn->sock, SHUT_RDWR);
        close(conn->sock);
    }

    md_free(conn);
}

conn_t *conn_serv_bind(int protocol, uint16_t port, list_t *ep) {
    conn_t *conn;
    
    if ((conn = conn_new()) == NULL) {
        LOG_ERROR("fail to create conn_t object.");
        return NULL;
    }

    conn->protocol = protocol;
    if (protocol == IPPROTO_TCP) {
        if (list_empty(ep)) {
            return NULL;    /* or bind to INADDR_ANY */
        } else {
            endpoint_t *host = (endpoint_t *)ep->next;
            conn->family = host->addr.ss_family;

            socklen_t len = SALEN(&host->addr);
            if (tcp_create_bind(&conn->sock, (struct sockaddr *)&host->addr, len) < 0) {
                return NULL;
            }
        }
    }

    return conn;
}

int conn_serv_listen(conn_t *conn) {
    if (listen(conn->sock, 5) < 0) {
        LOG_ERROR("fail to listen on socket %d: %s.", conn->sock, strerror(errno));
        return -1;
    }

    return 0;
}

conn_t *conn_serv_accept(conn_t *conn) {
    int cli_sock;
    struct sockaddr_storage cli_sa;
    socklen_t len = sizeof(cli_sa);
    conn_t *cli_conn;

    if((cli_sock = accept(conn->sock, (struct sockaddr *)&cli_sa, &len)) < 0) {
        LOG_ERROR("fail to accept new connection: %s.", strerror(errno));
        return NULL;
    }

    if((cli_conn = conn_new()) == NULL) {
        LOG_ERROR("fail to create conn_t object.");
        shutdown(cli_sock, SHUT_RDWR);
        close(cli_sock);
        return NULL;
    }

    cli_conn->sock     = cli_sock;
    cli_conn->family   = conn->family;
    cli_conn->protocol = conn->protocol;

    return cli_conn;
}

conn_t *conn_cli_connect(int protocol, uint16_t port, list_t *ep) {
    return NULL;
}

int conn_recv(conn_t *conn, unsigned char *buf, size_t len) {
    int num = recv(conn->sock, buf, len, 0);
    return num;
}

int conn_send(conn_t *conn, const unsigned char *buf, size_t len) {
    int num = send(conn->sock, buf, len, 0);
    return num;
}

