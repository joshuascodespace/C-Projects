#include <limits.h>
#include <math.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include "matrix.h"

int matrix_parallel_sum(const matrix_t *mat, unsigned n_procs, int *result) {
    int total_elements = mat->nrows * mat->ncols;
    *result = 0;
    int my_pipe[2]; 
    if (pipe(my_pipe) == -1) {
        perror("pipe");
        close(my_pipe[0]);
        close(my_pipe[1]);
        return -1;
    }

    for (int i = 0; i < n_procs; i++) {
        pid_t is_child = fork();
        if (is_child < 0) {
            close(my_pipe[0]);
            close(my_pipe[1]);
            return -1;
        } else if (is_child==0) {
            close(my_pipe[0]); //Child never reads from pipe.
            int temp_sum = 0;
            //Determines where child starts reading the matrix from.
            int child_size = ceil((double)total_elements / (double)n_procs);
            int flattened_index = (i * child_size);
            int last_child = 0;
            //Don't want the last child to read past the matrix's end.
            if ((i == n_procs - 1) && (total_elements % n_procs) != 0) {
                last_child = n_procs - (total_elements % n_procs);
            }
            for (int j = 0; j < child_size - last_child; j++) {
                int row = flattened_index / mat->ncols;
                int col = flattened_index - row * mat->ncols;
                temp_sum += mat->data[row][col];
                flattened_index++;
            }
            int is_valid = write(my_pipe[1],&temp_sum,sizeof(int));
            close(my_pipe[1]);
            if (is_valid < 0) {
                exit(-1);
            }
            exit(0); //Don't want child to produce other children.
        }
    }
    close(my_pipe[1]); //No longer need to write out to pipe

    //Each child will end the wait call exactly once 
    //After writing to pipe exactly once
    //And order doesn't matter.
    //So for each child, the parent must wait and read from the pipe exaclty once.
    for (int i = 0; i < n_procs; i++) {
        int status;
        wait(&status); 
        if (status != 0) {
            close(my_pipe[0]);
            return -1;
        }
        int partial_sum;
        status = read(my_pipe[0],&partial_sum,sizeof(int));
        if (status < 0) {
            close(my_pipe[0]);
            return -1;
        }
        *result += partial_sum;
    }    
    close(my_pipe[0]);
    return 0;
}

int matrix_parallel_max(const matrix_t *mat, unsigned n_procs, int *result) {
    int total_elements = mat->nrows * mat->ncols;
    *result = mat->data[0][0];
    int my_pipe[2]; 
    if (pipe(my_pipe) == -1) {
        perror("pipe");
        close(my_pipe[0]);
        close(my_pipe[1]);
        return -1;
    }

    for (int i = 0; i < n_procs; i++) {
        pid_t is_child = fork();
        if (is_child < 0) {
            close(my_pipe[0]);
            close(my_pipe[1]);
            return -1;
        } else if (is_child == 0) {
            close(my_pipe[0]); //Child never reads from pipe.
            int temp_max = mat->data[0][0];
            //Determines where child starts reading the matrix from.
            int child_size = ceil((double)total_elements / (double)n_procs);
            int flattened_index = (i * child_size);
            int last_child = 0;
            //Don't want the last child to read past the matrix's end.
            if ((i == n_procs - 1) && (total_elements % n_procs) != 0) {
                last_child = n_procs - (total_elements % n_procs);
            }
            for (int j = 0; j < child_size - last_child; j++) {
                int row = flattened_index / mat->ncols;
                int col = flattened_index - row * mat->ncols;
                if (temp_max < mat->data[row][col]) {
                    temp_max = mat->data[row][col];
                }    
                flattened_index++;
            }
            int is_valid = write(my_pipe[1],&temp_max,sizeof(int));
            close(my_pipe[1]); 
            if (is_valid < 0) {
                exit(-1);
            }
            exit(0); //Don't want child to produce other children.
        }
    }
    close(my_pipe[1]); //No longer need to write out to pipe

    //Each child will end the wait call exactly once 
    //After writing to pipe exactly once
    //And order doesn't matter.
    //So for each child, the parent must wait and read from the pipe exaclty once.
    for (int i = 0; i < n_procs; i++) {
        int status;
        wait(&status);
        if (status != 0) {
            close(my_pipe[0]);
            return -1;
        }
        int local_max;
        status = read(my_pipe[0],&local_max,sizeof(int));
        if (status < 0) {
            close(my_pipe[0]);
            return -1;
        }
        if (*result < local_max) {
            *result = local_max;
        }
    }    
    close(my_pipe[0]);
    return 0;
}
