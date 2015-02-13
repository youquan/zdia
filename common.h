#ifndef _COMMON_H_
#define _COMMON_H_

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"

/* memory */
typedef void *(*zd_alloc_func)(size_t);
typedef void *(*zd_realloc_func)(void *ptr, size_t);
typedef void  (*zd_free_func)(void *);

void  zd_set_alloc(zd_alloc_func f);
void  zd_set_free(zd_free_func f);

void *zd_malloc(size_t n);
void *zd_realloc(void *ptr, size_t n);
void  zd_free(void *ptr);

/* check return value */
#define CHECK_INT_DO(__call, __condition, __fallback) \
        int __ret = (__call); \
        if (__ret __condition) { \
            LOG_ERROR("Fail to call %s: return %d.", #__call, __ret); \
            __fallback; \
        }

#define CHECK_PTR_DO(__call, __condition, __fallback) \
        void *__ret = (void *)(__call); \
        if (__ret __condition) { \
            LOG_ERROR("Fail to call %s: return %d.", #__call, __ret); \
            __fallback; \
        }

#define CHECK_SYS_DO(__call, __callback) \
        int __ret = (__call); \
        if (__ret < 0) { \
            int __error = errno; \
            LOG_ERROR("Fail to call %s: %s.", #__call, strerror(__error)); \
            __fallback; \
        }

#endif
