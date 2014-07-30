#ifndef _LIST_H_
#define _LIST_H_

typedef struct dlist {
    struct dlist *  next;
    struct dlist *  prev;
} list_t;


typedef struct {
} array_t;


typedef list_t queue_t;

list_t *list_new();
void list_init(list_t *l);
void list_free(list_t *l);

void list_add_head(list_t *l, list_t *elem);
void list_add_tail(list_t *l, list_t *elem);
#define list_append(l, elem) list_add_tail(l, elem)

void list_del(list_t *elem);

int  list_empty(list_t *l);

#endif
