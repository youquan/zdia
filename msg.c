#include <arpa/inet.h>

#include "common.h"
#include "msg.h"

msg_t *msg_new(size_t n) {
    msg_t *msg = (msg_t *)md_malloc(sizeof(msg_t));
    if (msg == NULL) return NULL;

    memset(&msg->header, 0, sizeof(msg->header));
    msg->raw_data = NULL;

    if (n > 0) {
        if ((msg->raw_data = md_malloc(n)) == NULL) {
            md_free(msg);
            return NULL;
        }

        msg->header.version = DIAMETER_VERSION;
        msg->header.msg_len = n;
    }

    list_init(&msg->avps);

    return msg;
}

msg_t *msg_new_from(uint8_t *buf) {
    if (buf == NULL) return NULL;

    msg_t *msg = (msg_t *)md_malloc(sizeof(msg_t));
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

    if (msg->raw_data) md_free((void *)msg->raw_data);
    md_free(msg);
}

int msg_parse_header(msg_t *msg) {
    uint32_t value;
    uint32_t pos = 0;

    value = ntohl(*(uint32_t *)&msg->raw_data[pos]);
    msg->header.version = (uint8_t)(value >> 24);
    msg->header.msg_len = value & 0xffffff;
    pos += 4;

    value = ntohl(*(uint32_t *)&msg->raw_data[pos]);
    msg->header.cmd_flags = (uint8_t)(value >> 24);
    msg->header.cmd_code = value & 0xffffff;
    pos += 4;

    msg->header.app_id = ntohl(*(uint32_t *)&msg->raw_data[pos]);
    pos += 4;

    msg->header.hop_by_hop_id = ntohl(*(uint32_t *)&msg->raw_data[pos]);
    pos += 4;

    msg->header.end_to_end_id = ntohl(*(uint32_t *)&msg->raw_data[pos]);
    pos += 4;

    msg->parsed = MSG_PARSED_HEADER;

    return 0;
}

int msg_parse_all(msg_t *msg) {
    return 0;
}

