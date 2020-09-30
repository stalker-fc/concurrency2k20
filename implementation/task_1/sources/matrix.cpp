#include <omp.h>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "matrix.h"


double calculate_cell_value(Matrix A, Matrix B, int row_idx, int column_idx) {
    double result = 0.0;
    for (std::size_t i = 0; i < A.n_columns; ++i) {
        result += A.data[A.n_columns * row_idx + i] * B.data[B.n_columns * i + column_idx];
    }
    return result;
}

Matrix get_matrix_from_file(char *filename) {
    std::ifstream input;
    input.open(filename);
    if (!input) {
        std::cout << "Unable to open file";
        std::exit(1);
    }
    int n_columns;
    int n_rows;
    input >> n_columns;
    input >> n_rows;

    auto *data = new double[n_columns * n_rows];
    for (std::size_t i = 0; i < n_columns * n_rows; ++i)
        input >> data[i];

    Matrix matrix{n_rows, n_columns, data};

    return matrix;
}

void print_matrix(Matrix matrix) {
    std::cout << matrix.n_columns << " " << matrix.n_columns << std::endl;
    for (std::size_t i = 0; i < matrix.n_rows; i++) {
        for (int j = 0; j < matrix.n_columns; j++) {
            std::cout << std::fixed << std::setprecision(3) << matrix.data[i * matrix.n_columns + j] << " ";
        }
        std::cout << std::endl;
    }
}

Matrix get_sequential_result(Matrix A, Matrix B) {
    auto n_rows = A.n_rows;
    auto n_columns = B.n_columns;
    auto *data = new double[n_rows * n_columns];

    for (std::size_t row_idx = 0; row_idx < n_rows; ++row_idx) {
        for (std::size_t column_idx = 0; column_idx < n_columns; ++column_idx) {
            data[n_columns * row_idx + column_idx] = calculate_cell_value(A, B, row_idx, column_idx);
        }
    }

    Matrix result = {n_rows, n_columns, data};
    return result;
}


Matrix get_parallel_for_result(Matrix A, Matrix B) {
    auto n_rows = A.n_rows;
    auto n_columns = B.n_columns;
    auto *data = new double[n_rows * n_columns];

#pragma omp parallel
    {
        int row_idx, column_idx;
#pragma omp for
        for (row_idx = 0; row_idx < n_rows; ++row_idx) {
            for (column_idx = 0; column_idx < n_columns; ++column_idx) {
                data[n_columns * row_idx + column_idx] = calculate_cell_value(A, B, row_idx, column_idx);
            }
        }
    }

    Matrix result = {n_rows, n_columns, data};
    return result;
}


Matrix get_static_schedule_result(Matrix A, Matrix B) {
    auto n_rows = A.n_rows;
    auto n_columns = B.n_columns;
    auto *data = new double[n_rows * n_columns];

    int n_cells = n_rows * n_columns;
#pragma omp parallel for schedule(static)
    for (int cell_idx = 0; cell_idx < n_cells; ++cell_idx) {
        int row_idx = cell_idx / n_columns;
        int column_idx = cell_idx % n_columns;
        data[row_idx * n_columns + column_idx] = calculate_cell_value(A, B, row_idx, column_idx);
    }

    Matrix result = {n_rows, n_columns, data};
    return result;
}

Matrix get_dynamic_schedule_result(Matrix A, Matrix B) {
    auto n_rows = A.n_rows;
    auto n_columns = B.n_columns;
    auto *data = new double[n_rows * n_columns];

    int n_cells = n_rows * n_columns;
#pragma omp parallel for schedule(dynamic, 100)
    for (int cell_idx = 0; cell_idx < n_cells; ++cell_idx) {
        int row_idx = cell_idx / n_columns;
        int column_idx = cell_idx % n_columns;
        data[row_idx * n_columns + column_idx] = calculate_cell_value(A, B, row_idx, column_idx);
    }

    Matrix result = {n_rows, n_columns, data};
    return result;
}


Matrix get_guided_schedule_result(Matrix A, Matrix B) {
    auto n_rows = A.n_rows;
    auto n_columns = B.n_columns;
    double *data = new double[n_rows * n_columns];

    int n_cells = n_rows * n_columns;
#pragma omp parallel for schedule(guided, 100)
    for (int cell_idx = 0; cell_idx < n_cells; ++cell_idx) {
        int row_idx = cell_idx / n_columns;
        int column_idx = cell_idx % n_columns;
        data[row_idx * n_columns + column_idx] = calculate_cell_value(A, B, row_idx, column_idx);
    }

    Matrix result = {n_rows, n_columns, data};
    return result;
}