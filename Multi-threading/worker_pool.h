#ifndef WORKER_POOL_H
#define WORKER_POOL_H

#include <pthread.h>
#include "work_queue.h"

/*
 * Represents a pool of worker threads
 *   queue: The queue from which threads access their work tasks
 *   threads: Array of pthread_t instances representing the workers
 *   size: Number of worker threads in the tpool
 */
typedef struct {
    work_queue_t queue;
    pthread_t *threads;
    unsigned size;
} worker_pool_t;

/*
 * Initialize a new worker pool
 *   pool: The worker pool instance to intialize
 *   pool_size: The number of workers in the pool
 *   queue_size: The number of slots in the pool's queue
 */
int worker_pool_init(worker_pool_t *pool, unsigned pool_size, unsigned queue_size);

/*
 * Free a worker pool, deallocating all associated resources.
 *   pool: The worker pool to free
 * Returns 0 on success or -1 on error
 */
int worker_pool_free(worker_pool_t *pool);

/*
 * Compute the sum of all matrix elements using a pool of worker threads.
 *   mat: The matrix to sum over
 *   pool: The worker threads that should compute the sum
 *   result: Location to store the computed matrix sum
 * Returns 0 on success or -1 on error
 */
int matrix_parallel_sum_pool(const matrix_t *mat, worker_pool_t *pool, long *result);

#endif // WORKER_POOL_H
