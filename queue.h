#ifndef _FIFO_H_
#define _FIFO_H_

#include <pthread.h>

typedef struct {
    pthread_mutex_t     mutex;

    size_t              elem_size;
    size_t              capacity;

    size_t              head;
    size_t              tail;

    void *              data;
} queue_t;

queue_t *queue_new(size_t elem_size, size_t capacity);
void     queue_free(queue_t *q);

size_t   queue_size(const queue_t *q);
size_t   queue_capacity(const queue_t *q);
int      queue_empty(const queue_t *q);
int      queue_full(const queue_t *q);
int      queue_resize(queue_t *q, size_t n);

int      queue_push(queue_t *q, const void *elem);
int      queue_pop(queue_t *q, void *elem);

int      queue_swap(queue_t *q1, queue_t *q2);
int      queue_clear(queue_t *q);
queue_t  queue_clone(const queue_t *q);


#endif
