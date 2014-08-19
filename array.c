#include "common.h"
#include "array.h"

struct __array {
    size_t          elem_size;  /* element size */
    size_t          capacity;   /* total element number */

    size_t          size;       /* current element number  */

    array_cmp_func  cmp;        /* compare function for sorting */

    void *          data;       /* data */
};

static const size_t s_array_initial_size = 16;

static int __array_grow(array_t arr) {
    static size_t array_grow_factor = 2;

    void *data = md_realloc(arr->data, array_grow_factor * arr->elem_size * arr->capacity);
    if (data == NULL) return -1;

    arr->data = data;
    arr->capacity *= array_grow_factor;

    return 0;
}

array_t array_new(size_t elem_size) {
    array_t arr = (array_t)md_malloc(sizeof(struct __array));
    if (arr == NULL) return NULL;

    arr->data = md_malloc(elem_size * s_array_initial_size);
    if (arr->data == NULL) {
        md_free(arr);
        return NULL;
    }

    arr->elem_size = elem_size;
    arr->capacity = s_array_initial_size;
    arr->size = 0;
    arr->cmp = NULL;

    return arr;
}

void    array_free(array_t arr) {
    if (arr) {
        md_free(arr->data);
        md_free(arr);
    }
}

int     array_size(const array_t arr) {
    return arr->size;
}

int     array_capacity(const array_t arr) {
    return arr->capacity;
}

int     array_empty(const array_t arr) {
    return arr->size == 0;
}

int     array_reserve(array_t arr, size_t n) {
    return 0;
}

int     array_resize(array_t arr, size_t n) {
    return 0;
}

array_iter_t    array_at(const array_t arr, size_t pos) {
    if (pos >= arr->size) return NULL;

    return (char *)arr->data + arr->elem_size * pos;
}

array_iter_t    array_front(const array_t arr) {
    return arr->data;
}

array_iter_t    array_back(const array_t arr) {
    return (char *)arr->data + arr->elem_size * (arr->size - 1);
}

array_iter_t    array_begin(const array_t arr) {
    return arr->data;
}

array_iter_t    array_end(const array_t arr) {
    return (char *)arr->data + arr->elem_size * arr->size;
}

array_iter_t    array_next(const array_t arr, array_iter_t iter) {
    return (char *)iter + arr->elem_size;
}

int     array_push_back(array_t arr, const void *elem) {
    if ((arr->size == arr->capacity) && __array_grow(arr)) {
        return ENOMEM;
    }

    memcpy((char *)arr->data + arr->size * arr->elem_size, elem, arr->elem_size);
    arr->size++;

    return 0;
}

int     array_pop_back(array_t arr) {
    if (arr->size > 0) {
        arr->size--;
        return 0;
    }

    return ENOENT;
}

int     array_insert(array_t arr, array_iter_t pos, const void *elem) {
    if ((arr->size == arr->capacity) && __array_grow(arr)) {
        return ENOMEM;
    }

    memmove((char *)pos + arr->elem_size, pos, array_end(arr) - pos);
    memcpy((char *)pos, elem, arr->elem_size);
    arr->size++;

    return 0;
}

int     array_insert_at(array_t arr, size_t pos, const void *elem) {
    if (pos >= arr->size) {
        return ENOENT;
    }

    return array_insert(arr, (char *)arr->data + arr->elem_size * pos, elem);
}

int     array_erase(array_t arr, array_iter_t pos) {
    memmove(pos, (char *)pos + arr->elem_size, array_end(arr) - pos - 1);
    arr->size--;

    return 0;
}

int     array_erase_at(array_t arr, size_t pos) {
    if (pos >= arr->size) {
        return ENOENT;
    }

    return array_erase(arr, (char *)arr->data + arr->elem_size * pos);
}

int     array_swap(array_t arr1, array_t arr2) {
    return 0;
}

int     array_clear(array_t arr) {
    arr->size = 0;
    return 0;
}

array_t array_clone(const array_t arr) {
    array_t arr2 = array_new(arr->elem_size);
    if (arr2 == NULL) return NULL;

    /* TODO: clone here... */
    return arr2;
}


