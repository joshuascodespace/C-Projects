#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"
#include "worker_pool.h"

#define MAX_INPUT_LEN 128
#define PROMPT ">> "

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <num_workers> <queue_size>\n", argv[0]);
        return 0;
    }
    int num_workers = atoi(argv[1]);
    if (num_workers <= 0) {
        printf("Error: Number of workers must be positive\n");
        return 1;
    }
    int queue_size = atoi(argv[2]);
    if (queue_size <= 0) {
        printf("Error: Work queue size must be positive\n");
        return 1;
    }

    printf("SMOCK - Simple Matrix Operations for C Knowledge\n");
    printf("Commands:\n");
    printf("  new <nrows> <ncols>: Create new <nrows> x <ncols> matrix\n");
    printf("  put <i> <j> <value>: Change entry (i,j) of current matrix\n");
    printf("  get <i> <j>: Retrieve entry (i,j) of current matrix\n");
    printf("  print: Print out entries in the current matrix\n");
    printf("  sum: Compute and print out sum of all elements in current matrix\n");
    printf("  max: Compute and print out maximum of all elements in current matrix\n");
    printf("  clear: Delete current matrix\n");
    printf("  read_text <file_name>: Read a matrix from a text file\n");
    printf("  parallel_sum <n_threads>: Compute matrix sum with multiple threads\n");
    printf("  parallel_max <n_threads>: Compute matrix max with multiple threads\n");
    printf("  parallel_sum_pool: Compute matrix sum with pre-existing worker threads\n");
    printf("  exit: Quit this program\n");

    char input[MAX_INPUT_LEN];
    matrix_t *mat = NULL;
    worker_pool_t workers;
    if (worker_pool_init(&workers, num_workers, queue_size) == -1) {
        return 1;
    }

    while (1) { // Keep reading until we break out of loop
        printf("%s", PROMPT);

        if (scanf("%s", input) == EOF) {
            printf("\n");
            break;
        }

        if (strcmp("exit", input) == 0) {
            break;
        }

        else if (strcmp("new", input) == 0) {
            // Always need to read in nrows and ncols
            unsigned nrows;
            unsigned ncols;
            scanf("%u %u", &nrows, &ncols);
            if (mat != NULL) {
                printf("Error: You must clear the current matrix first\n");
            } else {
                mat = matrix_init(nrows, ncols);
                if (mat == NULL) {
                    printf("Matrix creation failed\n");
                } else {
                    int val;
                    for (int i = 0; i < nrows; i++) {
                        for (int j = 0; j < ncols; j++) {
                            scanf("%d", &val);
                            matrix_put(mat, i , j, val);
                        }
                    }
                }
            }
        }

        else if (strcmp("clear", input) == 0) {
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                matrix_free(mat);
                mat = NULL;
            }
        }

        else if (strcmp("print", input) == 0) {
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                for (int i = 0; i < mat->nrows; i++) {
                    printf("  ");
                    for (int j = 0; j < mat->ncols; j++) {
                        printf("%d ", matrix_get(mat, i, j));
                    }
                    printf("\n");
                }
            }
        }

        else if (strcmp("get", input) == 0) {
            // Still have to read these even if no active matrix
            unsigned i;
            unsigned j;
            scanf("%u %u", &i, &j);
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                printf("%d\n", matrix_get(mat, i, j));
            }
        }

        else if (strcmp("put", input) == 0) {
            // Still have to read these even if no active matrix
            unsigned i;
            unsigned j;
            int val;
            scanf("%u %u %d", &i, &j, &val);
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                matrix_put(mat, i, j, val);
            }
        }

        else if (strcmp("sum", input) == 0) {
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                printf("%ld\n", matrix_sum(mat));
            }
        }

        else if (strcmp("max", input) == 0) {
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                printf("%ld\n", matrix_max(mat));
            }
        }

        else if (strcmp("read_text", input) == 0) {
            scanf("%s", input); // Read in file name
            if (mat != NULL) {
                printf("Error: You must clear the current matrix first\n");
            } else {
                mat = matrix_read_text(input);
                if (mat == NULL) {
                    printf("Failed to read matrix from text file\n");
                } else {
                    printf("Matrix successfully read from text file\n");
                }
            }
        }

        else if (strcmp("parallel_sum", input) == 0) {
            unsigned n_threads;
            scanf("%u", &n_threads); // Read in desired number of threads
            if (n_threads == 0) {
                printf("Error: Invalid n_threads argument\n");
            } else if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                long result;
                if (matrix_parallel_sum(mat, n_threads, &result) == -1) {
                    printf("Matrix parallel sum failed\n");
                } else {
                    printf("%ld\n", result);
                }
            }
        }

        else if (strcmp("parallel_max", input) == 0) {
            unsigned n_threads;
            scanf("%u", &n_threads); // Read in desired number of threads
            if (n_threads == 0) {
                printf("Error: Invalid n_threads argument\n");
            } else if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                long result;
                if (matrix_parallel_max(mat, n_threads, &result) == -1) {
                    printf("Matrix parallel max failed\n");
                } else {
                    printf("%ld\n", result);
                }
            }
        }

        else if (strcmp("parallel_sum_pool", input) == 0) {
            long result;
            if (matrix_parallel_sum_pool(mat, &workers, &result) == -1) {
                printf("Parallel matrix sum failed\n");
            } else {
                printf("%ld\n", result);
            }
        }

        else {
            printf("Unknown command'%s'\n", input);
        }
    }

    if (mat != NULL) {
        matrix_free(mat);
    }
    worker_pool_free(&workers);
    return 0;
}
