#ifndef _CONN_H_
#define _CONN_H_

#include <netinet/in.h>
#include "list.h"

typedef struct {
    int         sock;

    int         family;     /* AF_INET or AF_INET6 */
    int         protocol;   /* IPPROTO_TCP or IPPROTO_SCTP */

    /* tls */

    /* sctp */
} conn_t;

typedef struct {
    list_t                  list;
    struct sockaddr_storage addr;
} endpoint_t;

conn_t *conn_new();
void    conn_free(conn_t *conn);

conn_t *conn_serv_bind(int protocol, uint16_t port, list_t *ep); 
int     conn_serv_listen(conn_t *conn);
conn_t *conn_serv_accept(conn_t *conn);

conn_t *conn_cli_connect(int protocol, uint16_t port, list_t *ep);

int     conn_recv(conn_t *conn, unsigned char *buf, size_t len);
int     conn_send(conn_t *conn, const unsigned char *buf, size_t len);

#define SALEN(sock) \
        ((socklen_t)((((struct sockaddr *)sock)->sa_family == AF_INET)  ? (sizeof(struct sockaddr_in)) : \
                    ((((struct sockaddr *)sock)->sa_family == AF_INET6) ? (sizeof(struct sockaddr_in6)) : \
                    0)))

#endif
