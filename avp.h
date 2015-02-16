#ifndef _AVP_H_
#define _AVP_H_

#include <stdint.h>
#include "dict.h"

/* AVP flags */
enum {
    AVP_FLAG_VENDOR     = 0x80,
    AVP_FLAG_MANDOTARY  = 0x40,
    AVP_FLAG_ENCRYPTED  = 0x20
};


typedef struct {
    avp_code_t      code;
    uint8_t         flags;
    uint32_t        len;
    vendor_id_t     vendor_id;
} avp_header_t;

#define AVP_HEADER_SIZE_NO_VENDOR   2
#define AVP_HEADER_SIZE_WITH_VENDOR 3
#define AVP_IS_FLAG_SET(avp, flag) (((avp)->flags & (flag)) == (flag))

typedef union {
    struct {
        uint8_t    *data;
        size_t      len;
    } os;   /* for octetstring and grouped */
    int32_t         i32;
    int64_t         i64;
    uint32_t        u32;
    uint64_t        u64;
    float           f32;
    double          f64;
} avp_value_t;


typedef struct {
    avp_code_t          code;
    uint8_t             flags;
    uint32_t            len;
    vendor_id_t         vendor_id;

    avp_value_t         value;
    array_t *           avps;

    const dict_avp_t *  dict_avp;

    /* always 4 octets aligned */
    const uint32_t *    raw_data;
} avp_t;

/* avp is always 4 octets aligned */
avp_t *avp_new();
avp_t *avp_new_from(const uint32_t *buf);
void   avp_free(avp_t *avp);

int    avp_encode(avp_t *avp, const dict_t *dict);
int    avp_decode(avp_t *avp);
int    avp_decode_header(avp_t *avp);

size_t avp_get_header_size(uint8_t flags);

#endif
