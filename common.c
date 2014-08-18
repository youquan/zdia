#include "common.h"
#include "log.h"

#include <string.h>
#include <stdlib.h>

static void die(const char *format, ...) {
    //LOG_CRIT(format, );
    exit(1);
}

/************************ memory ***********************/
static md_alloc_func alloc_func = malloc;
static md_realloc_func realloc_func = realloc;
static md_free_func  free_func  = free;

void md_set_alloc(md_alloc_func f) {
    alloc_func = f;
}

void md_set_free(md_free_func f) {
    free_func = f;
}

void *md_malloc(size_t n) {
    void *ret = alloc_func(n);

    if (ret == NULL) {
        die("Not enough memory");
    }

    return ret;
}

void *md_realloc(void *ptr, size_t n) {
    void *ret = realloc_func(ptr, n);

    if (ret == NULL) {
        die("Not enough memory");
    }

    return ret;
}

void md_free(void *ptr) {
    if (ptr) free_func(ptr);
}
/************************ memory ***********************/

