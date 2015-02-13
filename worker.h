#ifndef _WORKER_H_
#define _WORKER_H_

#include "msg.h"
#include "ind.h"
#include "queue.h"

struct __server;
typedef struct {
    int         id;
    pthread_t   thread;
    int         status;

    queue_t *   queue;

    struct __server *   server;
} worker_t;

typedef int (*msg_handler_func)(const msg_t *msg);
typedef int (*ind_handler_func)(const ind_t *ind);

worker_t *worker_new();
void worker_free();

int worker_start(worker_t *worker);
int worker_stop(worker_t *worker);

int worker_push_msg(worker_t *workers, msg_t *msg);
int worker_push_ind(worker_t *workers, ind_t *ind);

#endif
