#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>

#include "log.h"
#include "server.h"
#include "receiver.h"
#include "common.h"

server_t *server_new() {
    server_t *s = (server_t *)zd_malloc(sizeof(server_t));

    s->conn = NULL;
    s->endpoints = array_new(sizeof(endpoint_t));
    s->receivers = array_new(sizeof(receiver_t *));
    s->workers   = array_new(sizeof(worker_t *));

    int i;
    for (i = 0; i < 4; i++) {
        worker_t *worker = worker_new();
        worker->server = s;
        array_push_back(s->workers, &worker);
    }
    s->curr_worker = 0;

    return s;
}

void server_free(server_t *server) {
    if (!server) return;

    int i;
    for (i = 0; i < server->receivers->size; i++) {
        receiver_free(array_at(server->receivers, i));
    }
    for (i = 0; i < server->workers->size; i++) {
        worker_free(array_at(server->workers, i));
    }

    conn_free(server->conn);
    array_free(server->endpoints);
    array_free(server->receivers);
    array_free(server->workers);

    free(server);
}

void server_init(server_t *server) {
}

static void *server_thread(void *args);
int server_start(server_t *server) {
    conn_t *conn = conn_serv_bind(server->protocol, server->port, server->endpoints); 
    if (!conn) {
        LOG_DEBUG("fail to bind server.");
        return -1;
    }

    server->conn = conn;
    if (pthread_create(&server->thread, NULL, server_thread, server) != 0) {
        LOG_DEBUG("fail to create service thread (errno %d).", errno);
        return -1;
    }

    return 0;
}

int server_stop(server_t *server) {
    return 0;
}

int server_dispatch(server_t *server, msg_t *msg) {
    if (server->workers->size == 0) return -1;

    worker_push_msg(*(worker_t **)array_at(server->workers, server->curr_worker), msg);
    server->curr_worker++;
    if (server->curr_worker == server->workers->size)
        server->curr_worker = 0;

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

        receiver = receiver_new(server, conn);
        if (receiver == NULL) {
            LOG_ERROR("fail to create receiver.");
            continue;
        }

        array_push_back(server->receivers, &receiver);
    } while (1);

    return NULL;
}
