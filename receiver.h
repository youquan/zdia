#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include <pthread.h>

#include "list.h"
#include "conn.h"
#include "worker.h"

typedef struct {
    list_t          list;

    int             id;
    pthread_t       thread;

    conn_t *        conn;       /* data connection */

    worker_t **     workers;        /* workers per receiver */
    size_t          nworker;        /* worker number per receiver */
    size_t          next_worker;    /* next worker to receive event */
} receiver_t;


receiver_t *receiver_new(conn_t *conn, size_t nworker);
void        receiver_free(receiver_t *receiver);

#endif
