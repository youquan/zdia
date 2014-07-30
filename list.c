#include <pthread.h>
#include <malloc.h>
#include "list.h"

list_t *list_new() {
    list_t *l = (list_t *)malloc(sizeof(list_t));
    list_init(l);
    return l;
}

void list_free(list_t *l) {
    if (l) free(l);
}

void list_init(list_t *l) {
    l->next = l->prev = l;
}

int list_empty(list_t *l) {
    return (l == l->next) && (l == l->prev);
}

static void __list_add(list_t *prev, list_t *next, list_t *elem) {
    next->prev = elem;
    elem->next = next;
    elem->prev = prev;
    prev->next = elem;
}

static void __list_del(list_t *prev, list_t *next) {
    next->prev = prev;
    prev->next = next;
}

void list_add_head(list_t *l, list_t *elem) {
    __list_add(l, l->next, elem);
}

void list_add_tail(list_t *l, list_t *elem) {
    __list_add(l->prev, l, elem);
}

void list_del(list_t *elem) {
    __list_del(elem->prev, elem->next);
    elem->prev = elem->next = NULL;
}

