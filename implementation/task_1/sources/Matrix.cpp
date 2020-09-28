#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include "Matrix.h"


Matrix::Matrix() {
    n_rows = 0;
    n_columns = 0;
}

Matrix::Matrix(const Matrix &matrix) : n_rows(matrix.n_rows), n_columns(matrix.n_columns) {
    data = new double[n_rows * n_columns];
    for (int i = 0; i < n_rows * n_columns; i++) {
        data[i] = matrix.data[i];
    }
}

Matrix::Matrix(int n_rows, int n_columns) : n_rows(n_rows), n_columns(n_columns) {
    data = new double[n_rows * n_columns];
}

Matrix::Matrix(int n_rows, int n_columns, double *input_data) : n_rows(n_rows), n_columns(n_columns) {
    data = new double[n_rows * n_columns];
    std::memcpy(&data, input_data, n_rows * n_columns * sizeof(double));
}

Matrix::Matrix(char *filename) {
    std::ifstream input;
    input.open(filename);
    if (!input) {
        std::cout << "Unable to open file";
        std::exit(1);
    }
    input >> n_columns;
    input >> n_rows;

    data = new double[n_columns * n_rows];
    for (std::size_t i = 0; i < n_columns * n_rows; ++i)
        input >> data[i];
    input.close();
}

Matrix::~Matrix() {
    if (data)
        delete[] data;
}

void Matrix::print() {
    std::cout << n_rows << " " << n_columns << std::endl;
    for (size_t i = 0; i < n_rows; i++) {
        for (size_t j = 0; j < n_columns; j++) {
            std::cout << std::fixed << std::setprecision(3) << data[i * n_columns + j] << " ";
        }
        std::cout << std::endl;
    }
}