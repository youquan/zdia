#include <netinet/in.h>
#include <malloc.h>
#include <errno.h>
#include <pthread.h>

#include "log.h"
#include "server.h"
#include "receiver.h"

server_t *server_new() {
    server_t *s = (server_t *)malloc(sizeof(server_t));
    if (s == NULL) return NULL;

    list_init(&s->endpoints);
    list_init(&s->receivers);
    s->conn = NULL;

    return s;
}

void server_free(server_t *server) {
    if (!server) return;

    conn_free(server->conn);
    /* workers */
    /* endpoints */
    free(server);
}

void server_init(server_t *server) {
}

static void *server_thread(void *args);
int server_start(server_t *server) {
    conn_t *conn = conn_serv_bind(server->protocol, server->port, &server->endpoints); 
    if (!conn) {
        LOG_DEBUG("fail to bind server.");
        return -1;
    }

    server->conn = conn;
    pthread_create(&server->thread, NULL, server_thread, server);

    return 0;
}

int server_stop(server_t *server) {
    return 0;
}

static void *server_thread(void *args) {
    server_t *server = (server_t *)args;

    if(conn_serv_listen(server->conn)) {
        LOG_ERROR("fail to listen.");
        return NULL;
    }

    do {
        conn_t *conn;
        receiver_t *receiver;

        conn = conn_serv_accept(server->conn);
        if (conn == NULL) {
            LOG_ERROR("fail to accept new conenction");
            continue;
        }

        receiver = receiver_new(conn, 1);
        if (receiver == NULL) {
            LOG_ERROR("fail to create receiver.");
            continue;
        }

        list_add_tail((list_t *)&server->receivers, (list_t *)receiver);
    } while (1);

    return NULL;
}
