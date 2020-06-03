#ifndef MULTIPLICATION_MATRIX_H
#define MULTIPLICATION_MATRIX_H

struct Matrix {
    int n_columns;
    int n_rows;
    double *data;
};

Matrix get_matrix_from_file(char *filename);

void print_matrix(Matrix matrix);

Matrix multiplication(Matrix A, Matrix B);

Matrix multiplication_openmp(Matrix A, Matrix B);

#endif //MULTIPLICATION_MATRIX_H
