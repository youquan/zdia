#ifndef _SERVER_H_
#define _SERVER_H_

#include "conn.h"
#include "worker.h"

typedef struct __server {
    uint16_t        port;       /* port this service binds */
    array_t *       endpoints;  /* interfaces/ips this service binds */
    int             protocol;   /* IP, IPv6, SCTP */

    conn_t *        conn;       /* listening context */

    pthread_t       thread;     /* main thread of this thread */
    int             status;     /* thread is running or stopped */

    dict_t *        dict;       /* diameter dictionary */

    array_t *       receivers;  /* thread each client, number is in conf */
    array_t *       workers;    /* shared by all clients of this service */

    int             curr_worker;/* how incoming messagea are routed to workers */
} server_t;


server_t *server_new();
void      server_free(server_t *server);
void      server_init(server_t *server);
#define   SERVER_INITIALIZER \
          {.conn = NULL, .endpoints = NULL, .protocol = AF_INET, .workers = NULL}


int server_start(server_t *server);
int server_stop(server_t *server);

int server_dispatch(server_t *server, msg_t *msg);

#endif
