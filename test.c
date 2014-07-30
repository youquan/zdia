#include <stdio.h>
#include <unistd.h>
#include <string.h>

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

    sleep(100);

    return 0;
}
