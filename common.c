#include "common.h"
#include "log.h"

#include <string.h>
#include <stdlib.h>

static void die(const char *format, ...) {
    //LOG_CRIT(format, );
    exit(1);
}

/************************ memory ***********************/
static zd_alloc_func alloc_func = malloc;
static zd_realloc_func realloc_func = realloc;
static zd_free_func  free_func  = free;

void zd_set_alloc(zd_alloc_func f) {
    alloc_func = f;
}

void zd_set_free(zd_free_func f) {
    free_func = f;
}

void *zd_malloc(size_t n) {
    void *ret = alloc_func(n);

    if (ret == NULL) {
        die("Not enough memory");
    }

    return ret;
}

void *zd_realloc(void *ptr, size_t n) {
    void *ret = realloc_func(ptr, n);

    if (ret == NULL) {
        die("Not enough memory");
    }

    return ret;
}

void zd_free(void *ptr) {
    if (ptr) free_func(ptr);
}
/************************ memory ***********************/

