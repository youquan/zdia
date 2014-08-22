#ifndef _DICT_H_
#define _DICT_H_

#include "avp.h"
#include "msg.h"
#include "array.h"

//typedef uint32_t        cmd_code_t;
//typedef uint32_t        avp_code_t;
//typedef uint32_t        vendor_id_t;
//typedef uint32_t        app_id_t;

/* cmd: index = vendor-id >> 32 + code
 * avp: index = app-id >> 32 + code
 * */
typedef uint64_t        dict_index_t;

enum dict_avp_base_type {
    ABT_GROUPED,
    ABT_OCTETSTRING
};

enum dict_avp_position {
    AP_UNSPECIFIED,
    AP_FIRST,
    AP_LAST
};

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
    vendor_id_t         id;
    const char *        name;
} dict_vendor_t;

typedef struct {
    const char *        name;
    const char *        parent_name;
    const char *        desc;

    uint32_t            parent;
    int                 codec;
} dict_avp_type_t;

typedef struct {
    app_id_t            id;
    const char *        name;
    const char *        uri;
} dict_app_t;

typedef struct {
    /* dict_avp_t object index in dict */
    dict_index_t            avp;
    const char *            avp_name;

    /* rules, available for both command and grouped avp  */
    enum dict_avp_position  position;
    unsigned                min;
    unsigned                max;
} dict_avp_rule_t;

typedef struct {
    uint32_t                code;
    const char *            name;
} dict_enum_t;

typedef struct dict_avp {
    const char *                name;
    const char *                desc;
    avp_code_t                  code;
    vendor_id_t                 vendor_id;

    int                         may_encrypt;
    enum dict_requirement_level mandatory;
    enum dict_requirement_level protected;

    const char *                type_name;
    dict_avp_type_t             type;

    /* enum values or sub-avps with rule */
    union {
        array_t                 enums;
        array_t                 rules;
    } content;

    const dict_app_t *          app;
} dict_avp_t;

typedef struct {
    cmd_code_t          code;
    const char *        name;
    int                 proxiable;

    /* dict_avp_rule_t */
    array_t             req_rules;
    array_t             ans_rules;

    const dict_app_t *  app;
} dict_cmd_t;

typedef struct {
    /* dict parameters */

    /* dict_app_t */
    array_t             apps;

    /* dict_avp_t */
    array_t             vendors;

    /* dict_cmd_t */
    array_t             cmds;

    /* dict_avp_t */
    array_t             avps;

    /* dict_avp_type_t */
    array_t             types;

} dict_t;


/* dict operations */
dict_t *dict_new();
void dict_free(dict_t *dict);

int dict_add_dict(dict_t *dict, const char *file);

dict_app_t * dict_add_app(dict_t *dict, const dict_app_t *app);
dict_cmd_t * dict_add_cmd(dict_t *dict, const dict_cmd_t *cmd);
dict_avp_t * dict_add_avp(dict_t *dict, const dict_avp_t *avp);
dict_avp_type_t * dict_add_avp_type(dict_t *dict, const dict_avp_type_t *type);
int dict_add_vendor(dict_t *dict, const dict_vendor_t *vendor);

/* dict app operations */
dict_app_t *dict_app_new();
void dict_app_free(dict_app_t *app);
void dict_app_init(dict_app_t *app);
int dict_app_add_avp(dict_app_t *app, const dict_avp_t *avp);
int dict_app_add_cmd(dict_app_t *app, const dict_cmd_t *cmd);
int dict_app_add_type(dict_app_t *app, const dict_avp_type_t *t);

/* dict avp operations */
int dict_avp_add_avp(dict_avp_t *avp, const dict_avp_t *sub, const dict_avp_rule_t *r);
int dict_cmd_add_avp(dict_cmd_t *cmd, const dict_avp_t *sub, const dict_avp_rule_t *r);

const dict_cmd_t *dict_get_cmd(const dict_t *dict, uint32_t code);
const dict_avp_t *dict_get_avp(const dict_t *dict, uint32_t code);

const char *dict_dump(const dict_t *dict);

#endif
