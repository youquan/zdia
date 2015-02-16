#include <stdio.h>
#include <errno.h>
#include <expat.h>

#include "common.h"
#include "dict.h"
#include "log.h"

/*********************** xml parse functions start ************************/
#define BUFFSIZE 8192

typedef struct {
    dict_t *        dict;
    array_t         path;
    dict_app_t *    curr_app;
    dict_cmd_t *    curr_cmd;
    dict_avp_t *    curr_avp;
    array_t *       curr_rules;
} xml_parse_info_t;

#if 0
static void xml_dia_command() {
}

static void xml_dia_avp() {
}

static void xml_dia_app() {
}

static void xml_dia_vendor() {
}

static void xml_dia_type() {
}
#endif

static void xml_elem_start(void *user_data, const XML_Char *name, const XML_Char **attrs) {
    int i;
    xml_parse_info_t *parse_info = (xml_parse_info_t *)user_data;
    array_push_back(&parse_info->path, strdup(name));

    if (0 == strcmp(name, "base")) {
        dict_app_t app;
        dict_app_init(&app);

        app.id = 0;
        app.name = "base";

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "uri")) {
                app.uri = strdup(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s, ignored.", attrs[i], attrs[i + 1]);
            }
        }

        parse_info->curr_app = dict_add_app(parse_info->dict, &app);
    } else if (0 == strcmp(name, "application")) {
        dict_app_t app;
        dict_app_init(&app);

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "id")) {
                app.id = atoi(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "name")) {
                app.name = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "uri")) {
                app.uri = strdup(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        parse_info->curr_app = dict_add_app(parse_info->dict, &app);
    } else if (0 == strcmp(name, "vendor")) {
        dict_vendor_t vendor;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "id")) {
                vendor.id = atoi(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "name")) {
                vendor.name = strdup(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        dict_add_vendor(parse_info->dict, &vendor);
    } else if (0 == strcmp(name, "command")) {
        dict_cmd_t cmd;
        cmd.proxiable = 1;
        cmd.req_rules = array_new(sizeof(dict_avp_rule_t));
        cmd.ans_rules = array_new(sizeof(dict_avp_rule_t));

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "name")) {
                cmd.name = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "code")) {
                cmd.code = atoi(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "proxiable")) {
                cmd.proxiable = atoi(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        parse_info->curr_cmd = dict_add_cmd(parse_info->dict, &cmd);
    } else if (0 == strcmp(name, "requestrules")) {
        parse_info->curr_rules = parse_info->curr_cmd->req_rules;
    } else if (0 == strcmp(name, "answerrules")) {
        parse_info->curr_rules = parse_info->curr_cmd->ans_rules;
    } else if (0 == strcmp(name, "grouped")) {
        parse_info->curr_avp->type_name = "grouped";
        parse_info->curr_avp->content.rules = array_new(sizeof(dict_avp_rule_t));
        parse_info->curr_rules = parse_info->curr_avp->content.rules;
    } else if (0 == strcmp(name, "avprule")) {
        dict_avp_rule_t rule;
        memset(&rule, 0, sizeof(rule));

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "name")) {
                rule.avp_name = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "position")) {
                if (0 == strcmp(attrs[i + 1], "first")) {
                    rule.position = AP_FIRST;
                } else if (0 == strcmp(attrs[i + 1], "last")) {
                    rule.position = AP_LAST;
                } else if (0 == strcmp(attrs[i + 1], "unspecified")) {
                    rule.position = AP_UNSPECIFIED;
                } else {
                    rule.position = AP_UNSPECIFIED;
                    LOG_WARN("unexpected value [%s] for attribute [%s].", attrs[i + 1], attrs[i]);
                }
            } else if (0 == strcmp(attrs[i], "min")) {
                rule.min = atoi(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "max")) {
                rule.max = atoi(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        if (rule.max < rule.min) rule.max = rule.min;
        array_push_back(parse_info->curr_rules, &rule);
    } else if (0 == strcmp(name, "avp")) {
        dict_avp_t avp;
        memset(&avp, 0, sizeof(avp));
        avp.app = parse_info->curr_app;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "name")) {
                avp.name = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "code")) {
                avp.code = atoi(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "mandatory")) {
                if (0 == strcmp(attrs[i + 1], "may")) {
                    avp.mandatory = RL_MAY;
                } else if (0 == strcmp(attrs[i + 1], "must")) {
                    avp.mandatory = RL_MUST;
                } else if (0 == strcmp(attrs[i + 1], "mustnot")) {
                    avp.mandatory = RL_MUST_NOT;
                } else {
                    LOG_WARN("unexpected value [%s] for attribute [%s].", attrs[i + 1], attrs[i]);
                }
            } else if (0 == strcmp(attrs[i], "may-encrypt")) {
                if (0 == strcmp(attrs[i + 1], "yes")) {
                    avp.may_encrypt = 1;
                } else if (0 == strcmp(attrs[i + 1], "no")) {
                    avp.may_encrypt = 0;
                } else {
                    LOG_WARN("unexpected value [%s] for attribute [%s].", attrs[i + 1], attrs[i]);
                }
            } else if (0 == strcmp(attrs[i], "protected")) {
                if (0 == strcmp(attrs[i + 1], "may")) {
                    avp.protected = RL_MAY;
                } else if (0 == strcmp(attrs[i + 1], "must")) {
                    avp.protected = RL_MUST;
                } else if (0 == strcmp(attrs[i + 1], "mustnot")) {
                    avp.protected = RL_MUST_NOT;
                } else {
                    LOG_WARN("unexpected value [%s] for attribute [%s].", attrs[i + 1], attrs[i]);
                }
            } else if (0 == strcmp(attrs[i], "description")) {
                avp.desc = strdup(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        parse_info->curr_avp = dict_add_avp(parse_info->dict, &avp);
    } else if (0 == strcmp(name, "type")) {
        dict_avp_t *avp = parse_info->curr_avp;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "type-name")) {
                avp->type_name = strdup(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }
    } else if (0 == strcmp(name, "enum")) {
        dict_enum_t e;

        array_t **enums = &parse_info->curr_avp->content.enums;
        if (*enums == NULL) *enums = array_new(sizeof(dict_enum_t));

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "name")) {
                e.name = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "code")) {
                e.code = atoi(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        array_push_back(*enums, &e);
    } else if (0 == strcmp(name, "typedefn")) {
        dict_avp_type_t type;
        memset(&type, 0, sizeof(type));
        type.codec = CODEC_UNKNOWN;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "type-name")) {
                type.name = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "type-parent")) {
                type.parent_name = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "description")) {
                type.desc = strdup(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        dict_add_avp_type(parse_info->dict, &type);
    }
}

