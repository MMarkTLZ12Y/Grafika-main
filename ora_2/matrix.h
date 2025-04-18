#ifndef MATRIX_H
#define MATRIX_H

/**
 * Initializes all elements of the matrix to zero.
 */
void init_zero_matrix(float matrix[3][3]);

/**
 * Initializes all elements of the matrix to zero except main diagonal; main diagonal=1.

 */
 void initIdentityMatrix(float matrix[3][3]);

 /**
 * Scalar Mult.
 */

 void scalarMultiplyMatrix(float matrix[3][3], float scalar);



/**
 * Print the elements of the matrix.
 */
void print_matrix(const float matrix[3][3]);

/**
 * Add matrices.
 */
void add_matrices(const float a[3][3], const float b[3][3], float c[3][3]);

#endif // MATRIX_H

