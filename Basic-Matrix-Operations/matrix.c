#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

matrix_t *matrix_init(unsigned nrows, unsigned ncols) {
    matrix_t *mat = malloc(sizeof(matrix_t));
    if (mat == NULL) {
        return NULL;
    }
    mat->data = malloc(nrows * sizeof(int *));
    if (mat->data == NULL) {
        free(mat);
        return  NULL;
    }

    for (int i = 0; i < nrows; i++) {
        mat->data[i] = malloc(ncols * sizeof(int));
        if (mat->data[i] == NULL) {
            // Need to free previously allocated matrix rows
            for (int j = 0; j < i; j++) {
                free(mat->data[j]);
            }
            free(mat->data);
            free(mat);
            return NULL;
        }
    }
    mat->nrows = nrows;
    mat->ncols = ncols;

    return mat;
}

void matrix_free(matrix_t *mat) {
    //Need to free each row first.
    for (int row = 0; row < mat->nrows; row++) {
        free(mat->data[row]);
    }
    free(mat->data);
    free(mat);
}

void matrix_put(matrix_t *mat, unsigned i, unsigned j, int val) {
    mat->data[i][j] = val;
}

int matrix_get(const matrix_t *mat, unsigned i, unsigned j) {
    return mat->data[i][j];
}

long matrix_sum(const matrix_t *mat) {
    int sum = 0;
    for (int row = 0; row < mat->nrows; row++) {
        for (int col = 0; col < mat->ncols; col++) {
            sum = sum + mat->data[row][col];
        }
    }
    return sum;
}

int matrix_max(const matrix_t *mat) {
    int max = mat->data[0][0];
    for (int row = 0; row < mat->nrows; row++) {
        for (int col = 0; col < mat->ncols; col++) {
            //Compares each value with current max..
            if (max < mat->data[row][col]) {
                max = mat->data[row][col];
            }
        }
    }
    return max;
}

int matrix_write_text(const matrix_t *mat, const char *file_name) {
    FILE *f = fopen(file_name, "w");
    //Checks for error with file opening.
    if (f == NULL) {
        return -1;
    }

    //prints rows and columns out.
    fprintf(f, "%u %u\n", mat->nrows, mat->ncols);
    //Prints each matrix element out.
    for (int i = 0; i < mat->nrows; i++) {
        for (int j = 0; j < mat->ncols; j++) {
            fprintf(f, "%d ", mat->data[i][j]);
        }
        //Separates each row with a newline.
        fprintf(f, "\n");
    }

    fclose(f);
    return 0;
}

matrix_t *matrix_read_text(const char *file_name) {
    int ncols;
    int nrows;
    matrix_t *inputMat;
    FILE *inputFile = fopen(file_name,"r");
    //Checks for error with file opening.
    if (inputFile == NULL) {
        return NULL;
    }

    //Scans numbers of rows and columns then creates corresponding matrix.
    fscanf(inputFile,"%d %d",&nrows,&ncols);
    inputMat = matrix_init(nrows,ncols);

    //Assigns each element of matrix to corresponding file value.
    for (int row = 0; row < nrows; row++) {
        for (int col = 0; col < ncols; col++) {
            int element;
            fscanf(inputFile, "%d",&element);
            inputMat->data[row][col] = element;
        }
    }

    fclose(inputFile);
    return inputMat;
}

int matrix_write_bin(const matrix_t *mat, const char *file_name) {
    FILE *outputFile = fopen(file_name,"w");
    //Checks for error with file opening.
    if (outputFile == NULL) {
        return -1;
    }

    //Writes out number of matrix rows and columns.
    fwrite(&(mat->nrows), sizeof(unsigned int), 1, outputFile);
    fwrite(&(mat->ncols), sizeof(unsigned int), 1, outputFile);

    //Writes out each matrix element.
    for (int i = 0; i < mat->nrows; i++) {
        for (int j = 0; j < mat->ncols; j++) {
            fwrite(&(mat->data[i][j]), sizeof(int), 1, outputFile);
        }
    }

    fclose(outputFile);
    return 0;
}

matrix_t *matrix_read_bin(const char *file_name) {
    matrix_t *inputMat;
    int newRows;
    int newCols;

    //Checks for error with file opening.
    FILE *inputFile = fopen(file_name, "r");
    if (inputFile == NULL) {
        return NULL;
    }

    //Scans numbers of rows and columns then creates corresponding matrix.
    fread(&newRows, sizeof(unsigned int),1,inputFile);
    fread(&newCols, sizeof(unsigned int),1,inputFile);
    inputMat = matrix_init(newRows,newCols);

    //Checks if matrix creation was successful.
    if (inputMat == NULL) {
        return NULL;
    }

    //Assigns each element of matrix to corresponding file value.
    for (int row = 0; row < newRows; row++) {
        for (int col = 0; col < newCols; col++) {
            fread(&(inputMat->data[row][col]), sizeof(int),1,inputFile);
        }
    }

    fclose(inputFile);
    return inputMat;
}
