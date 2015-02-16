#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "dict.h"
#include "server.h"

int main() {
    server_t *s = server_new();
    s->port = 3868;

    endpoint_t ep;
    struct sockaddr_in *sa = (struct sockaddr_in *)&ep.addr;
    sa->sin_family = AF_INET;
    sa->sin_port = htons(3868);
    sa->sin_addr.s_addr = INADDR_ANY;
    array_push_back(s->endpoints, &ep);

    s->protocol = IPPROTO_TCP;
    s->conn = NULL;

    dict_t *dict = dict_new();
    dict_add_dict(dict, "dictionary.xml");
    s->dict = dict;

    server_start(s);

    int i, j;
    j = 0;

    fprintf(stdout, "application\n");
    dict_app_t *app = (dict_app_t *)dict->apps->data;
    for (i = 0; i < dict->apps->size; i++) {
        fprintf(stdout, "    app id: %d, name: %s, uri: %s\n", app[i].id, app[i].name, app[i].uri);
    }

    fprintf(stdout, "\nvendor\n");
    dict_vendor_t *vendor = (dict_vendor_t *)dict->vendors->data;
    for (i = 0; i < dict->vendors->size; i++) {
        fprintf(stdout, "    vendor id: %d, name: %s\n", vendor[i].id, vendor[i].name);
    }

    fprintf(stdout, "\ncommand\n");
    dict_cmd_t *cmd = (dict_cmd_t *)dict->cmds->data;
    for (i = 0; i < dict->cmds->size; i++) {
        fprintf(stdout, "    command proxible: %d, name: %s, code: %u\n", cmd[i].proxiable, cmd[i].name, cmd[i].code);

        fprintf(stdout, "        request avps \n");
        dict_avp_rule_t *rule = (dict_avp_rule_t *)cmd[i].req_rules->data;
        for (j = 0; j < cmd[i].req_rules->size; j++) {
            fprintf(stdout, "            name: %s, min: %d, max: %d, code: %d, vendor-id: %d\n", rule[j].avp_name, rule[j].min, rule[j].max, rule[j].avp->code, rule[j].avp->vendor_id);
        }

        fprintf(stdout, "        answer avps\n");
        rule = (dict_avp_rule_t *)cmd[i].ans_rules->data;
        for (j = 0; j < cmd[i].ans_rules->size; j++) {
            fprintf(stdout, "            name: %s, min: %d, max: %d, code: %d, vendor-id: %d\n", rule[j].avp_name, rule[j].min, rule[j].max, rule[j].avp->code, rule[j].avp->vendor_id);
        }
    }

    fprintf(stdout, "\navp\n");
    dict_avp_t *avp = (dict_avp_t *)dict->avps->data;
    for (i = 0; i < dict->avps->size; i++) {
        fprintf(stdout, "    avp name: %s, code: %u, type: %s\n", avp[i].name, avp[i].code, avp[i].type_name);

        if (0 == strcmp("grouped", avp[i].type_name)) {
            fprintf(stdout, "        sub avps\n");
            dict_avp_rule_t *rule = (dict_avp_rule_t *)avp[i].content.rules->data;
            for (j = 0; j < avp[i].content.rules->size; j++) {
                fprintf(stdout, "            name: %s, min: %d, max: %d\n", rule[j].avp_name, rule[j].min, rule[j].max);
            }
        } else if (avp[i].content.enums) {
            fprintf(stdout, "        enums\n");
            dict_enum_t *e = (dict_enum_t *)avp[i].content.enums->data;
            for (j = 0; j < avp[i].content.enums->size; j++) {
                fprintf(stdout, "            name: %s, code: %d\n", e[j].name, e[j].code);
            }
        }
    }

    fprintf(stdout, "\ntype\n");
    dict_avp_type_t *type = (dict_avp_type_t *)dict->types->data;
    for (i = 0; i < dict->types->size; i++) {
        fprintf(stdout, "    type name: %s, parent: %s, codec: %d\n", type[i].name, type[i].parent_name, type[i].codec);
    }

    free(dict);
    pthread_join(s->thread, NULL);

    return 0;
}
