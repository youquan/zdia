#include <stdio.h>
#include <errno.h>
#include <expat.h>

#include "common.h"
#include "dict.h"
#include "log.h"

/*********************** xml parse functions start ************************/
#define BUFFSIZE 8192

typedef struct {
    unsigned    depth;
    dict_t *    dict;
} xml_parse_info_t;

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

static void xml_elem_start(void *user_data, const XML_Char *name, const XML_Char **attrs) {
    int i;
    xml_parse_info_t *parse_info = (xml_parse_info_t *)user_data;
    parse_info.depth++;

    if (0 == strcmp(name, "base")) {
        //dict_cmd_t *dict_cmd = parse_info->dict->cmd_dict;
        dict_cmd_t cmd;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "name")) {
                cmd.name = attrs[i + 1];
            } else if (0 == strcmp(attrs[i], "code")) {
                cmd.code = atoi(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "vendor")) {
                cmd.vendor-id = atoi(attrs[i + 1]);
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        dict_add_cmd(parse_info->dict, &cmd);
    } else if (0 == strcmp(name, "application")) {
        dict_app_t app;

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "id")) {
                app.id = atoi(attrs[i + 1]);
            } else if (0 == strcmp(attrs[i], "name")) {
                app.name = attrs[i + 1];
            } else if (0 == strcmp(attrs[i], "uri")) {
                app.uri = attrs[i + 1];
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        dict_add_app(parse_info->dict, &app);
    } else if (0 == strcmp(name, "command")) {
        dict_app_t app;
        app.id = 0;
        app.name = "base";

        for (i = 0; attrs[i]; i += 2) {
            if (0 == strcmp(attrs[i], "uri")) {
                app.uri = attrs[i + 1];
            } else {
                LOG_WARN("unsupported attribute %s with value %s.", attrs[i], attrs[i + 1]);
            }
        }

        dict_add_app(parse_info->dict, &app);
    } else if (0 == strcmp(name, "vendor")) {
    } else if (0 == strcmp(name, "typedefn")) {
    } else if (0 == strcmp(name, "avp")) {
    }

    for (i = 0; i < depth; i++)
        printf(" ");

    printf("%s", name);

    for (i = 0; attrs[i]; i += 2) {
        printf(" %s='%s'", attrs[i], attrs[i + 1]);
    }

    printf("\n");
}

static void xml_elem_end(void *user_data, const XML_Char *name) {
    xml_parse_info_t *parse_info = (xml_parse_info_t *)user_data;
    parse_info.depth--;
}

static void xml_text(void *user_data, const XML_Char *text, int len) {
}
/*********************** xml parse functions end **************************/

dict_t *dict_new() {
    dict_t *dict = (dict_t *)md_malloc(sizeof(dict_t));
    if (dict == NULL) return NULL;

    return dict;
}

void dict_free(dict_t *dict) {
    md_free(dict);
}

int dict_add_dict(dict_t *dict, const char *file) {
    xml_parse_info_t parse_info = {0};

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

int dict_add_app(dict_t *dict, const dict_avp_t *app) {
}

int dict_add_vendor(dict_t *dict, const dict_avp_t *vendor) {
}

int dict_add_avp(dict_t *dict, const dict_avp_t *avp) {
}

int dict_add_cmd(dict_t *dict, const dict_cmd_t *cmd) {
}

