#include <iostream>
#include "Matrix.h"
#include "MatrixMultiplier.h"


MatrixMultiplier::MatrixMultiplier(Matrix *a, Matrix *b): A(a), B(b), n_rows(A->n_rows), n_columns(B->n_columns) {}

double MatrixMultiplier::calculate_cell_value(int row_idx, int column_idx) const {
    double result = 0.0;
    for (int i = 0; i < n_columns; ++i) {
        result += A->data[A->n_columns * row_idx + i] * B->data[B->n_columns * i + column_idx];
    }
    return result;
}

Matrix MatrixMultiplier::get_sequential_result() {
    Matrix result = Matrix(n_rows, n_columns);

    for (std::size_t row_idx = 0; row_idx < n_rows; ++row_idx) {
        for (std::size_t column_idx = 0; column_idx < n_columns; ++column_idx) {
            result.data[n_columns * row_idx + column_idx] = calculate_cell_value(row_idx, column_idx);
        }
    }

    return result;
}


Matrix MatrixMultiplier::get_parallel_for_result() {
    double *data = new double[n_rows * n_columns];

#pragma omp parallel
    {
        int row_idx, column_idx;
#pragma omp for
        for (row_idx = 0; row_idx < n_rows; ++row_idx) {
            for (column_idx = 0; column_idx < n_columns; ++column_idx) {
                data[n_columns * row_idx + column_idx] = calculate_cell_value(row_idx, column_idx);
            }
        }
    }

    Matrix result = Matrix(n_rows, n_columns, data);
    delete[] data;
    return result;
}


Matrix MatrixMultiplier::get_static_schedule_result() {
    double *data = new double[n_rows * n_columns];

    int n_cells = n_rows * n_columns;
#pragma omp parallel for schedule(static)
    for (int cell_idx = 0; cell_idx < n_cells; ++cell_idx) {
        int row_idx = cell_idx / n_columns;
        int column_idx = cell_idx % n_columns;
        data[row_idx * n_columns + column_idx] = calculate_cell_value(row_idx, column_idx);
    }

    Matrix result = Matrix(n_rows, n_columns, data);
    delete[] data;
    return result;
}


Matrix MatrixMultiplier::get_dynamic_schedule_result() {
    double *data = new double[n_rows * n_columns];

    int n_cells = n_rows * n_columns;
#pragma omp parallel for schedule(dynamic, 100)
    for (int cell_idx = 0; cell_idx < n_cells; ++cell_idx) {
        int row_idx = cell_idx / n_columns;
        int column_idx = cell_idx % n_columns;
        data[row_idx * n_columns + column_idx] = calculate_cell_value(row_idx, column_idx);
    }

    Matrix result = Matrix(n_rows, n_columns, data);
    delete[] data;
    return result;
}


Matrix MatrixMultiplier::get_guided_schedule_result() {
    double *data = new double[n_rows * n_columns];

    int n_cells = n_rows * n_columns;
#pragma omp parallel for schedule(guided, 100)
    for (int cell_idx = 0; cell_idx < n_cells; ++cell_idx) {
        int row_idx = cell_idx / n_columns;
        int column_idx = cell_idx % n_columns;
        data[row_idx * n_columns + column_idx] = calculate_cell_value(row_idx, column_idx);
    }

    Matrix result = Matrix(n_rows, n_columns, data);
    delete[] data;
    return result;
}
