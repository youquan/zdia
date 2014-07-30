#ifndef _FIFO_H_
#define _FIFO_H_

#include <pthread.h>

#include "event.h"

typedef struct {
    pthread_mutex_t     mutex;

    size_t              size;

    size_t              head;
    size_t              tail;

    event_t *           events;
} fifo_t;

fifo_t *fifo_new(size_t size);
void fifo_free(fifo_t *f);

int fifo_empty(fifo_t *f);
int fifo_full(fifo_t *f);
int fifo_count(fifo_t *f);

int fifo_push(fifo_t *f, const event_t *ev);
int fifo_pop(fifo_t *f, event_t *ev);

#endif
