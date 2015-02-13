#include "common.h"
#include "array.h"

static int __array_grow(array_t *arr) {
    static size_t array_grow_factor = 2;

    void *data = zd_realloc(arr->data, array_grow_factor * arr->elem_size * arr->capacity);

    arr->data = data;
    arr->capacity *= array_grow_factor;

    return 0;
}

array_t *array_new(size_t elem_size) {
    array_t *arr = (array_t *)zd_malloc(sizeof(array_t));
    arr->data    = zd_malloc(elem_size * ARRAY_INITIAL_SIZE);

    arr->elem_size = elem_size;
    arr->capacity = ARRAY_INITIAL_SIZE;
    arr->size = 0;
    arr->cmp = NULL;

    return arr;
}

void array_free(array_t *arr) {
    if (arr) {
        zd_free(arr->data);
        zd_free(arr);
    }
}

void array_free_data(array_t *arr) {
    if (arr && arr->data) {
        zd_free(arr->data);
    }
}

void array_init(array_t *arr, size_t elem_size) {
    if (arr->data) zd_free(arr->data);

    arr->data      = zd_malloc(elem_size * ARRAY_INITIAL_SIZE);
    arr->elem_size = elem_size;
    arr->capacity  = ARRAY_INITIAL_SIZE;
    arr->size      = 0;
    arr->cmp       = NULL;
}

void array_set_cmp(array_t *arr, int (*cmp)(const void *, const void *)) {
    arr->cmp = cmp;
}

void array_sort(array_t *arr) {
    qsort(arr->data, arr->size, arr->elem_size, arr->cmp);
}

int array_reserve(array_t *arr, size_t n) {
    return 0;
}

int array_resize(array_t *arr, size_t n) {
    return 0;
}

void *array_at(const array_t *arr, size_t pos) {
    if (pos >= arr->size) return NULL;

    return (char *)arr->data + arr->elem_size * pos;
}

void *array_front(const array_t *arr) {
    return arr->data;
}

void *array_back(const array_t *arr) {
    return (char *)arr->data + arr->elem_size * (arr->size - 1);
}

void *array_begin(const array_t *arr) {
    return arr->data;
}

void *array_end(const array_t *arr) {
    return (char *)arr->data + arr->elem_size * arr->size;
}

void *array_next(const array_t *arr, void *iter) {
    return (char *)iter + arr->elem_size;
}

void *array_find(array_t *arr, const void *elem) {
    void *it;

    for (it = array_begin(arr); it != array_end(arr); it = array_next(arr, it)) {
        if (0 == arr->cmp(elem, it)) {
            break;
        }
    }

    return it;
}

int array_push_back(array_t *arr, const void *elem) {
    if ((arr->size == arr->capacity) && __array_grow(arr)) {
        return ENOMEM;
    }

    memcpy((char *)arr->data + arr->size * arr->elem_size, elem, arr->elem_size);
    arr->size++;

    return 0;
}

int array_pop_back(array_t *arr) {
    if (arr->size > 0) {
        arr->size--;
        return 0;
    }

    return ENOENT;
}

int array_insert(array_t *arr, void *pos, const void *elem) {
    if ((arr->size == arr->capacity) && __array_grow(arr)) {
        return ENOMEM;
    }

    memmove((char *)pos + arr->elem_size, pos, array_end(arr) - pos);
    memcpy((char *)pos, elem, arr->elem_size);
    arr->size++;

    return 0;
}

int array_insert_at(array_t *arr, size_t pos, const void *elem) {
    if (pos >= arr->size) {
        return ENOENT;
    }

    return array_insert(arr, (char *)arr->data + arr->elem_size * pos, elem);
}

int array_erase(array_t *arr, void *pos) {
    memmove(pos, (char *)pos + arr->elem_size, array_end(arr) - pos - 1);
    arr->size--;

    return 0;
}

int array_erase_at(array_t *arr, size_t pos) {
    if (pos >= arr->size) {
        return ENOENT;
    }

    return array_erase(arr, (char *)arr->data + arr->elem_size * pos);
}

int array_swap(array_t *arr1, array_t *arr2) {
    return 0;
}

int array_clear(array_t *arr) {
    arr->size = 0;
    return 0;
}

array_t *array_clone(const array_t *arr) {
    array_t *arr2 = array_new(arr->elem_size);
    if (arr2 == NULL) return NULL;

    /* TODO: clone here... */
    return arr2;
}


