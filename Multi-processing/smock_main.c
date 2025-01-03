#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

#define MAX_INPUT_LEN 128
#define PROMPT ">> "

int main(int argc, char *argv[]) {
    int file_given = argc-1; //Two arguments means file was given; One means it wasn't.
    FILE *input_file;
    if (file_given) {
        input_file = fopen(argv[1],"r"); //Second argument is given for the file name.
        if (input_file == NULL) {
            printf("Failed to open script file");
            return -1;
        }
    } else {
        //Only print out statements when file is not given.
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
        printf("  parallel_sum <n_procs>: Compute matrix sum with multiple processes\n");
        printf("  parallel_max <n_procs>: Compute matrix max with multiple processes\n");
        printf("  exit: Quit this program\n");
        input_file = stdin; //Any file reference will only reference the terminal.
    }
    char input[MAX_INPUT_LEN];
    matrix_t *mat = NULL;
    while (1) { // Keep reading until we break out of loop
        if (!file_given)
            printf("%s", PROMPT);

        if (fscanf(input_file,"%s", input) == EOF) {
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
            fscanf(input_file,"%u %u", &nrows, &ncols);
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
                            fscanf(input_file,"%d", &val);
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
            fscanf(input_file,"%u %u", &i, &j);
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
            fscanf(input_file,"%u %u %d", &i, &j, &val);
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
                printf("%d\n", matrix_max(mat));
            }
        }

        else if (strcmp("write_text", input) == 0) {
            fscanf(input_file,"%s", input); // Read in file name
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                if (matrix_write_text(mat, input) != 0) {
                    printf("Failed to write matrix to text file\n");
                } else {
                    printf("Matrix successfully written to text file\n");
                }
            }
        }

        else if (strcmp("read_text", input) == 0) {
            fscanf(input_file,"%s", input); // Read in file name
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

        else if (strcmp("write_bin", input) == 0) {
            fscanf(input_file,"%s", input); // Read in file name
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else {
                if (matrix_write_bin(mat, input) != 0) {
                    printf("Failed to write matrix to binary file\n");
                } else {
                    printf("Matrix successfully written to binary file\n");
                }
            }
        }

        else if (strcmp("read_bin", input) == 0) {
            fscanf(input_file,"%s", input); // Read in file name
            if (mat != NULL) {
                printf("Error: You must clear the current matrix first\n");
            } else {
                mat = matrix_read_bin(input);
                if (mat == NULL) {
                    printf("Failed to read matrix from binary file\n");
                } else {
                    printf("Matrix successfully read from binary file\n");
                }
            }
        }

        else if (strcmp("parallel_sum", input) == 0) {
            // Still have to read these even if no active matrix
            unsigned n_procs;
            int result;
            fscanf(input_file,"%d",&n_procs);
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else if (n_procs == 0) {
                printf("Error: Invalid n_procs argument\n");
            } else {
                matrix_parallel_sum(mat,n_procs,&result);
                printf("%d\n",result);
            }
        }

        else if (strcmp("parallel_max", input) == 0) {
            // Still have to read these even if no active matrix
            unsigned n_procs;
            int result;
            fscanf(input_file,"%d",&n_procs);
            if (mat == NULL) {
                printf("Error: There is no active matrix\n");
            } else if (n_procs == 0) {
                printf("Error: Invalid n_procs argument\n");
            } else {
                matrix_parallel_max(mat,n_procs,&result);
                printf("%d\n", result);
            }
        }

        else {
            printf("Unknown command'%s'\n", input);
        }
    }

    if (mat != NULL) {
        matrix_free(mat);
    }

    fclose(input_file);
    return 0;
}
