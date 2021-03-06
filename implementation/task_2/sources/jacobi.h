#ifndef LINEAR_EQUATIONS_JACOBI_H
#define LINEAR_EQUATIONS_JACOBI_H


struct Matrix {
    int n_rows;
    int n_columns;
    double *data;
};

struct Vector {
    int len;
    double *data;
};

Matrix get_matrix(char *filename);

Vector get_vector(char *filename);

void print_matrix(Matrix matrix);

void print_vector(Vector vector);

Vector multiplicate_matrix_by_vector(Matrix &A, Vector &b);

Matrix multiplicate_matrix_by_matrix(Matrix &A, Matrix &B);

Matrix substract_matrix_from_matrix(Matrix &A, Matrix &B);

bool is_result_correct(Matrix &matrix, Vector &vector, Vector &result, double eps);

Vector solve_system_of_linear_equations(Matrix data_matrix, Vector data_vector, double eps);

Vector solve_system_of_linear_equations_mpi(Matrix data_matrix, Vector data_vector, double eps);

void init_MPI();

#endif //LINEAR_EQUATIONS_JACOBI_H
