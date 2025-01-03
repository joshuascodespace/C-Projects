#ifndef SMOCK_FUNC_H
#define SMOCK_FUNC_H

/*
 * Matrix data structure
 * data: Two-dimensional integer array (dynamically allocated)
 * nrows: Number of rows in matrix
 * ncols: Number of columns in matrix
 */
typedef struct {
    int **data;
    unsigned nrows;
    unsigned ncols;
} matrix_t;

/*
 * Create a new matrix_t instance
 * 'nrows': Number of rows for new matrix
 * 'ncols': Number of columns for new matrix
 * You do not need to worry about setting matrix elements to initial values
 * Returns a pointer to a new matrix_t on success or NULL on failure
 */
matrix_t *matrix_init(unsigned nrows, unsigned ncols);

/*
 * Free all of the memory associated with a matrix_t instance
 * 'mat': Pointer to matrix instance to free
 */
void matrix_free(matrix_t *mat);

/*
 * Set a matrix element
 * 'mat': Pointer to matrix containing element to modify
 * 'i': Row index of element to modify
 * 'j': Column index of element to modify
 * You may assume that 'i' and 'j' are valid for the dimensions of 'mat'
 */
void matrix_put(matrix_t *mat, unsigned i, unsigned j, int val);

/*
 * Retrieve a matrix element
 * 'mat': Pointer to matrix containing element to retrieve
 * 'i': Row index of element to retrieve
 * 'j': Column index of element to retrieve
 * You may assume that 'i' and 'j' are valid for the dimensions of 'mat'
 * Returns element at row 'i' and column 'j' of 'mat'
 */
int matrix_get(const matrix_t *mat, unsigned i, unsigned j);

/*
 * Computes sum of all matrix elements
 * 'mat': Pointer to matrix containing elements to sum
 * Returns element-wise matrix sum
 */
long matrix_sum(const matrix_t *mat);

/*
 * Computes maximum of all matrix elements
 * 'mat': Pointer to matrix instance
 * Returns maximum element contained in 'mat'
 */
int matrix_max(const matrix_t *mat);

/*
 * Write matrix data to a text file
 * 'mat': Pointer to matrix instance to save
 * 'file_name': String storing name of file to write to
 * Returns 0 on success or -1 on error
 */
int matrix_write_text(const matrix_t *mat, const char *file_name);

/*
 * Read matrix data from a text file
 * 'file_name': String storing name of file to read from
 * Returns pointer to new matrix read from file on success, or NULL on error
 */
matrix_t *matrix_read_text(const char *file_name);

/*
 * Write matrix data to a binary file
 * 'mat': Pointer to matrix instance to save
 * 'file_name': String storing name of file to read from
 * Returns 0 on success or -1 on error
 */
int matrix_write_bin(const matrix_t *mat, const char *file_name);

/*
 * Read matrix data from a binary file
 * 'file_name': String storing name of file to read from
 * Returns pointer to new matrix read from file on success, or NULL on error
 */
matrix_t *matrix_read_bin(const char *file_name);

#endif // SMOCK_FUNC_H
