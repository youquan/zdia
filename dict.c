#include <stdio.h>
#include <errno.h>
#include <expat.h>

#include "common.h"
#include "dict.h"
#include "log.h"

/*********************** xml parse functions start ************************/
#define BUFFSIZE 8192

typedef struct {
    dict_t *    dict;
    unsigned    depth;
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
    parse_info->depth++;

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

        dict_add_app(parse_info->dict, &app);
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

        dict_add_app(parse_info->dict, &app);
    } else if (0 == strcmp(name, "vendor")) {
        dict_vendor_t vendor;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "vendor-id")) {
                vendor.id = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "code")) {
                vendor.code = atoi(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "name")) {
                vendor.name = strdup(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        dict_add_vendor(parse_info->dict, &vendor);
    } else if (0 == strcmp(name, "command")) {
        dict_cmd_t cmd;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "name")) {
                cmd.name = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "code")) {
                cmd.code = atoi(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "vendor-id")) {
                cmd.vendor_id = strdup(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        dict_app_t *app = (dict_app_t *)array_back(parse_info->dict->apps);
        dict_app_add_cmd(app, &cmd);
    } else if (0 == strcmp(name, "avp")) {
        dict_avp_t avp;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "name")) {
                avp.name = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "code")) {
                avp.code = atoi(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "mandatory")) {
            } else if (0 == strcmp(attrs[i], "may-encrypt")) {
                if (0 == strcmp(attrs[i + 1], "yes")) {
                    avp.may_encrypt = 1;
                } else if (0 == strcmp(attrs[i + 1], "no")) {
                    avp.may_encrypt = 0;
                } else {
                    LOG_WARN("unexpected value for attribute %s.", attrs[i]);
                }
            } else if (0 == strcmp(attrs[i], "protected")) {
            } else if (0 == strcmp(attrs[i], "vendor-bit")) {
            } else if (0 == strcmp(attrs[i], "description")) {
                avp.desc = strdup(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        dict_app_t *app = (dict_app_t *)array_back(parse_info->dict->apps);
        dict_app_add_avp(app, &avp);
    } else if (0 == strcmp(name, "type")) {
        dict_avp_t *avp;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "type-name")) {
                avp.name = strdup(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }
    } else if (0 == strcmp(name, "enum")) {
        dict_avp_t *avp;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "name")) {
                cmd.name = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "code")) {
                cmd.name = atoi(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }
    } else if (0 == strcmp(name, "typedefn")) {
        dict_type_t type;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "type-name")) {
                type.type_name = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "type-parent")) {
                type.type_parent = strdup(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "description")) {
                type.desc = strdup(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        dict_app_t *app = (dict_app_t *)array_back(parse_info->dict->apps);
        dict_app_add_type(app, &type);
    }
}

static void xml_elem_end(void *user_data, const XML_Char *name) {
    xml_parse_info_t *parse_info = (xml_parse_info_t *)user_data;
    parse_info->depth--;
}

static void xml_text(void *user_data, const XML_Char *text, int len) {
}
/*********************** xml parse functions end **************************/

dict_t *dict_new() {
    dict_t *dict = (dict_t *)md_malloc(sizeof(dict_t));
    if (dict == NULL) return NULL;

    dict->apps = array_new(sizeof(dict_app_t));
    dict->vendors = array_new(sizeof(dict_vendor_t));

    if (dict->apps == NULL || dict->vendors == NULL) {
        dict_free(dict);
        return NULL;
    }

    return dict;
}

void dict_free(dict_t *dict) {
    if (dict) {
        array_free(dict->apps);
        array_free(dict->vendors);
    }

    md_free(dict);
}

int dict_add_dict(dict_t *dict, const char *file) {
    xml_parse_info_t parse_info = {dict, 0};

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

    return 0;
}

int dict_add_app(dict_t *dict, const dict_app_t *app) {
    return array_push_back(dict->apps, app);
}

int dict_add_vendor(dict_t *dict, const dict_vendor_t *vendor) {
    return array_push_back(dict->vendors, vendor);
}


dict_app_t *dict_app_new() {
    dict_app_t *app = (dict_app_t *)md_malloc(sizeof(dict_app_t));
    if (app == NULL) return NULL;

    app->cmds = array_new(sizeof(dict_cmd_t));
    app->avps = array_new(sizeof(dict_avp_t));
    app->types = array_new(sizeof(dict_type_t));

    if (app->cmds == NULL || app->avps == NULL || app->types == NULL) {
        dict_app_free(app);
        return NULL;
    }

    app->id   = 0;
    app->name = NULL;
    app->uri  = NULL;

    return app;
}

void dict_app_free(dict_app_t *app) {
    if (app) {
        array_free(app->cmds);
        array_free(app->avps);
        array_free(app->types);

        md_free(app);
    }
}

void dict_app_init(dict_app_t *app) {
    app->id   = 0;
    app->name = NULL;
    app->uri  = NULL;

    app->cmds = array_new(sizeof(dict_cmd_t));
    app->avps = array_new(sizeof(dict_avp_t));
    app->types = array_new(sizeof(dict_type_t));
}

int dict_app_add_avp(dict_app_t *app, const dict_avp_t *avp) {
    return array_push_back(app->avps, avp);
}

int dict_app_add_cmd(dict_app_t *app, const dict_cmd_t *cmd) {
    return array_push_back(app->cmds, cmd);
}

int dict_app_add_type(dict_app_t *app, const dict_type_t *t) {
    return array_push_back(app->types, t);
}


