#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include <pthread.h>

#include "list.h"
#include "conn.h"
#include "worker.h"
#include "server.h"

typedef struct {
    list_t          list;

    int             id;
    pthread_t       thread;

    conn_t *        conn;           /* data connection */

    server_t *      server;
} receiver_t;


receiver_t *receiver_new(server_t *server, conn_t *conn);
void        receiver_free(receiver_t *receiver);

#endif
