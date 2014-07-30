#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include <avp.h>
#include <msg.h>

typedef struct avp_dict {
    avp_code_t          code;
    vendor_id_t         vendor_id;
    char *              name;
    uint8_t             flags;
    avp_type_e          type;
    struct avp_dict *   sub_avps;
} avp_dict_t;

typedef struct {
    int                 fixed;
    unsigned            min;
    unsigned            max;
    avp_dict_t *        avp_dict;
} avp_dict_;

typedef struct cmd_dict {
    cmd_code_t          code;
    char *              name;
    uint8_t             flags;
    struct avp_dict *   avps;
} cmd_dict_t;

cmd_dict_new();

#endif
