#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

#define MAX_INPUT_LEN 128
#define PROMPT ">> "

/*
 * This is generally similar to the linked list application you will see in lab 2
 * One big difference is the notion of switching between different matrices in one
 * run of the program.
 * You have to create or load a matrix from a file before you can do things like
 * print, sum, and max operations.
 * Also, the user must explicitly clear the current matrix before they can create
 * or load in a new matrix.
 */
int main(int argc, char *argv[]) {
    printf("SMOCK - Simple Matrix Operations for C Knowledge\n");
    printf("Commands:\n");
    printf("  new <nrows> <ncols>: Create new <nrows> x <ncols> matrix\n");
    printf("  put <i> <j> <value>: Change entry (i,j) of current matrix\n");
    printf("  get <i> <j>: Retrieve entry (i,j) of current matrix\n");
    printf("  print: Print out entries in the current matrix\n");
    printf("  sum: Compute and print out sum of all elements in current matrix\n");
    printf("  max: Compute and print out maximum of all elements in current matrix\n");
    printf("  clear: Delete current matrix\n");
    printf("  write_text <file_name>: Write current matrix to a text file\n");
    printf("  read_text <file_name>: Read a matrix from a text file\n");
    printf("  write_bin <file_name>: Write current matrix to a binary file\n");
    printf("  read_bin <file_name>: Read current matrix from a binary file\n");
    printf("  exit: Quit this program\n");

    char input[MAX_INPUT_LEN];
    matrix_t *mat = NULL;
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
            if (mat != NULL) {
                printf("Error: You must clear the current matrix first\n");
            } else {
                unsigned nrows;
                unsigned ncols;
                scanf("%u %u", &nrows, &ncols);
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

        else if (strcmp("get", input) == 0) {
            // Still have to read these even if no active matrix
            unsigned i;
            unsigned j;
            int val;
            scanf("%u %u", &i, &j);
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                val = matrix_get(mat, i, j);
                printf("%d \n",val);
            }
        } 

        else if (strcmp("sum", input) == 0) {
            int val;
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                val = matrix_sum(mat);
                printf("%d \n",val);
            }
        } 

        else if (strcmp("max", input) == 0) {
            int val;
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                val = matrix_max(mat);
                printf("%d \n",val);
            }
        } 

        else if (strcmp("write_text", input) == 0) {
            // Still have to read these even if no active matrix
            char *file_name = input;
            scanf("%s", file_name);
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                if (matrix_write_text(mat,file_name)) {
                    printf("Failed to write matrix to text file\n");
                } else {
                    printf("Matrix successfully written to text file\n");
                }
            }
        } 

        else if (strcmp("read_text", input) == 0) {
            // Still have to read these even is already an active matrix
            char *file_name = input;
            scanf("%s", file_name);
            if (mat != NULL) {
                printf("Error: You must clear the current matrix first\n");
            } else {
                mat = matrix_read_text(file_name);
                if (mat == NULL) {
                    printf("Failed to read matrix from text file\n");
                } else {
                    printf("Matrix successfully read from text file\n");
                }
            }
        } 

        else if (strcmp("write_bin", input) == 0) {
            // Still have to read these even if no active matrix
            char *file_name = input;
            scanf("%s", file_name);
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                if (matrix_write_bin(mat,file_name)) {
                    printf("Failed to write matrix to binary file\n");
                } else {
                    printf("Matrix successfully written to binary file\n");
                }
            }
        } 

        else if (strcmp("read_bin", input) == 0) {
            // Still have to read these even is already an active matrix
            char *file_name = input;
            scanf("%s", file_name);
            if (mat != NULL) {
                printf("Error: You must clear the current matrix first\n");
            } else {
                mat = matrix_read_bin(file_name);
                if (mat == NULL) {
                    printf("Failed to read matrix from binary file\n");
                } else {
                    printf("Matrix successfully read from binary file\n");
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
                for (int row = 0; row < mat->nrows; row++) {
                    for (int col = 0; col < mat->ncols; col++) {
                        printf("%d ",mat->data[row][col]);
                    }
                    printf("\n");
                }
            }
        } 

        else {
            printf("Unknown command'%s'\n", input);
        }
    }

    if (mat != NULL) {
        matrix_free(mat);
    }
    return 0;
}
