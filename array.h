#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <stddef.h>

typedef struct {
    size_t          elem_size;  /* element size */
    size_t          capacity;   /* total element number */

    size_t          size;       /* current element number  */

    int             (*cmp)(const void *, const void *); /* compare function for sorting */

    void *          data;       /* data */
} array_t;

#define ARRAY_INITIAL_SIZE 4

array_t *   array_new(size_t elem_size);
void        array_free(array_t *arr);
void        array_free_data(array_t *arr);
void        array_init(array_t *arr, size_t elem_size);
#define     ARRAY_INITIALIZER \
            {.elem_size = sizeof(int), capacity = ARRAY_INITIAL_SIZE, size = 0, .data = NULL}

void    array_set_cmp(array_t *arr, int (*cmp)(const void *, const void *));
void    array_sort(array_t *arr);

int     array_reserve(array_t *arr, size_t n);
int     array_resize(array_t *arr, size_t n);

void *array_at(const array_t *arr, size_t pos);
void *array_front(const array_t *arr);
void *array_back(const array_t *arr);
void *array_begin(const array_t *arr);
void *array_end(const array_t *arr);
void *array_next(const array_t *arr, void *curr);
void *array_find(array_t *arr, const void *key);

int     array_push_back(array_t *arr, const void *elem);
int     array_pop_back(array_t *arr);
int     array_insert(array_t *arr, void *pos, const void *elem);
int     array_insert_at(array_t *arr, size_t pos, const void *elem);
int     array_erase(array_t *arr, void *pos);
int     array_erase_at(array_t *arr, size_t pos);
int     array_swap(array_t *arr1, array_t *arr2);
int     array_clear(array_t *arr);

array_t *array_clone(const array_t *arr);

#endif
