#include <pthread.h>
#include <unistd.h>

#include "common.h"
#include "worker.h"
#include "list.h"
#include "event.h"
#include "avp.h"

int msg_handler_default(const msg_t *msg) {
    return 0;
}

int ind_handler_default(const ind_t *ind) {
    return 0;
}

static msg_handler_func msg_handler = msg_handler_default;
static ind_handler_func ind_handler = ind_handler_default;

static int worker_msg_proc(msg_t *msg) {
    msg_parse_all(msg);

    switch (msg->header.cmd_code) {
        case 257:
        LOG_DEBUG("CE message received");
        break;

        default:
        msg_handler(msg);
        break;
    }

    int i;
    const avp_t **avp = (const avp_t **)msg->avps->data;
    for (i = 0; i < msg->avps->size; i++) {
        LOG_INFO("avp[code: %d; name: %s]: %d", avp[i]->dict_avp->code, avp[i]->dict_avp->name, avp[i]->value.i32);
    }

    return 0;
}

static int worker_ind_proc(ind_t *ind) {
    ind_handler(ind);
    return 0;
}

static void *worker_thread(void *args) {
    worker_t *worker = (worker_t *)args;
    queue_t  *queue  = worker->queue;

    while (worker->status) {
        // 1.get from queue
        event_t ev;
        if (0 != queue_pop(queue, &ev)) {
            usleep(1000000);
            continue;
        }

        switch (ev.type) {
            case EVENT_MSG:
            worker_msg_proc(ev.data.msg);  // return value here is used for stats.
            break;

            case EVENT_IND:
            worker_ind_proc(ev.data.ind);
            break;

            default:
            break;
        }

        // free event;
    }

    return NULL;
}

worker_t *worker_new() {
    worker_t *worker = (worker_t *)zd_malloc(sizeof(worker_t));

    worker->queue = queue_new(sizeof(event_t), 1024);
    if (worker->queue == NULL) {
        zd_free(worker);
        return NULL;
    }

    worker->id = 0;
    worker->status = 0;

    if (worker_start(worker) != 0 ) {
        zd_free(worker);
    }

    return worker;
}

void worker_free(worker_t *worker) {
    if (worker) {
        queue_free(worker->queue);

        worker_stop(worker);

        zd_free(worker);
    }
}

int worker_start(worker_t *worker) {
    if ((worker) && (worker->status == 0)) {
        worker->status = 1;
        int ret = pthread_create(&worker->thread, NULL, worker_thread, worker);

        if (ret != 0) {
            LOG_ERROR("fail to create worker thread: %s.", strerror(ret));
            worker->status = 0;
        }

        return ret;
    }

    return EINVAL;
}

int worker_stop(worker_t *worker) {
    if (worker->status) {
        worker->status = 0;
        pthread_join(worker->thread, NULL);
    }

    return 0;
}

int worker_push_msg(worker_t *worker, msg_t *msg) {
    event_t ev = {EVENT_MSG, .data.msg = msg};
    queue_push(worker->queue, &ev);

    return 0;
}

int worker_push_ind(worker_t *worker, ind_t *ind) {
    event_t ev = {EVENT_IND, .data.ind = ind};
    queue_push(worker->queue, &ev);

    return 0;
}
