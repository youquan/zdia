#include <malloc.h>
#include <unistd.h>

#include "msg.h"
#include "common.h"
#include "receiver.h"

static int receiver_id = 0;

static void *receiver_thread(void *args);

receiver_t *receiver_new(server_t *server, conn_t *conn) {
    receiver_t *receiver = (receiver_t *)zd_malloc(sizeof(receiver_t));

    receiver->id = receiver_id++;
    receiver->conn = conn;
    receiver->server = server;

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

    zd_free(receiver);
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
            ret = conn_recv(conn, (unsigned char *)(msg->raw_data) + received, msg_len - received);
            // handle ret
            received += ret;
        };
        memcpy((void *)msg->raw_data, header, sizeof(header));

        msg_parse_header(msg);
        //msg_parse_all(msg);

        if (server_dispatch(receiver->server, msg) != 0) {
            LOG_ERROR("fail to push message [code: %u, size: %u] to worker", msg->header.cmd_code, msg_len);
            msg_free(msg);
        }
    }

    LOG_DEBUG("exiting receiver thread, cleaning...");
    receiver_free(receiver);

    return NULL;
}
