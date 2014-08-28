#ifndef _MSG_H_
#define _MSG_H_

#include <stdint.h>
#include <stdlib.h>
#include "list.h"
#include "dict.h"

enum {
    CMD_FLAG_REQUEST,
    CMD_FLAG_PROXIABLE,
    CMD_FLAG_ERROR,
    CMD_FLAG_RETRANSMITTED
};

typedef struct {
    uint8_t         version;
    uint32_t        msg_len;
    uint8_t         cmd_flags;
    cmd_code_t      cmd_code;
    app_id_t        app_id;
    uint32_t        hop_by_hop_id;
    uint32_t        end_to_end_id;
} msg_header_t;


#define DIAMETER_VERSION 1
#define MSG_HEADER_SIZE 20

enum {
    MSG_PARSED_NONE,        /* not parsed */
    MSG_PARSED_HEADER,      /* only header parsed */
    MSG_PARSED_ALL          /* header and all avps are parsed */
};

typedef struct {
    msg_header_t        header;
    int                 parsed;
    list_t              avps;

    /* always 4 octets aligned */
    const uint32_t *    raw_data;
} msg_t;

msg_t *msg_new(size_t n);
msg_t *msg_new_from(uint32_t *buf);
void   msg_free(msg_t *msg);

int    msg_parse_header(msg_t *msg);
int    msg_parse_all(msg_t *msg, const dict_t *dict);

//int    msg_add_avp(msg_t *msg, const avp_t *avp);
//int    msg_add_avp_raw(msg_t *msg, avp_code_t code, const char *val);

#endif
