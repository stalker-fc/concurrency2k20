#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "jacobi.h"
#include "mpi.h"


int procs_rank;
int procs_num;

Matrix get_matrix(char *filename) {
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

Vector get_vector(char *filename) {
    std::ifstream input;
    input.open(filename);
    if (!input) {
        std::cout << "Unable to open file";
        std::exit(1);
    }
    int len;
    input >> len;

    auto *data = new double[len];
    for (std::size_t i = 0; i < len; ++i) {
        input >> data[i];
    }
    Vector vector{len, data};

    return vector;
}

void print_matrix(Matrix matrix) {
    std::cout << matrix.n_columns << " " << matrix.n_columns << std::endl;
    for (std::size_t i = 0; i < matrix.n_rows; i++) {
        for (std::size_t j = 0; j < matrix.n_columns; j++) {
            std::cout << std::fixed << std::setprecision(3) << matrix.data[i * matrix.n_columns + j] << " ";
        }
        std::cout << std::endl;
    }
}

void print_vector(Vector vector) {
    std::cout << vector.len << std::endl;
    for (std::size_t i = 0; i < vector.len; i++) {
        std::cout << std::fixed << std::setprecision(3) << vector.data[i] << std::endl;
    }
}

Vector multiplicate_matrix_by_vector(Matrix &A, Vector &b) {
    auto *data = new double[b.len];

    double dot;
    for (std::size_t i = 0; i < A.n_columns; i++) {
        dot = 0.0;
        for (std::size_t j = 0; j < b.len; j++)
            dot += A.data[i * A.n_columns + j] * b.data[j];
        data[i] = dot;
    }
    Vector result{b.len, data};
    return result;
}

Matrix multiplicate_matrix_by_matrix(Matrix &A, Matrix &B) {
    auto *data = new double[A.n_rows * B.n_columns];

    double dot;
    for (std::size_t i = 0; i < A.n_rows; i++) {
        for (std::size_t j = 0; j < B.n_columns; j++) {
            dot = 0.0;
            for (std::size_t k = 0; k < A.n_columns; k++) {
                dot += A.data[i * A.n_columns + k] * B.data[k * B.n_columns + j];
            }
            data[i * B.n_columns + j] = dot;
        }
    }

    Matrix result{A.n_rows, B.n_columns, data};
    return result;
}

Matrix substract_matrix_from_matrix(Matrix &A, Matrix &B) {
    auto *data = new double[A.n_columns * A.n_rows];

    for (std::size_t i = 0; i < A.n_rows * A.n_columns; i++)
        data[i] = A.data[i] - B.data[i];

    Matrix result{A.n_rows, A.n_columns, data};
    return result;
}


bool is_result_correct(Matrix &matrix, Vector &vector, Vector &result, double eps) {
    double error = 0.0;
    struct Vector answer = multiplicate_matrix_by_vector(matrix, result);
    bool res = true;
    for (std::size_t i = 0; i < vector.len; ++i) {
        if (fabs(answer.data[i] - vector.data[i]) > eps) {
            if (fabs(answer.data[i] - vector.data[i]) > error)
                error = fabs(answer.data[i] - vector.data[i]);
            res = false;
        }
    }
    std::cout << "error = " << error << std::endl;

    delete[] answer.data;
    return res;
}

Vector solve_system_of_linear_equations(Matrix A, Vector b, double eps) {
    int N_ROWS = A.n_rows;
    int N_COLUMNS = A.n_columns;

    auto *D_data = new double[N_COLUMNS * N_ROWS];
    auto *invD_data = new double[N_COLUMNS * N_ROWS];

    for (std::size_t i = 0; i < N_COLUMNS * N_ROWS; ++i) {
        D_data[i] = 0.0;
        invD_data[i] = 0.0;
    }

    auto *x_data = new double[b.len];
    auto *cur_x_data = new double[b.len];

    for (std::size_t i = 0; i < b.len; ++i) {
        cur_x_data[i] = 0.0;
        x_data[i] = 0.0;
    }

    for (std::size_t i = 0; i < N_COLUMNS; ++i) {
        D_data[i * N_COLUMNS + i] = A.data[i * N_COLUMNS + i];
        invD_data[i * N_COLUMNS + i] = 1.0 / A.data[i * N_COLUMNS + i];
    }


    Matrix D{N_ROWS, N_COLUMNS, D_data};
    Matrix invD{N_ROWS, N_COLUMNS, invD_data};

    struct Matrix substraction = substract_matrix_from_matrix(D, A);
    struct Matrix B = multiplicate_matrix_by_matrix(invD, substraction);
    struct Vector g = multiplicate_matrix_by_vector(invD, b);

    delete[] D.data;
    delete[] invD.data;
    delete[] substraction.data;

    double dot;
    double delta;
    do {
        for (std::size_t i = 0; i < N_COLUMNS; ++i) {
            dot = 0.0;
            for (std::size_t j = 0; j < N_ROWS; ++j)
                dot += B.data[i * N_COLUMNS + j] * x_data[j];
            cur_x_data[i] = dot;
        }
        for (std::size_t i = 0; i < N_ROWS; ++i)
            cur_x_data[i] += g.data[i];

        delta = 0.0;
        for (std::size_t i = 0; i < N_ROWS; ++i)
            delta += (x_data[i] - cur_x_data[i]) * (x_data[i] - cur_x_data[i]);
        delta = std::sqrt(delta);
        for (std::size_t i = 0; i < N_ROWS; ++i)
            x_data[i] = cur_x_data[i];

        if (delta == std::numeric_limits<double>::infinity()) {
            std::cerr << "Incorrect input data. System of linear equations can`t be solved." << std::endl;
            exit(1);
        }
    } while (delta > eps);

    Vector x{N_ROWS, x_data};

    return x;
}

Vector solve_system_of_linear_equations_mpi(Matrix A, Vector b, double eps) {
    init_MPI();
    int N_ROWS = A.n_rows;
    int N_COLUMNS = A.n_columns;

    int chunk_size = N_ROWS / procs_num;
    int bonus = N_ROWS % procs_num;
    auto local_rows = new int[procs_num];
    auto local_rows_offsets = new int[procs_num];
    auto local_data_sizes = new int[procs_num];
    auto local_data_offsets = new int[procs_num];
    for (int i = 0; i < procs_num; ++i) {
        local_rows[i] = chunk_size;
        local_rows_offsets[i] = i * chunk_size;
        local_data_sizes[i] = chunk_size * N_COLUMNS;
        local_data_offsets[i] = i * chunk_size * N_COLUMNS;
    }
    local_rows[procs_num - 1] += bonus;
    local_data_sizes[procs_num - 1] += bonus * N_COLUMNS;

    auto local_N_ROWS = local_rows[procs_rank];
    std::cout << "Procs rank: " << procs_rank << "; local_N_ROWS " << local_N_ROWS << std::endl;

    // declare data for main process
    double *B_data;
    double *g_data;

    // declare data for child processes
    auto *proc_B_data = new double[local_data_sizes[procs_rank]];
    auto *proc_g_data = new double[local_N_ROWS];
    auto *proc_x_data = new double[local_N_ROWS];
    auto *x_data = new double[N_ROWS];
    for (std::size_t i = 0; i < N_ROWS; ++i)
        x_data[i] = 0.0;

    if (procs_rank == 0) {
        auto *D_data = new double[N_COLUMNS * N_ROWS];
        auto *invD_data = new double[N_COLUMNS * N_ROWS];

        for (std::size_t i = 0; i < N_COLUMNS * N_ROWS; ++i) {
            D_data[i] = 0.0;
            invD_data[i] = 0.0;
        }

        for (std::size_t i = 0; i < N_COLUMNS; ++i) {
            D_data[i * N_COLUMNS + i] = A.data[i * N_COLUMNS + i];
            invD_data[i * N_COLUMNS + i] = 1.0 / A.data[i * N_COLUMNS + i];
        }

        Matrix D{N_ROWS, N_COLUMNS, D_data};

        Matrix invD{N_ROWS, N_COLUMNS, invD_data};

        struct Matrix substraction = substract_matrix_from_matrix(D, A);
        struct Matrix B = multiplicate_matrix_by_matrix(invD, substraction);
        struct Vector g = multiplicate_matrix_by_vector(invD, b);
        B_data = B.data;
        g_data = g.data;

        delete[] D.data;
        delete[] invD.data;
        delete[] substraction.data;
    }

    MPI_Scatterv(B_data, local_data_sizes, local_data_offsets, MPI_DOUBLE,
                 proc_B_data, local_data_sizes[procs_rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(g_data, local_rows, local_rows_offsets, MPI_DOUBLE,
                 proc_g_data, local_N_ROWS, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double delta;
    double dot;
    do {
        for (std::size_t i = 0; i < local_N_ROWS; ++i) {
            dot = 0.0;
            for (std::size_t j = 0; j < N_COLUMNS; ++j)
                dot += proc_B_data[i * N_COLUMNS + j] * x_data[j];
            proc_x_data[i] = dot + proc_g_data[i];
        }

        // check convergence
        double local_delta = 0.0;
        for (std::size_t i = 0; i < local_N_ROWS; ++i)
            local_delta += (proc_x_data[i] - x_data[local_rows_offsets[procs_rank] + i]) *
                           (proc_x_data[i] - x_data[local_rows_offsets[procs_rank] + i]);
        local_delta = sqrt(local_delta);
        MPI_Allreduce(&local_delta, &delta, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        if (delta == std::numeric_limits<double>::infinity()) {
            std::cerr << "Incorrect input data. System of linear equations can`t be solved." << std::endl;
            exit(1);
        }
        MPI_Allgatherv(proc_x_data, local_N_ROWS, MPI_DOUBLE, x_data, local_rows, local_rows_offsets, MPI_DOUBLE,
                       MPI_COMM_WORLD);
    } while (delta > eps);
    // cleaning memory
    delete[] proc_B_data;
    delete[] proc_g_data;
    delete[] proc_x_data;

    if (procs_rank == 0) {
        delete[] g_data;
        delete[] B_data;
    }

    Vector x{b.len, x_data};
    MPI_Finalize();
    return x;
}

void init_MPI() {
    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &procs_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &procs_rank);
}