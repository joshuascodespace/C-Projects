#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"
#include "work_queue.h"

int work_queue_init(work_queue_t *queue, unsigned size) {
    if (size == 0) {
        return -1;
    }

    int err = pthread_mutex_init(&queue->mutex, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_init: %s\n", strerror(err));
        return -1;
    }
    err = pthread_cond_init(&queue->item_available, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_cond_init: %s\n", strerror(err));
        pthread_mutex_destroy(&queue->mutex);
        return -1;
    }
    err = pthread_cond_init(&queue->space_available, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_cond_init: %s\n", strerror(err));
        pthread_mutex_destroy(&queue->mutex);
        pthread_cond_destroy(&queue->item_available);
    }

    queue->buffer = malloc(size * sizeof(work_queue_item_t));
    if (queue->buffer == NULL) {
        perror("malloc");
        pthread_mutex_destroy(&queue->mutex);
        pthread_cond_destroy(&queue->item_available);
        pthread_cond_destroy(&queue->space_available);
        return -1;
    }

    queue->buf_read_idx = 0;
    queue->buf_write_idx = 0;
    queue->buf_len = 0;
    queue->buf_capacity = size;
    queue->shutdown = 0;
    return 0;
}

int work_queue_free(work_queue_t *queue) {
    free(queue->buffer);
    int ret_val = 0;
    int err;
    err = pthread_mutex_destroy(&queue->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_destroy: %s\n", strerror(err));
        ret_val = -1;
    }

    err = pthread_cond_destroy(&queue->item_available);
    if (err != 0) {
        fprintf(stderr, "pthread_cond_destroy: %s\n", strerror(err));
        ret_val = -1;
    }

    err = pthread_cond_destroy(&queue->space_available);
    if (err != 0) {
        fprintf(stderr, "pthread_cond_destroy: %s\n", strerror(err));
        ret_val = -1;
    }

    return ret_val;
}

int work_queue_put(work_queue_t *queue, work_queue_item_t *item) {
    int err = pthread_mutex_lock(&queue->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err));
        return -1;
    }

    if (queue->shutdown != 0) {
        err = pthread_mutex_unlock(&queue->mutex);
        if (err != 0) {
            fprintf(stderr, "pthread_mutex_unlock: %s\n",strerror(err));
            return -1;
        }
        return 1;
    }


    while(queue->buf_len >= queue->buf_capacity) {
        err = pthread_cond_wait(&queue->space_available,&queue->mutex);
        if (err != 0) {
            fprintf(stderr, "pthread_cond_wait: %s\n", strerror(err));
            pthread_mutex_unlock(&queue->mutex);
            return -1;
        }

        //Need to make sure queue wasn't put in shutdown while thread waited.
        if (queue->shutdown != 0) {
            err = pthread_mutex_unlock(&queue->mutex);
            if (err != 0) {
                fprintf(stderr, "pthread_mutex_unlock: %s\n",strerror(err));
                return -1;
            }
            return 1;
        }
    }
    queue->buffer[queue->buf_write_idx] = *item;
    queue->buf_len = queue->buf_len + 1;
    queue->buf_write_idx = queue->buf_write_idx + 1;

    // If next index is outside of queue, wrap around to beginning.
    if (queue->buf_write_idx >= queue->buf_capacity) {
        queue->buf_write_idx = 0;
    }

    // Item was added to the queue, so an item is now available.
    err = pthread_cond_signal(&queue->item_available);
    if (err != 0) {
        fprintf(stderr, "pthread_cond_signal: %s\n", strerror(err));
        pthread_mutex_unlock(&queue->mutex);
        return -1;
    }
    err = pthread_mutex_unlock(&queue->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(err));
        return -1;
    }
    return 0;
}

int work_queue_get(work_queue_t *queue, work_queue_item_t *dest) {
    int err = pthread_mutex_lock(&queue->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err));
        return -1;
    }

    if (queue->shutdown != 0) {
        err = pthread_mutex_unlock(&queue->mutex);
        if (err != 0) {
            fprintf(stderr, "pthread_mutex_unlock: %s\n",strerror(err));
            return -1;
        }
        return 1;
    }

    while (queue->buf_len == 0) {
        err = pthread_cond_wait(&queue->item_available, &queue->mutex);
        if (err != 0) {
            pthread_mutex_unlock(&queue->mutex);
            fprintf(stderr, "pthread_cond_wait: %s\n", strerror(err));
            return -1;
        }

        //Need to make sure queue wasn't put in shutdown while thread waited.
        if (queue->shutdown != 0) {
            err = pthread_mutex_unlock(&queue->mutex);
            if (err != 0) {
                fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(err));
                return -1;
            }
            return 1;
        }
    }

    
    *dest = queue->buffer[queue->buf_read_idx];
    queue->buf_len = queue->buf_len - 1;
    queue->buf_read_idx = queue->buf_read_idx + 1;

    // If next index is outside of queue, wrap around to beginning.
    if (queue->buf_read_idx >= queue->buf_capacity) {
        queue->buf_read_idx = 0;
    }

    // Item was removed from queue, so space is now available.
    err = pthread_cond_signal(&queue->space_available);
    if (err != 0) {
        pthread_mutex_unlock(&queue->mutex);
        fprintf(stderr, "pthread_cond_signal: %s\n", strerror(err));
        return -1;
    }

    err = pthread_mutex_unlock(&queue->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(err));
        return -1;
    }

    return 0;
}

int work_queue_shut_down(work_queue_t *queue) {
    // No lock; want to update shutdown value before thread with mutex finishes.
    queue->shutdown = 1;

    int err = pthread_mutex_lock(&queue->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err));
        return -1;
    }

    //Want all waiting threads to be notified of shutdown queue.
    err = pthread_cond_broadcast(&queue->item_available);
    if (err != 0) {
        pthread_mutex_unlock(&queue->mutex);
        fprintf(stderr, "pthread_mutex_broadcast: %s\n", strerror(err));
        return -1;
    }
    err = pthread_cond_broadcast(&queue->space_available);
    if (err != 0) {
        pthread_mutex_unlock(&queue->mutex);
        fprintf(stderr, "pthread_mutex_broadcast: %s\n", strerror(err));
        return -1;
    }

    err = pthread_mutex_unlock(&queue->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(err));
        return -1;
    }

    return 0;
}
