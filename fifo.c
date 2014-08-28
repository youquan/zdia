#include "common.h"
#include "fifo.h"

struct __fifo {
    pthread_mutex_t     mutex;

    size_t              elem_size;
    size_t              capacity;

    size_t              head;
    size_t              tail;

    void *              data;
};

fifo_t fifo_new(size_t elem_size, size_t capacity) {
    fifo_t f = (fifo_t)md_malloc(sizeof(struct __fifo));
    if (f == NULL) return NULL;

    f->data = md_malloc(elem_size * capacity);
    if (f->data == NULL) {
        md_free(f);
        return NULL;
    }

    f->elem_size = elem_size;
    f->capacity  = capacity;
    f->head = 0;
    f->tail = 0;

    pthread_mutex_init(&f->mutex, NULL);

    return f;
}

void fifo_free(fifo_t f) {
    if (f) {
        pthread_mutex_destroy(&f->mutex);
        md_free(f->data);
        md_free(f);
    }
}

size_t fifo_size(const fifo_t f) {
    return (f->tail - f->head) % f->capacity;
}

size_t fifo_capacity(const fifo_t f) {
    return f->capacity;
}

int fifo_empty(const fifo_t f) {
    return (f->head == f->tail);
}

int fifo_full(const fifo_t f) {
    return ((f->tail + 1) % f->capacity) == f->head;
}

int fifo_resize(fifo_t f, size_t n) {
    /* TODO */
    return 0;
}

int fifo_push(fifo_t f, const void *elem) {
    pthread_mutex_lock(&f->mutex);

    if (fifo_full(f)) {
        pthread_mutex_unlock(&f->mutex);
        return EINVAL;
    }

    memcpy((char *)f->data + f->elem_size * f->tail, elem, f->elem_size);
    f->tail++;
    if (f->tail == f->capacity) f->tail = 0;

    pthread_mutex_unlock(&f->mutex);

    return 0;
}

int fifo_pop(fifo_t f, void *elem) {
    pthread_mutex_lock(&f->mutex);

    if (fifo_empty(f)) {
        pthread_mutex_unlock(&f->mutex);
        return ENOENT;
    }

    memcpy(elem, (char *)f->data + f->elem_size * f->head, f->elem_size);
    f->head++;
    if (f->head == f->capacity) f->head = 0;

    pthread_mutex_unlock(&f->mutex);

    return 0;
}

