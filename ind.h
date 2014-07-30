#ifndef _IND_H_
#define _IND_H_

enum {
    IND_CONN_UP,
    IND_CONN_DOWN,
    IND_PEER_UP,
    IND_PEER_DOWN,

    IND_TIMEOUT
};

typedef struct {
    int         code;
} ind_t;

#endif
