#ifndef _SERVER_H_
#define _SERVER_H_

#include "conn.h"
#include "worker.h"

typedef struct {
    list_t          list;

    uint16_t        port;
    list_t          endpoints;
    int             protocol;

    conn_t *        conn;       /* listening */

    pthread_t       thread;
    int             status;

    list_t          receivers;  /* thread each client, number is in conf */
} server_t;


server_t *server_new();
void      server_free(server_t *server);
void      server_init(server_t *server);
#define   SERVER_INITIALIZER \
          {.conn = NULL, .endpoints = NULL, .protocol = AF_INET, .workers = NULL}


int server_start(server_t *server);
int server_stop(server_t *server);

#endif
