#ifndef _EVENT_H_
#define _EVENT_H_

#include "msg.h"
#include "ind.h"

enum {
    EVENT_MSG,
    EVENT_IND,
    EVENT_MAX
};

typedef struct {
    int         type;
    union {
        msg_t * msg;
        ind_t * ind;
    } data;
} event_t;

#endif
