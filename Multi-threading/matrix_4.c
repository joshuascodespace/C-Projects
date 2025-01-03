#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "matrix.h"

typedef struct {
    const matrix_t *mat;
    unsigned start;
    unsigned rows_to_add;
} thread_task_t;

void *parallel_sum_func(void *information) {
    //Making variables local for easier access.
    const matrix_t *mat = ((thread_task_t *) information)->mat;
    unsigned start = ((thread_task_t *) information)->start;
    unsigned rows_to_add = ((thread_task_t *) information)->rows_to_add;
    unsigned nums_to_add = rows_to_add * mat->ncols;
    unsigned start_idx = start * mat->ncols;
    long temp_sum = 0;
    
    for (unsigned i = 0; i < nums_to_add; i++) {
        temp_sum += mat->data[start_idx + i];
    }

    return (void *) temp_sum;
}

void *parallel_max_func(void *information) {
    // Making variables local for easier access.
    const matrix_t *mat = ((thread_task_t *) information)->mat;
    unsigned ncols = mat->ncols;
    unsigned index = ((thread_task_t *) information)->start * ncols;
    unsigned nums_to_add = ((thread_task_t *) information)->rows_to_add * ncols;
    long temp_max = mat->data[0];

    for (unsigned i = 0; i < nums_to_add; i++) {
        if (temp_max <= mat->data[index]) {
            temp_max = mat->data[index];
        }
        index++;
    }

    return (void *) temp_max;
}

int matrix_parallel_sum(const matrix_t *mat, unsigned n_threads, long *result) {
    pthread_t threads[n_threads];
    unsigned nrows = mat->nrows;
    unsigned rows_to_offset = nrows % n_threads;
    unsigned start_index = 0;

    //Must give seperate information to each thread.
    thread_task_t all_info[n_threads];

    

    for (unsigned i = 0; i < n_threads; i++) {
        all_info[i].mat = mat;
        all_info[i].start = start_index;

        all_info[i].rows_to_add = nrows / n_threads;
        //Some threads are given an extra row to accomodate the above's remainder
        if (rows_to_offset != 0) {
            all_info[i].rows_to_add++;
            rows_to_offset--;
        }
        int err = pthread_create(&threads[i], NULL, parallel_sum_func, (void *) &all_info[i]);
        if (err != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(err));
            return -1;
        }
        start_index = start_index + all_info[i].rows_to_add;
    }

    long sum = 0;
    for (unsigned i = 0; i < n_threads; i++) {
        long local_sum;
        int err = pthread_join(threads[i], (void **) &local_sum);
        if (err != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(err));
            return -1;
        }
        sum += local_sum;
    }

    *result = sum;
    return 0;
}

int matrix_parallel_max(const matrix_t *mat, unsigned n_threads, long *result) {
    long max = mat->data[0];
    pthread_t threads[n_threads];
    unsigned start_row = 0;
    unsigned rows_to_offset = mat->nrows % n_threads;

    //Must give seperate information to each thread.
    thread_task_t all_info[n_threads];

    for (unsigned i = 0; i < n_threads; i++) {
        all_info[i].mat = mat;
        all_info[i].start = start_row;

        all_info[i].rows_to_add = mat->nrows / n_threads;
        //Some threads are given an extra row to accomodate the above's remainder
        if (rows_to_offset > 0) {
            all_info[i].rows_to_add = all_info[i].rows_to_add + 1;
            rows_to_offset--;
        }

        int err = pthread_create(&threads[i], NULL, parallel_max_func, (void *) &all_info[i]);
        if (err != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(err));
            return -1;
        }

        start_row += all_info[i].rows_to_add;
    }


    for (unsigned i = 0; i < n_threads; i++) {
        long local_max;
        int err = pthread_join(threads[i], (void **) &local_max);
        if (err != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(err));
            return -1;
        }

        if (max <= local_max) {
            max = local_max;
        }
    }

    *result = max;

    return 0;
}
