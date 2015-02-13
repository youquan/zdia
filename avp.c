#include <arpa/inet.h>
#include "avp.h"
#include "common.h"

avp_t *avp_new() {
    avp_t *avp = (avp_t *)zd_malloc(sizeof(avp_t));
    if (avp == NULL) return NULL;

    memset(avp, 0, sizeof(avp_t));

    return avp;
}

avp_t *avp_new_from(const uint32_t *buf) {
    avp_t *avp = (avp_t *)zd_malloc(sizeof(avp_t));
    if (avp == NULL) return NULL;

    memset(avp, 0, sizeof(avp_t));
    avp->raw_data = buf;

    return avp;
}

int avp_decode(avp_t *avp, const dict_t *dict) {
    size_t pos = 0;
    uint32_t value;
    const uint32_t *raw = avp->raw_data;

    /* avp code */
    avp->code = ntohl(raw[pos]);
    pos++;

    /* flags and length */
    value = ntohl(raw[pos]);
    avp->flags = (uint8_t)(value >> 24);
    avp->len   = value & 0xffffff;
    pos++;

    /* vendor id if any */
    if (AVP_IS_FLAG_SET(avp, AVP_FLAG_VENDOR)) {
        avp->vendor_id = ntohl(raw[pos]);
        pos++;
    }

    /* value */
    //const dict_avp_t *da = dict_get_avp(dict, avp->code, avp->vendor_id);
    const dict_avp_t *da = NULL;
    if (da == NULL) {
        LOG_WARN("cannot decode avp [code: %u; vendor-id: %u], please add information in dictinory.", avp->code, avp->vendor_id);
        pos = (avp->len + 3) / 4;
    } else {
        switch (da->type->base) {
            case ABT_GROUPED:
            break;

            case ABT_OCTETSTRING:
            break;

            case ABT_INTEGER32:
            avp->value.i32 = ntohl(raw[pos]);
            pos++;
            break;

            case ABT_INTEGER64:
            pos += 2;
            break;

            case ABT_UNSIGNED32:
            avp->value.u32 = ntohl(raw[pos]);
            pos++;
            break;

            case ABT_UNSIGNED64:
            pos += 2;
            break;

            case ABT_FLOAT32:
            pos++;
            break;

            case ABT_FLOAT64:
            pos += 2;
            break;

            default:
            break;
        }
    }

    return pos;
}
