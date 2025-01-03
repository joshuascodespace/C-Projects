#include <stdio.h>
#include <string.h>
#include "task_group.h"

int task_group_init(task_group_t *group, unsigned n_tasks) {
    int err = pthread_mutex_init(&group->mutex, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_init: %s\n", strerror(err));
        return -1;
    }

    err = pthread_cond_init(&group->all_tasks_done, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_cond_init: %s\n", strerror(err));
        pthread_mutex_destroy(&group->mutex);
        return -1;
    }

    group->n_tasks = n_tasks;
    group->completed_tasks = 0;
    return 0;
}

int task_group_done(task_group_t *group) {
    int err = pthread_mutex_lock(&group->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err));
        return -1;
    }

    group->completed_tasks++;
    if (group->completed_tasks >= group->n_tasks) {
        err = pthread_cond_broadcast(&group->all_tasks_done);
        if (err != 0) {
            fprintf(stderr, "pthread_cond_broadcast: %s\n", strerror(err));
            pthread_mutex_unlock(&group->mutex);
            return -1;
        }
    }

    err = pthread_mutex_unlock(&group->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(err));
        return -1;
    }

    return 0;
}

int task_group_wait(task_group_t *group) {
    int err = pthread_mutex_lock(&group->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err));
        return -1;
    }

    while (group->completed_tasks < group->n_tasks) {
        err = pthread_cond_wait(&group->all_tasks_done, &group->mutex);
        if (err != 0) {
            fprintf(stderr, "pthread_cond_wait: %s\n", strerror(err));
            pthread_mutex_unlock(&group->mutex);
            return -1;
        }
    }

    err = pthread_mutex_unlock(&group->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(err));
        return -1;
    }

    return 0;
}

int task_group_free(task_group_t *group) {
    int err = pthread_mutex_destroy(&group->mutex);
    if (err != 0) {
        fprintf(stderr, "pthread_mutex_destroy: %s\n", strerror(err));
        return -1;
    }

    err = pthread_cond_destroy(&group->all_tasks_done);
    if (err != 0) {
        fprintf(stderr, "pthread_cond_destroy: %s\n", strerror(err));
        return -1;
    }

    return 0;
}
