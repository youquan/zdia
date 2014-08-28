#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "dict.h"
#include "server.h"

int main() {
    server_t *s = server_new();
    s->port = 12345;

    endpoint_t ep;
    struct sockaddr_in *sa = (struct sockaddr_in *)&ep.addr;
    sa->sin_family = AF_INET;
    sa->sin_port = htons(12345);
    sa->sin_addr.s_addr = INADDR_ANY;
    list_append(&s->endpoints, (list_t *)&ep);

    s->protocol = IPPROTO_TCP;
    s->conn = NULL;

    dict_t *dict = dict_new();
    dict_add_dict(dict, "dictionary.xml");

    server_start(s, dict);

    array_iter_t it, it2;

    fprintf(stdout, "application\n");
    for (it = array_begin(dict->apps); it != array_end(dict->apps); it = array_next(dict->apps, it)) {
        dict_app_t *app = (dict_app_t *)it;
        fprintf(stdout, "    app id: %d, name: %s, uri: %s\n", app->id, app->name, app->uri);
    }

    fprintf(stdout, "\nvendor\n");
    for (it = array_begin(dict->vendors); it != array_end(dict->vendors); it = array_next(dict->vendors, it)) {
        dict_vendor_t *vendor = (dict_vendor_t *)it;
        fprintf(stdout, "    vendor id: %d, name: %s\n", vendor->id, vendor->name);
    }

    fprintf(stdout, "\ncommand\n");
    for (it = array_begin(dict->cmds); it != array_end(dict->cmds); it = array_next(dict->cmds, it)) {
        dict_cmd_t *cmd = (dict_cmd_t *)it;
        fprintf(stdout, "    command proxible: %d, name: %s, code: %u\n", cmd->proxiable, cmd->name, cmd->code);

        fprintf(stdout, "        request avps \n");
        for (it2 = array_begin(cmd->req_rules); it2 != array_end(cmd->req_rules); it2 = array_next(cmd->req_rules, it2)) {
            dict_avp_rule_t *rule = (dict_avp_rule_t *)it2;
            fprintf(stdout, "            name: %s, min: %d, max: %d, code: %d, vendor-id: %d\n", rule->avp_name, rule->min, rule->max, rule->avp->code, rule->avp->vendor_id);
        }

        fprintf(stdout, "        answer avps\n");
        for (it2 = array_begin(cmd->ans_rules); it2 != array_end(cmd->ans_rules); it2 = array_next(cmd->ans_rules, it2)) {
            dict_avp_rule_t *rule = (dict_avp_rule_t *)it2;
            fprintf(stdout, "            name: %s, min: %d, max: %d, code: %d, vendor-id: %d\n", rule->avp_name, rule->min, rule->max, rule->avp->code, rule->avp->vendor_id);
        }
    }

    fprintf(stdout, "\navp\n");
    for (it = array_begin(dict->avps); it != array_end(dict->avps); it = array_next(dict->avps, it)) {
        dict_avp_t *avp = (dict_avp_t *)it;
        fprintf(stdout, "    avp name: %s, code: %u, type: %s\n", avp->name, avp->code, avp->type_name);

        if (0 == strcmp("grouped", avp->type_name)) {
            fprintf(stdout, "        sub avps\n");
            for (it2 = array_begin(avp->content.rules); it2 != array_end(avp->content.rules); it2 = array_next(avp->content.rules, it2)) {
                dict_avp_rule_t *rule = (dict_avp_rule_t *)it2;
                fprintf(stdout, "            name: %s, min: %d, max: %d\n", rule->avp_name, rule->min, rule->max);
            }
        } else if (avp->content.enums) {
            fprintf(stdout, "        enums\n");
            for (it2 = array_begin(avp->content.enums); it2 != array_end(avp->content.enums); it2 = array_next(avp->content.enums, it2)) {
                dict_enum_t *e = (dict_enum_t *)it2;
                fprintf(stdout, "            name: %s, code: %d\n", e->name, e->code);
            }
        }
    }

    fprintf(stdout, "\ntype\n");
    for (it = array_begin(dict->types); it != array_end(dict->types); it = array_next(dict->types, it)) {
        dict_avp_type_t *type = (dict_avp_type_t *)it;
        fprintf(stdout, "    type name: %s, parent: %s, codec: %d\n", type->name, type->parent_name, type->base);
    }

    sleep(100);

    return 0;
}
