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

    server_start(s);

    dict_t *dict = dict_new();
    dict_add_dict(dict, "dictionary.xml");

    array_iter_t it;
    for (it = array_begin(dict->apps); it != array_end(dict->apps); it = array_next(dict->apps, it)) {
        dict_app_t *app = (dict_app_t *)it;
        fprintf(stdout, "app id: %d, name: %s, uri: %s\n", app->id, app->name, app->uri);

        array_iter_t it2;
        for (it2 = array_begin(app->cmds); it2 != array_end(app->cmds); it2 = array_next(app->cmds, it2)) {
            dict_cmd_t *cmd = (dict_cmd_t *)it2;
            fprintf(stdout, "command vendor-id: %s, name: %s, code: %u\n", cmd->vendor_id, cmd->name, cmd->code);
        }

        for (it2 = array_begin(app->avps); it2 != array_end(app->avps); it2 = array_next(app->avps, it2)) {
            dict_avp_t *avp = (dict_avp_t *)it2;
            fprintf(stdout, "avp name: %s, code: %u\n", avp->name, avp->code);
        }

        for (it2 = array_begin(app->types); it2 != array_end(app->types); it2 = array_next(app->types, it2)) {
            dict_type_t *type = (dict_type_t *)it2;
            fprintf(stdout, "type name: %s, parent: %s\n", type->type_name, type->type_parent);
        }

    }

    for (it = array_begin(dict->vendors); it != array_end(dict->vendors); it = array_next(dict->vendors, it)) {
        dict_vendor_t *vendor = (dict_vendor_t *)it;
        fprintf(stdout, "vendor id: %s, code: %d, name: %s\n", vendor->id, vendor->code, vendor->name);
    }

    sleep(100);

    return 0;
}
