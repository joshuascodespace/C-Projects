#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <pthread.h>
#include "matrix.h"
#include "task_group.h"

/*
 * Represents one unit of work in the queue
 *   mat: The matrix to work on
 *   row_num: Which row in the matrix to work on
 *   destination: Pointer to the value to update with results of work
 *   dest_mutex: Synchronizes access to the destination memory location
 *   task_group: Task group to notify when work is done
 */
typedef struct {
    const matrix_t *mat;
    unsigned row_num;
    long *destination;
    pthread_mutex_t *dest_mutex;
    task_group_t *task_group;
} work_queue_item_t;

/*
 * Represents a work queue instance.
 *   buffer: A circular buffer for storing work items
 *   buf_read_idx: Position in buffer of next occupied slot to remove from
 *   buf_write_idx: Position in buffer of next empty slot to store to
 *   buf_len: The number of occupied slots in the buffer
 *   buf_capacity: The total number of slots in the buffer (stays constant)
 *   shutdown: Indicates whether or not work queue is shut down
 *   mutex: Synchronizes access to the work queue
 *   item_avaialble: Used for threads to wait until new work is available
 *   space_avaiable: Used for threads to wait until an open slot is available
 */
typedef struct {
    work_queue_item_t *buffer;
    int buf_read_idx;
    int buf_write_idx;
    int buf_len;
    int buf_capacity;
    int shutdown;
    pthread_mutex_t mutex;
    pthread_cond_t item_available;
    pthread_cond_t space_available;
} work_queue_t;

/*
 * Initialize a new work queue
 *   queue: The work queue instance to initialize
 *   size: The number of slots in the queue (its capacity)
 * Returns 0 on success or -1 on error
 */
int work_queue_init(work_queue_t *queue, unsigned size);

/*
 * Frees a work queue instance, deallocating all associated resources
 *   queue: The work queue to free
 * Returns 0 on success or -1 on error
 */
int work_queue_free(work_queue_t *queue);

/*
 * Add a new item to the work queue, blocking if necessary until space in the
 * queue becomes available.
 *   queue: The queue instance to add to
 *   item: The item to add
 * Returns 0 on success, -1 on error, or 1 if queue was shut down
 */
int work_queue_put(work_queue_t *queue, work_queue_item_t *item);

/*
 * Remove an item from the work queue, blocking if necessary until an item in
 * the queue becomes available.
 *   queue: The queue instance to add to
 *   item: Location to store the retrieved item
 * Returns 0 on success, -1 on error, or 1 if queue was shut down
 */
int work_queue_get(work_queue_t *queue, work_queue_item_t *dest);

/*
 * Shut down the work queue, alerting all threads waiting to perform a put
 * or a get operation.
 *   queue: The queue to shut down
 * Returns 0 on success or -1 on error
 */
int work_queue_shut_down(work_queue_t *queue);

#endif // WORK_QUEUE_H