static void xml_elem_end(void *user_data, const XML_Char *name) {
    xml_parse_info_t *parse_info = (xml_parse_info_t *)user_data;
    array_pop_back(&parse_info->path);
}

static void xml_text(void *user_data, const XML_Char *text, int len) {
}
/*********************** xml parse functions end **************************/

static int cmp_app(const void *a, const void *b) {
    return ((const dict_app_t *)a)->id - ((const dict_app_t *)b)->id;
}

static int cmp_vendor(const void *a, const void *b) {
    return ((const dict_vendor_t *)a)->id - ((const dict_vendor_t *)b)->id;
}

static int cmp_cmd(const void *a, const void *b) {
    return ((const dict_cmd_t *)a)->code - ((const dict_cmd_t *)b)->code;
}

static int cmp_avp(const void *a, const void *b) {
    return ((const dict_avp_t *)a)->code - ((const dict_avp_t *)b)->code;
}

static int cmp_type(const void *a, const void *b) {
    return strcmp(((const dict_avp_type_t *)a)->name, ((const dict_avp_type_t *)b)->name);
}

static void dict_build_base_types(dict_t *dict) {
    static dict_avp_type_t types[] = {
        /* name,        parent_name,   description,         parent,    base */
        {"Grouped",     NULL,          "Grouped",           NULL,      CODEC_GROUPED},
        {"OctetString", NULL,          "Octet String",      NULL,      CODEC_OCTETSTRING},
        {"Integer32",   NULL,          "Integer32",         NULL,      CODEC_INTEGER32},
        {"Integer32",   NULL,          "Integer32",         NULL,      CODEC_INTEGER32},
        {"Unsigned32",  NULL,          "Unsigned32",        NULL,      CODEC_UNSIGNED32},
        {"Unsigned64",  NULL,          "Unsigned64",        NULL,      CODEC_UNSIGNED64},
        {"Float32",     NULL,          "Float32",           NULL,      CODEC_FLOAT32},
        {"Float64",     NULL,          "Float64",           NULL,      CODEC_FLOAT64}
    };

    int i;
    for (i = 0; i < sizeof(types)/sizeof(dict_avp_type_t); i++) {
        array_push_back(dict->types, &types[i]);
    }
}

