#include <malloc.h>
#include <unistd.h>

#include "msg.h"
#include "common.h"
#include "receiver.h"

static int receiver_id = 0;

static void *receiver_thread(void *args);

static worker_t *receiver_next_worker(receiver_t *receiver) {
    worker_t *worker = receiver->workers[receiver->next_worker];

    receiver->next_worker++;
    if (receiver->next_worker == receiver->nworker) {
        receiver->next_worker = 0;
    }

    return worker;
}

receiver_t *receiver_new(conn_t *conn, size_t nworker) {
    receiver_t *receiver = (receiver_t *)md_malloc(sizeof(receiver_t));

    if (receiver == NULL) return NULL;

    if ((receiver->workers = (worker_t **)md_malloc(nworker * sizeof(worker_t *))) == NULL) {
        md_free(receiver);
        return NULL;
    }

    receiver->id = receiver_id++;
    receiver->conn = conn;
    receiver->nworker = nworker;
    receiver->next_worker = 0;

    int i;
    for (i = 0; i < receiver->nworker; i++) {
        receiver->workers[i] = worker_new();
    }

    int ret = pthread_create(&receiver->thread, NULL, receiver_thread, receiver);
    if (ret != 0) {
        LOG_ERROR("fail to create receiver thread: %s.", strerror(ret));
        receiver_free(receiver);
        return NULL;
    }

    return receiver;
}

void receiver_free(receiver_t *receiver) {
    if (!receiver) return;

    conn_free(receiver->conn);

    int i;
    for (i = 0; i < receiver->nworker; i++) {
        worker_free(receiver->workers[i]);
    }

    md_free(receiver->workers);
    md_free(receiver);
}

static void *receiver_thread(void *args) {
    receiver_t *receiver = (receiver_t *)args;
    conn_t *conn = receiver->conn;

    LOG_DEBUG("in receiver thread");

    while (1) {
        unsigned char header[4];
        size_t ret = 0;
        size_t received = 0;
        msg_t *msg;

        do {
            ret = conn_recv(conn, &header[received], sizeof(header) - received);
            if (ret < 0) {
                LOG_ERROR("connection broken");
                // connection down indication
                break;      // stop the receiver thread
            }
            if (ret == 0) {
                // connection down indication
                break;      // stop the receiver thread
            }

            received += ret;
        } while (received < sizeof(header));

        unsigned version = header[0];
        unsigned msg_len = (header[1] << 16) + (header[2] << 8) + header[3];
        if (header[0] != DIAMETER_VERSION) {
            LOG_ERROR("invalid header [version: %u, size: %u], regard as disconnection.", version, msg_len);
            // connection down indication
            break;      // stop the receiver thread
        }

        if ((msg = msg_new(msg_len)) == NULL) {
            LOG_ERROR("fail to create msg object [size: %u].", msg_len);
            // clean
            break;
        }

        while (received < msg_len) {
            ret = conn_recv(conn, (unsigned char *)(msg->raw_data + received), msg_len - received);
            // handle ret
            received += ret;
        };
        memcpy(msg->raw_data, header, sizeof(header));

        msg_parse_header(msg);
        //msg_parse_all(msg);

        if (worker_push_msg(receiver_next_worker(receiver), msg) != 0) {
            LOG_ERROR("fail to push message [code: %u, size: %u] to worker", msg->header.cmd_code, msg_len);
            msg_free(msg);
        }
    }

    LOG_DEBUG("exiting receiver thread, cleaning...");
    receiver_free(receiver);

    return NULL;
}
