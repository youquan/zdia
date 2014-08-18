#ifndef _AVP_H_
#define _AVP_H_

#include <stdint.h>

/* Base AVP types */
typedef enum {
    AVP_TYPE_OCTET_STRING,
    AVP_TYPE_INTEGER32,
    AVP_TYPE_INTEGER64,
    AVP_TYPE_UNSIGNED32,
    AVP_TYPE_UNSIGNED64,
    AVP_TYPE_FLOAT32,
    AVP_TYPE_FLOAT64,
    AVP_TYPE_GROUPED
} avp_type_e;


/* AVP flags */
enum {
    AVP_FLAG_MANDOTARY  = 0x80,
    AVP_FLAG_VENDOR     = 0x40,
    AVP_FLAG_ENCRYPTED  = 0x20
};


typedef uint32_t avp_code_t;
typedef uint32_t vendor_id_t;


typedef struct {
    avp_code_t      code;
    uint8_t         flags;
    uint32_t        len;
    vendor_id_t     vendor_id;
} avp_header_t;

#define AVP_HEADER_SIZE_NO_VENDOR   8
#define AVP_HEADER_SIZE_WITH_VENDOR 12

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
} avp_value_u;


typedef struct {
    avp_header_t    header;
    avp_value_u     value;
} avp_t;

size_t avp_get_header_size(uint8_t flags);

#endif