static void dict_build_rules(array_t *rules, array_t *avps) {
    int i;

    dict_avp_rule_t *rule = (dict_avp_rule_t *)rules->data;
    for (i = 0; i < rules->size; i++) {
        int j;

        /* already bind */
        if (rule[i].avp != NULL) continue;

        dict_avp_t *avp = (dict_avp_t *)avps->data;
        for (j = 0; j < avps->size; j++) {
            if (0 == strcmp(rule[i].avp_name, avp[j].name)) {
                rule[i].avp = &avp[j];
                break;
            }
        }
    }
}

static void dict_build(dict_t *dict) {
    int i, j;

    array_sort(dict->apps);

    array_sort(dict->vendors);

    array_sort(dict->cmds);
    dict_cmd_t *cmd = (dict_cmd_t *)dict->cmds->data;
    for (i = 0; i < dict->cmds->size; i++) {
        dict_build_rules(cmd[i].req_rules, dict->avps);
        dict_build_rules(cmd[i].ans_rules, dict->avps);
    }

    /* create type chain at first */
    array_sort(dict->types);
    dict_avp_type_t *type = (dict_avp_type_t *)dict->types->data;
    for (i = 0; i < dict->types->size; i++) {
        if (type[i].parent_name == NULL) continue;

        for (j = 0; j < dict->types->size; j++) {
            if (0 == strcmp(type[j].name, type[i].parent_name)) {
                type[i].parent = &type[j];
                break;
            }
        }
    }

    array_sort(dict->avps);
    dict_avp_t *avp = (dict_avp_t *)dict->avps->data;
    for (i = 0; i < dict->avps->size; i++) {
        if (0 == strcmp(avp[i].type_name, "grouped")) {
            avp[i].type = &type[CODEC_GROUPED];
            dict_build_rules(avp[i].content.rules, dict->avps);
        }

        type = (dict_avp_type_t *)dict->types->data;
        for (j = 0; j < dict->types->size; j++) {
            if (0 == strcmp(type[j].name, avp[i].type_name)) {
                avp[i].type = &type[j];
                break;
            }
        }
    }

    /* set codec for types */
    type = (dict_avp_type_t *)dict->types->data;
    for (i = 0; i < dict->types->size; i++) {
        dict_avp_type_t *curr = &type[i];

        while (curr && curr->codec == CODEC_UNKNOWN) {
            curr = curr->parent;
        };

        if (curr) {
            type[i].codec = curr->codec;
        } else {
            LOG_ERROR("no codec for type %s.", type->name);
        }
    }
}

dict_t *dict_new() {
    dict_t *dict = (dict_t *)zd_malloc(sizeof(dict_t));

    dict->apps    = array_new(sizeof(dict_app_t));
    dict->vendors = array_new(sizeof(dict_vendor_t));
    dict->cmds    = array_new(sizeof(dict_cmd_t));
    dict->avps    = array_new(sizeof(dict_avp_t));
    dict->types   = array_new(sizeof(dict_avp_type_t));

    dict->apps->cmp    = cmp_app;
    dict->vendors->cmp = cmp_vendor;
    dict->cmds->cmp    = cmp_cmd;
    dict->avps->cmp    = cmp_avp;
    dict->types->cmp   = cmp_type;

    dict_build_base_types(dict);

    return dict;
}

