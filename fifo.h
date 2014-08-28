#ifndef _FIFO_H_
#define _FIFO_H_

#include <pthread.h>

typedef struct __fifo *fifo_t;

fifo_t  fifo_new(size_t elem_size, size_t capacity);
void    fifo_free(fifo_t f);

size_t  fifo_size(const fifo_t f);
size_t  fifo_capacity(const fifo_t f);
int     fifo_empty(const fifo_t f);
int     fifo_full(const fifo_t f);
int     fifo_resize(fifo_t f, size_t n);

int     fifo_push(fifo_t f, const void *elem);
int     fifo_pop(fifo_t f, void *elem);

int     fifo_swap(fifo_t f1, fifo_t f2);
int     fifo_clear(fifo_t f);
fifo_t  fifo_clone(const fifo_t f);


#endif
