#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef list_t queue_t;

queue_t *queue_new(size_t capacity, size_t size);
void queue_free(queue_t *q);

int queue_push(queue_t *q, void *data);
int queue_pop(queue_t *q, void *data);

const void *queue_front(const queue_t *q);
const void *queue_back(const queue_t *q);

#endif
