#include "common.h"
#include "queue.h"

queue_t *queue_new(size_t elem_size, size_t capacity) {
    queue_t *q = (queue_t *)zd_malloc(sizeof(queue_t));

    q->data = zd_malloc(elem_size * capacity);
    if (q->data == NULL) {
        zd_free(q);
        return NULL;
    }

    q->elem_size = elem_size;
    q->capacity  = capacity;
    q->head = 0;
    q->tail = 0;

    pthread_mutex_init(&q->mutex, NULL);

    return q;
}

void queue_free(queue_t *q) {
    if (q) {
        pthread_mutex_destroy(&q->mutex);
        zd_free(q->data);
        zd_free(q);
    }
}

size_t queue_size(const queue_t *q) {
    return (q->tail - q->head) % q->capacity;
}

size_t queue_capacity(const queue_t *q) {
    return q->capacity;
}

int queue_empty(const queue_t *q) {
    return (q->head == q->tail);
}

int queue_full(const queue_t *q) {
    return ((q->tail + 1) % q->capacity) == q->head;
}

int queue_resize(queue_t *q, size_t n) {
    /* TODO */
    return 0;
}

int queue_push(queue_t *q, const void *elem) {
    pthread_mutex_lock(&q->mutex);

    if (queue_full(q)) {
        pthread_mutex_unlock(&q->mutex);
        return EINVAL;
    }

    memcpy((char *)q->data + q->elem_size * q->tail, elem, q->elem_size);
    q->tail++;
    if (q->tail == q->capacity) q->tail = 0;

    pthread_mutex_unlock(&q->mutex);

    return 0;
}

int queue_pop(queue_t *q, void *elem) {
    pthread_mutex_lock(&q->mutex);

    if (queue_empty(q)) {
        pthread_mutex_unlock(&q->mutex);
        return ENOENT;
    }

    memcpy(elem, (char *)q->data + q->elem_size * q->head, q->elem_size);
    q->head++;
    if (q->head == q->capacity) q->head = 0;

    pthread_mutex_unlock(&q->mutex);

    return 0;
}

