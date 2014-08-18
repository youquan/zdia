#ifndef _DICT_H_
#define _DICT_H_

#include "avp.h"
#include "msg.h"

typdef struct {
    uint32_t            id;
    const char *        name;
    const char *        uri;
} dict_app_t;

typedef struct {
    const char *        id;
    uint32_t            code;
    const char *        name;
} dict_vendor_t;

typedef struct {
    const char *        type_name;
    const char *        type_parent;
    const char *        desc;
} dict_type_t;

typedef struct {
    uint32_t            id;
    const char *        name;
    const char *        uri;

    /* dict_cmd_t */
    array_t             cmds;

    /* dict_avp_t */
    array_t             avps;

    /* dict_type_t */
    array_t             types;
} dict_app_t;

typedef struct {
    cmd_code_t          code;
    const char *        name;
    uint8_t             flags;
    vendor_id_t         vendor_id;

    dict_avp_schema_t * avps;
} dict_cmd_t;

typedef struct {
    const char *        name;
    const char *        position;
    unsigned            min;
    unsigned            max;
} dict_avp_rule_t;

enum dict_requirement_level {
    RL_MUST,
    RL_REQUIRED = RL_MUST,
    RL_SHALL = RL_MUST,

    RL_MUST_NOT,
    RL_SHALL_NOT = RL_MUST_NOT,

    RL_SHOULD,
    RL_RECOMMENDED = RL_SHOULD,

    RL_SHOULD_NOT,
    RL_NOT_RECOMMENDED = RL_SHOULD_NOT,

    RL_MAY,
    RL_OPTIONAL = RL_MAY
};

typedef struct {
    const char *        name;
    uint32_t            code;
} dict_enum_t;

typedef struct dict_avp {
    const char *                name;
    const char *                desc;
    avp_code_t                  code;

    int                         may_encrypt;
    enum dict_requirement_level mandatory;
    enum dict_requirement_level protected;
    vendor_id_t                 vendor_id;

    dict_type_t                 type;
    dict_enum_t *               enums;
    struct {
        struct dict_avp *       avp;
        dict_avp_rule_t         rule;
    } *avps;
} dict_avp_t;

typedef struct {
    /* dict parameters */


    /* dict_app_t */
    array_t             apps;

    /* dict_avp_t */
    array_t             vendors;

} dict_t;


dict_t *dict_new();
void dict_free(dict_t *dict);

int dict_add_dict(dict_t *dict, const char *file);

int dict_add_app(dict_t *dict, const dict_avp_t *app);
int dict_add_vendor(dict_t *dict, const dict_avp_t *vendor);
int dict_add_avp(dict_t *dict, const dict_avp_t *avp);
int dict_add_cmd(dict_t *dict, const dict_cmd_t *cmd);

int dict_avp_add_schema(dict_avp_t *avp, const dict_avp_schema_t *s);
int dict_cmd_add_schema(dict_cmd_t *cmd, const dict_avp_schema_t *s);

const dict_cmd_t *dict_get_cmd(const dict_t *dict, uint32_t code);
const dict_avp_t *dict_get_avp(const dict_t *dict, uint32_t code);

#endif