void dict_free(dict_t *dict) {
    if (dict) {
        array_free(dict->apps);
        array_free(dict->vendors);
        array_free(dict->cmds);
        array_free(dict->avps);
        array_free(dict->types);
    }

    zd_free(dict);
}

int dict_add_dict(dict_t *dict, const char *file) {
    xml_parse_info_t parse_info;
    parse_info.dict = dict;
    array_init(&parse_info.path, sizeof(char *));

    XML_Parser p = XML_ParserCreate(NULL);
    if (!p) {
        LOG_ERROR("cannot create xml parser.");
        return ENOMEM;
    }

    XML_SetUserData(p, &parse_info);
    XML_SetElementHandler(p, xml_elem_start, xml_elem_end);
    XML_SetCharacterDataHandler(p, xml_text);

    FILE *fp = fopen(file, "r");
    if (!fp) {
        LOG_ERROR("cannot open dictionary file %s.", file);
        return EBADFD;
    }

    char buf[BUFFSIZE];
    int done;
    int len;
    for (;;) {
        len = fread(buf, 1, BUFFSIZE, fp);

        if (ferror(fp)) {
            LOG_ERROR("read error");
            fclose(fp);
            return EBADFD;
        }

        done = feof(fp);

        if (!XML_Parse(p, buf, len, done)) {
            LOG_ERROR("parse error at line %u: %s\n",
                       (unsigned)XML_GetCurrentLineNumber(p),
                       XML_ErrorString(XML_GetErrorCode(p)));
            return EINVAL;
        }

        if (done)
            break;
    }

    fclose(fp);
    XML_ParserFree(p);
    array_free_data(&parse_info.path);

    dict_build(dict);

    return 0;
}

dict_app_t * dict_add_app(dict_t *dict, const dict_app_t *app) {
    if (array_push_back(dict->apps, app) == 0) {
        return array_back(dict->apps);
    } else {
        return NULL;
    }
}

dict_cmd_t * dict_add_cmd(dict_t *dict, const dict_cmd_t *cmd) {
    if (array_push_back(dict->cmds, cmd) == 0) {
        return array_back(dict->cmds);
    } else {
        return NULL;
    }
}

dict_avp_t * dict_add_avp(dict_t *dict, const dict_avp_t *avp) {
    if (array_push_back(dict->avps, avp) == 0) {
        return array_back(dict->avps);
    } else {
        return NULL;
    }
}

dict_avp_type_t * dict_add_avp_type(dict_t *dict, const dict_avp_type_t *type) {
    dict_avp_type_t *ret = array_find(dict->types, type);
    if (ret != array_end(dict->types)) {
        LOG_WARN("existing type %s.", type->name);
        return ret;
    }

    if (array_push_back(dict->types, type) == 0) {
        return array_back(dict->types);
    } else {
        return NULL;
    }
}

int dict_add_vendor(dict_t *dict, const dict_vendor_t *vendor) {
    return array_push_back(dict->vendors, vendor);
}

dict_app_t *dict_app_new() {
    dict_app_t *app = (dict_app_t *)zd_malloc(sizeof(dict_app_t));
    if (app == NULL) return NULL;

    app->id   = 0;
    app->name = NULL;
    app->uri  = NULL;

    return app;
}

void dict_app_free(dict_app_t *app) {
    if (app) {
        zd_free(app);
    }
}

void dict_app_init(dict_app_t *app) {
    app->id   = 0;
    app->name = NULL;
    app->uri  = NULL;
}

const dict_cmd_t *dict_get_cmd(const dict_t *dict, cmd_code_t code, app_id_t app) {
    return NULL;
}

const dict_avp_t *dict_get_avp(const dict_t *dict, avp_code_t code, vendor_id_t vendor_id) {
    int i;

    dict_avp_t *da = (dict_avp_t *)dict->avps->data;
    for (i = 0; i < dict->avps->size; i++) {
        if (da[i].code == code && da[i].vendor_id == vendor_id) {
            return &da[i];
        }
    }

    return NULL;
}

