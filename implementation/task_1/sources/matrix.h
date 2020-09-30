#ifndef MULTIPLICATION_MATRIX_H
#define MULTIPLICATION_MATRIX_H

struct Matrix {
    int n_rows;
    int n_columns;
    double *data;
};

Matrix get_matrix_from_file(char *filename);

void print_matrix(Matrix matrix);

Matrix get_sequential_result(Matrix A, Matrix B);

Matrix get_parallel_for_result(Matrix A, Matrix B);

Matrix get_static_schedule_result(Matrix A, Matrix B);

Matrix get_dynamic_schedule_result(Matrix A, Matrix B);

Matrix get_guided_schedule_result(Matrix A, Matrix B);

double calculate_cell_value(Matrix A, Matrix B, int row_idx, int column_idx);

#endif //MULTIPLICATION_MATRIX_H