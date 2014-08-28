#include <pthread.h>
#include <unistd.h>

#include "common.h"
#include "worker.h"
#include "list.h"
#include "event.h"

int msg_handler_default(const msg_t *msg) {
    return 0;
}

int ind_handler_default(const ind_t *ind) {
    return 0;
}

static msg_handler_func msg_handler = msg_handler_default;
static ind_handler_func ind_handler = ind_handler_default;

static int worker_msg_proc(msg_t *msg) {
    msg_parse_all(msg, dict);

    switch (msg->header.cmd_code) {
        case 257:
        LOG_DEBUG("CE message received");
        break;

        default:
        msg_handler(msg);
        break;
    }

    return 0;
}

static int worker_ind_proc(ind_t *ind) {
    ind_handler(ind);
    return 0;
}

static void *worker_thread(void *args) {
    worker_t *worker = (worker_t *)args;
    fifo_t    fifo = worker->fifo;

    while (worker->status) {
        // 1.get from queue
        event_t ev;
        if (0 != fifo_pop(fifo, &ev)) {
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
    worker_t *worker = (worker_t *)md_malloc(sizeof(worker_t));
    if (worker == NULL) return NULL;

    worker->fifo = fifo_new(sizeof(event_t), 1024);
    if (worker->fifo == NULL) {
        md_free(worker);
        return NULL;
    }

    worker->id = 0;
    worker->status = 0;

    if (worker_start(worker) != 0 ) {
        md_free(worker);
    }

    return worker;
}

void worker_free(worker_t *worker) {
    if (worker) {
        fifo_free(worker->fifo);

        worker_stop(worker);

        md_free(worker);
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
    fifo_push(worker->fifo, &ev);

    return 0;
}

int worker_push_ind(worker_t *worker, ind_t *ind) {
    event_t ev = {EVENT_IND, .data.ind = ind};
    fifo_push(worker->fifo, &ev);

    return 0;
}
