#include <arpa/inet.h>
#include "avp.h"
#include "common.h"

avp_t *avp_new() {
    avp_t *avp = (avp_t *)zd_malloc(sizeof(avp_t));

    memset(avp, 0, sizeof(avp_t));

    return avp;
}

avp_t *avp_new_from(const uint32_t *buf) {
    size_t pos = 0;
    uint32_t value;

    avp_t *avp = (avp_t *)zd_malloc(sizeof(avp_t));
    memset(avp, 0, sizeof(avp_t));
    avp->raw_data = buf;

    /* decode header */
    if (buf != NULL) {
        /* avp code */
        avp->code = ntohl(buf[pos]);
        pos++;

        /* flags and length */
        value = ntohl(buf[pos]);
        avp->flags = (uint8_t)(value >> 24);
        avp->len   = value & 0xffffff;
        pos++;

        /* vendor id if any */
        if (AVP_IS_FLAG_SET(avp, AVP_FLAG_VENDOR)) {
            avp->vendor_id = ntohl(buf[pos]);
        }
    }

    return avp;
}

void avp_free(avp_t *avp) {
    if (avp) {
        /* free sub avps if any */
        if (avp->avps) {
            int i;
            avp_t **a = (avp_t **)avp->avps->data;
            for (i = 0; i < avp->avps->size; i++) {
                avp_free(a[i]);
            }
            array_free(avp->avps);
        }

        /* never free raw_data */
        zd_free(avp);
    }
}

int avp_decode(avp_t *avp) {
    const uint32_t *raw = avp->raw_data;
    size_t pos = AVP_HEADER_SIZE_NO_VENDOR;

    if (AVP_IS_FLAG_SET(avp, AVP_FLAG_VENDOR)) {
        pos = AVP_HEADER_SIZE_WITH_VENDOR;
    }

    /* value */
    switch (avp->dict_avp->type->codec) {
        case CODEC_GROUPED:
            break;

        case CODEC_OCTETSTRING:
            break;

        case CODEC_INTEGER32:
            avp->value.i32 = ntohl(raw[pos]);
            pos++;
            break;

        case CODEC_INTEGER64:
            pos += 2;
            break;

        case CODEC_UNSIGNED32:
            avp->value.u32 = ntohl(raw[pos]);
            pos++;
            break;

        case CODEC_UNSIGNED64:
            pos += 2;
            break;

        case CODEC_FLOAT32:
            pos++;
            break;

        case CODEC_FLOAT64:
            pos += 2;
            break;

        default:
            break;
    }

    return pos;
}
