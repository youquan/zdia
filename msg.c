#include <arpa/inet.h>

#include "common.h"
#include "avp.h"
#include "msg.h"

msg_t *msg_new(size_t n) {
    msg_t *msg = (msg_t *)zd_malloc(sizeof(msg_t));

    memset(&msg->header, 0, sizeof(msg->header));
    msg->avps     = NULL;
    msg->raw_data = NULL;

    /* n == 0 used for encoding/outgoing message */
    if (n > 0) {
        msg->raw_data = zd_malloc(n);

        msg->header.version = DIAMETER_VERSION;
        msg->header.msg_len = n;
    }

    msg->avps = array_new(sizeof(avp_t *));

    return msg;
}

msg_t *msg_new_from(uint32_t *buf) {
    if (buf == NULL) return NULL;

    msg_t *msg = (msg_t *)zd_malloc(sizeof(msg_t));

    memset(&msg->header, 0, sizeof(msg->header));
    msg->raw_data = NULL;

    uint32_t value = ntohl(*(uint32_t *)&buf[0]);
    int version = value >> 24;
    int msg_len = value & 0xffffff;

    if (buf[0] != DIAMETER_VERSION) {
        LOG_ERROR("invalid raw data [version: %d, size %d].", version, msg_len);
        msg_free(msg);
        return NULL;
    }

    msg->raw_data = buf;
    msg->avps = array_new(sizeof(avp_t *));

    return msg;
}

void msg_free(msg_t *msg) {
    if (!msg) return;

    if (msg->raw_data) zd_free((void *)msg->raw_data);
    if (msg->avps) {
        int i;
        avp_t **avp = (avp_t **)msg->avps->data;
        for (i = 0; i < msg->avps->size; i++) {
            avp_free(avp[i]);
        }
        array_free(msg->avps);
    }

    zd_free(msg);
}

int msg_parse_header(msg_t *msg) {
    uint32_t value;
    uint32_t pos = 0;

    value = ntohl(msg->raw_data[pos]);
    msg->header.version = (uint8_t)(value >> 24);
    msg->header.msg_len = value & 0xffffff;
    pos++;

    value = ntohl(msg->raw_data[pos]);
    msg->header.cmd_flags = (uint8_t)(value >> 24);
    msg->header.cmd_code = value & 0xffffff;
    pos++;

    msg->header.app_id = ntohl(msg->raw_data[pos]);
    pos++;

    msg->header.hop_by_hop_id = ntohl(msg->raw_data[pos]);
    pos++;

    msg->header.end_to_end_id = ntohl(msg->raw_data[pos]);
    pos++;

    msg->parsed = MSG_PARSED_HEADER;

    return 0;
}

int msg_parse_all(msg_t *msg) {
    //uint32_t value;
    uint32_t pos = 0;
    //const dict_cmd_t *cmd;

    if (msg->parsed == MSG_PARSED_NONE) {
        msg_parse_header(msg);
    }
    pos = MSG_HEADER_SIZE / DIAMETER_ALIGN;

    while (pos * DIAMETER_ALIGN < msg->header.msg_len) {
        avp_t *avp = avp_new_from(&msg->raw_data[pos]);
        avp->dict_avp = dict_get_avp(msg->dict, avp->code, avp->vendor_id);

        if (avp->dict_avp == NULL) {
            LOG_WARN("cannot decode avp [code: %u; vendor-id: %u], please add information in dictinory.", avp->code, avp->vendor_id);
        } else {
            avp_decode(avp);
            array_push_back(msg->avps, &avp);
        }
        pos += (avp->len + DIAMETER_ALIGN - 1) / DIAMETER_ALIGN;
    }

    msg->parsed = MSG_PARSED_ALL;

    return 0;
}

