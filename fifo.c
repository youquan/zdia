#include "common.h"
#include "fifo.h"

fifo_t *fifo_new(size_t size) {
    fifo_t *f = (fifo_t *)md_malloc(sizeof(fifo_t));
    if (f == NULL) return NULL;

    f->events = md_malloc(size * sizeof(event_t));
    if (f->events == NULL) {
        md_free(f);
        return NULL;
    }

    f->size = size;
    f->head = 0;
    f->tail = 0;

    pthread_mutex_init(&f->mutex, NULL);

    return f;
}

void fifo_free(fifo_t *f) {
    if (f) {
        pthread_mutex_destroy(&f->mutex);
        md_free(f->events);
        md_free(f);
    }
}

int fifo_empty(fifo_t *f) {
    return (f->head == f->tail);
}

int fifo_full(fifo_t *f) {
    return (((f->tail + 1) % f->size) == f->head);
}

int fifo_count(fifo_t *f) {
    return (f->tail - f->head) % f->size;
}

int fifo_push(fifo_t *f, const event_t *ev) {
    pthread_mutex_lock(&f->mutex);

    if (fifo_full(f)) {
        pthread_mutex_unlock(&f->mutex);
        return EINVAL;
    }

    f->events[f->tail++] = *ev;
    if (f->tail == f->size) f->tail = 0;

    pthread_mutex_unlock(&f->mutex);

    return 0;
}

int fifo_pop(fifo_t *f, event_t *ev) {
    pthread_mutex_lock(&f->mutex);

    if (fifo_empty(f)) {
        ev->data.msg = NULL;
        pthread_mutex_unlock(&f->mutex);
        return ENOENT;
    }

    *ev = f->events[f->head++];
    if (f->head == f->size) f->head = 0;

    pthread_mutex_unlock(&f->mutex);

    return 0;
}

