#include "queue.h"

queue_t *queue_new(size_t capacity, size_t size) {
    queue_t *q = (queue_t *)md_malloc(sizeof(queue_t));
    if (q == NULL) return NULL;

    q->data = md_malloc(capacity * size);
    if (q->data == NULL) {
        md_free(q);
        return NULL;
    }

    q->capacity = capacity;
    q->size = size;
    q->count = 0;
    q->head = 0;
    q->tail = 0;

    return q;
}

void queue_free(queue_t *q) {
    if (q) {
        md_free(q->data);
        md_free(q);
    }
}

int queue_push(queue_t *q, void *data) {
    return 0;
}

int queue_pop(queue_t *q, void *data) {
    return 0;
}

const void *queue_front(const queue_t *q) {
    return NULL;
}

const void *queue_back(const queue_t *q) {
    return NULL;
}

