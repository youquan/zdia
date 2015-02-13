#include <arpa/inet.h>

#include "common.h"
#include "avp.h"
#include "msg.h"

msg_t *msg_new(size_t n) {
    msg_t *msg = (msg_t *)zd_malloc(sizeof(msg_t));

    memset(&msg->header, 0, sizeof(msg->header));
    msg->raw_data = NULL;

    if (n > 0) {
        msg->raw_data = zd_malloc(n);

        msg->header.version = DIAMETER_VERSION;
        msg->header.msg_len = n;
    }

    list_init(&msg->avps);

    return msg;
}

msg_t *msg_new_from(uint32_t *buf) {
    if (buf == NULL) return NULL;

    msg_t *msg = (msg_t *)zd_malloc(sizeof(msg_t));
    if (msg == NULL) return NULL;

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

    return msg;
}

void msg_free(msg_t *msg) {
    if (!msg) return;

    if (msg->raw_data) zd_free((void *)msg->raw_data);
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

int msg_parse_all(msg_t *msg, const dict_t *dict) {
    //uint32_t value;
    uint32_t pos = 0;
    //const dict_cmd_t *cmd;

    if (msg->parsed == MSG_PARSED_NONE) {
        msg_parse_header(msg);
    }
    pos = MSG_HEADER_SIZE;

    avp_t *avp = avp_new_from(&msg->raw_data[pos]);
    avp_decode(avp, dict);
    pos += (avp->len + 3) / 4;

    msg->parsed = MSG_PARSED_ALL;

    return 0;
}

