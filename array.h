#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <stddef.h>

typedef int (*array_cmp_func)(const void *elem1, const void *elem2);

typedef struct __array *array_t;
typedef void *array_iter_t;

array_t array_new(size_t elem_size);
void    array_free(array_t arr);

int     array_size(const array_t arr);
int     array_capacity(const array_t arr);
int     array_empty(const array_t arr);
int     array_reserve(array_t arr, size_t n);
int     array_resize(array_t arr, size_t n);

array_iter_t    array_at(const array_t arr, size_t pos);
array_iter_t    array_front(const array_t arr);
array_iter_t    array_back(const array_t arr);
array_iter_t    array_begin(const array_t arr);
array_iter_t    array_end(const array_t arr);
array_iter_t    array_next(const array_t arr, array_iter_t iter);

int     array_push_back(array_t arr, const void *elem);
int     array_pop_back(array_t arr);
int     array_insert(array_t arr, array_iter_t pos, const void *elem);
int     array_insert_at(array_t arr, size_t pos, const void *elem);
int     array_erase(array_t arr, array_iter_t pos);
int     array_erase_at(array_t arr, size_t pos);
int     array_swap(array_t arr1, array_t arr2);
int     array_clear(array_t arr);

array_t array_clone(const array_t arr);

#endif
