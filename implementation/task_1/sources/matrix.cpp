#include <omp.h>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "matrix.h"


Matrix get_matrix_from_file(char* filename) {
    std::ifstream input;
    input.open(filename);
    if(!input) {
        std::cout << "Unable to open file";
        std::exit(1);
    }
    int n_columns;
    int n_rows;
    input >> n_columns;
    input >> n_rows;

    double* data = new double[n_columns * n_rows];
    for (std::size_t i = 0; i < n_columns * n_rows; ++i)
        input >> data[i];

    Matrix matrix;
    matrix.n_rows = n_rows;
    matrix.n_columns = n_columns;
    matrix.data = data;

    return matrix;
}

void print_matrix(Matrix matrix) {
    std::cout << matrix.n_columns << " " << matrix.n_columns << std::endl;
    for (int i = 0; i < matrix.n_rows; i++) {
        for (int j = 0; j < matrix.n_columns; j++) {
            std::cout << std::fixed << std::setprecision(3) << matrix.data[i*matrix.n_columns + j] << " ";
        }
        std::cout << std::endl;
    }
}

Matrix multiplication(Matrix A, Matrix B) {
    double *data = new double[A.n_rows * B.n_columns];

    double dot;
    for (int i = 0; i < A.n_rows; i++) {
        for (int j = 0; j < B.n_columns; j++) {
            dot = 0.0;
            for (int k = 0; k < A.n_columns; k++) {
                dot += A.data[i * A.n_columns + k] * B.data[k * B.n_columns + j];
            }
            data[i * B.n_columns + j] = dot;
        }
    }

    Matrix result;
    result.n_columns = B.n_columns;
    result.n_rows = A.n_rows;
    result.data = data;
    return result;
}


Matrix multiplication_openmp(Matrix A, Matrix B) {
    double *data = new double[A.n_rows * B.n_columns];

    omp_set_num_threads(4);

    #pragma omp parallel
    {
        int i, j, k;
        double dot = 0.0;
        #pragma omp for
        for (i = 0; i < A.n_rows; i++) {
            for (j = 0; j < B.n_columns; j++) {
                dot = 0.0;
                for (int k = 0; k < A.n_columns; k++) {
                    dot += A.data[i * A.n_columns + k] * B.data[k * B.n_columns + j];
                }
                data[i * B.n_columns + j] = dot;
            }
        }
    }

    Matrix result;
    result.n_columns = B.n_columns;
    result.n_rows = A.n_rows;
    result.data = data;
    return result;
}

