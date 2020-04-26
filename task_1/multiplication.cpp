#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>


struct Matrix {
    int n_columns;
    int n_rows;
    double *data;
};

Matrix get_matrix(char* filename) {
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
    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_columns; j++) {
            input >> data[i*n_columns + j];
        }
    }
    Matrix matrix;
    matrix.n_rows = n_rows;
    matrix.n_columns = n_columns;
    matrix.data = data;

    return matrix;
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

void print_matrix(Matrix matrix) {
    std::cout << matrix.n_columns << " " << matrix.n_columns << std::endl;
    for (int i = 0; i < matrix.n_rows; i++) {
        for (int j = 0; j < matrix.n_columns; j++) {
            std::cout << std::fixed << std::setprecision(3) << matrix.data[i*matrix.n_columns + j] << " ";
        }
        std::cout << std::endl;
    }
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " " << "N_COLUMNS N_ROWS" << std::endl;
        std::exit(1);
    }
    Matrix A;
    Matrix B;
    A = get_matrix(argv[1]);
    B = get_matrix(argv[2]);

    auto start_time = std::chrono::steady_clock::now();
    Matrix C;
    C = multiplication(A, B);
    auto end_time = std::chrono::steady_clock::now();
    std::cout << "Multiplication random matrices A(" << A.n_columns << " " << A.n_rows << ")  " ;
    std::cout << "and B(" << B.n_columns << " " << B.n_rows << ")  in usual way took: " << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " ms" << std::endl;

    return 0;
}