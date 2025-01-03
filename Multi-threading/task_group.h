#ifndef TASK_GROUP_H
#define TASK_GROUP_H

#include <pthread.h>

/*
 * Struct representing a specific task group instance
 *  n_tasks: The number of tasks in the group
 *   completed_tasks: The number of completed tasks, between 0 and n_tasks
 *   mutex: Used for synchronization when checking/modifying task group state
 *   all_tasks_done: Used for a thread to wait until all tasks marked as complete
 */
typedef struct {
    unsigned n_tasks;
    unsigned completed_tasks;
    pthread_mutex_t mutex;
    pthread_cond_t all_tasks_done;
} task_group_t;

/*
 * Initializes a new task group instance
 *   group: The instance to initialize
 *   n_tasks: The number of tasks in this group
 * Returns 0 on success or -1 on error
 */
int task_group_init(task_group_t *group, unsigned n_tasks);

/*
 * Mark one task in the group as complete. Should only be called once per task.
 *   group: The group containing the newly completed task
 * Returns 0 on success or -1 on error
 */
int task_group_done(task_group_t *group);

/*
 * Block the calling thread until all tasks in group are marked as complete
 *   group: The group containing the tasks to wait on
 * Returns 0 on success or -1 on error
 */
int task_group_wait(task_group_t *group);

/*
 * Free a task group, cleaning up all allocated resources
 * Returns 0 on success or -1 on error
 */
int task_group_free(task_group_t *group);

#endif // TASK_GROUP_H
