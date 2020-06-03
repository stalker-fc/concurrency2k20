#include "mpi.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "jacobi.h"


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

    double *data = new double[n_columns * n_rows];
    for (std::size_t i = 0; i < n_columns * n_rows; ++i)
        input >> data[i];

    Matrix matrix;
    matrix.n_rows = n_rows;
    matrix.n_columns = n_columns;
    matrix.data = data;

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

    double *data = new double[len];
    for (std::size_t i = 0; i < len; ++i) {
        input >> data[i];
    }
    Vector vector;
    vector.len = len;
    vector.data = data;

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
    double *data = new double[b.len];

    double dot;
    for (std::size_t i = 0; i < A.n_columns; i++) {
        dot = 0.0;
        for (std::size_t j = 0; j < b.len; j++)
            dot += A.data[i * A.n_columns + j] * b.data[j];
        data[i] = dot;
    }
    Vector result;
    result.len = b.len;
    result.data = data;
    return result;
}

Matrix multiplicate_matrix_by_matrix(Matrix &A, Matrix &B) {
    double *data = new double[A.n_rows * B.n_columns];

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

    Matrix result;
    result.n_columns = B.n_columns;
    result.n_rows = A.n_rows;
    result.data = data;
    return result;
}

Matrix substract_matrix_from_matrix(Matrix &A, Matrix &B) {
    double *data = new double[A.n_columns * A.n_rows];

    for (std::size_t i = 0; i < A.n_rows * A.n_columns; i++)
        data[i] = A.data[i] - B.data[i];

    Matrix result;
    result.n_rows = A.n_rows;
    result.n_columns = A.n_columns;
    result.data = data;
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
    if (answer.data) {
        delete[] answer.data;
    }

    return res;
}

Vector solve_system_of_linear_equations(Matrix data_matrix, Vector data_vector, double eps) {
    int N_ROWS = data_matrix.n_rows;
    int N_COLUMNS = data_matrix.n_columns;

    double *A_data = new double[N_COLUMNS * N_ROWS];
    double *D_data = new double[N_COLUMNS * N_ROWS];
    double *invD_data = new double[N_COLUMNS * N_ROWS];

    for (std::size_t i = 0; i < N_COLUMNS * N_ROWS; ++i) {
        A_data[i] = data_matrix.data[i];
        D_data[i] = 0.0;
        invD_data[i] = 0.0;
    }

    double *x_data = new double[data_vector.len];
    double *x1_data = new double[data_vector.len];
    double *b_data = new double[data_vector.len];

    for (std::size_t i = 0; i < data_vector.len; ++i) {
        b_data[i] = data_vector.data[i];
        x1_data[i] = 0.0;
        x_data[i] = 0.0;
    }

    for (std::size_t i = 0; i < N_COLUMNS; ++i) {
        D_data[i * N_COLUMNS + i] = A_data[i * N_COLUMNS + i];
        invD_data[i * N_COLUMNS + i] = 1.0 / A_data[i * N_COLUMNS + i];
    }

    Matrix A;
    A.n_columns = N_COLUMNS;
    A.n_rows = N_ROWS;
    A.data = A_data;

    Matrix D;
    D.n_columns = N_COLUMNS;
    D.n_rows = N_ROWS;
    D.data = D_data;

    Matrix invD;
    invD.n_columns = N_COLUMNS;
    invD.n_rows = N_ROWS;
    invD.data = invD_data;

    Vector b;
    b.len = data_vector.len;
    b.data = b_data;

    struct Matrix substraction = substract_matrix_from_matrix(D, A);
    struct Matrix B = multiplicate_matrix_by_matrix(invD, substraction);
    struct Vector g = multiplicate_matrix_by_vector(invD, b);

    if (A.data) {
        delete[] A.data;
    }
    if (D.data) {
        delete[] D.data;
    }
    if (invD.data) {
        delete[] invD.data;
    }
    if (b.data) {
        delete[] b.data;
    }
    if (substraction.data) {
        delete[] substraction.data;
    }

    int k = 0;
    double dot;
    double delta;
    do {
        k++;
        for (std::size_t i = 0; i < N_COLUMNS; ++i) {
            dot = 0.0;
            for (std::size_t j = 0; j < N_ROWS; ++j)
                dot += B.data[i * N_COLUMNS + j] * x1_data[j];
            x_data[i] = dot;
        }
        for (std::size_t i = 0; i < N_ROWS; ++i)
            x_data[i] += g.data[i];

        delta = 0.0;
        for (std::size_t i = 0; i < N_ROWS; ++i)
            delta += (x_data[i] - x1_data[i]) * (x_data[i] - x1_data[i]);
        delta = std::sqrt(delta);
        for (std::size_t i = 0; i < N_ROWS; ++i)
            x1_data[i] = x_data[i];

        if (delta == std::numeric_limits<double>::infinity()) {
            std::cerr << "Incorrect input data. System of linear equations can`t be solved." << std::endl;
            exit(1);
        }
    } while (delta > eps);

    Vector x;
    x.len = N_ROWS;
    x.data = x_data;

    return x;
}

Vector solve_system_of_linear_equations_mpi(Matrix data_matrix, Vector data_vector, double eps) {
    int N_ROWS = data_matrix.n_rows;
    int N_COLUMNS = data_matrix.n_columns;
    int local_N_ROWS = data_vector.len / procs_num;

    // declare data for main process
    double *x_data = new double[N_ROWS];
    double *B_data;
    double *g_data;

    // declare data for child processes
    double *proc_B_data = new double[N_COLUMNS * local_N_ROWS];
    double *proc_g_data = new double[local_N_ROWS];
    double *proc_x_data = new double[local_N_ROWS];

    if (procs_rank == 0) {
        double *A_data = new double[N_COLUMNS * N_ROWS];
        double *D_data = new double[N_COLUMNS * N_ROWS];
        double *invD_data = new double[N_COLUMNS * N_ROWS];

        for (std::size_t i = 0; i < N_COLUMNS * N_ROWS; ++i) {
            A_data[i] = data_matrix.data[i];
            D_data[i] = 0.0;
            invD_data[i] = 0.0;
        }

        double *b_data = new double[N_ROWS];
        x_data = new double[N_ROWS];
        for (std::size_t i = 0; i < N_ROWS; ++i) {
            b_data[i] = data_vector.data[i];
            x_data[i] = 0.0;
        }

        for (std::size_t i = 0; i < N_COLUMNS; ++i) {
            D_data[i * N_COLUMNS + i] = A_data[i * N_COLUMNS + i];
            invD_data[i * N_COLUMNS + i] = 1.0 / A_data[i * N_COLUMNS + i];
        }

        Matrix A;
        A.n_columns = N_COLUMNS;
        A.n_rows = N_ROWS;
        A.data = A_data;

        Matrix D;
        D.n_columns = N_COLUMNS;
        D.n_rows = N_ROWS;
        D.data = D_data;

        Matrix invD;
        invD.n_columns = N_COLUMNS;
        invD.n_rows = N_ROWS;
        invD.data = invD_data;

        Vector b;
        b.len = N_ROWS;
        b.data = b_data;

        struct Matrix substraction = substract_matrix_from_matrix(D, A);
        struct Matrix B = multiplicate_matrix_by_matrix(invD, substraction);
        struct Vector g = multiplicate_matrix_by_vector(invD, b);
        B_data = B.data;
        g_data = g.data;

        if (A.data) {
            delete[] A.data;
        }
        if (D.data) {
            delete[] D.data;
        }
        if (invD.data) {
            delete[] invD.data;
        }
        if (b.data) {
            delete[] b.data;
        }
        if (substraction.data) {
            delete[] substraction.data;
        }
    }


    MPI_Bcast(x_data, N_ROWS, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(B_data, N_COLUMNS * local_N_ROWS, MPI_DOUBLE, proc_B_data, N_COLUMNS * local_N_ROWS, MPI_DOUBLE, 0,
                MPI_COMM_WORLD);
    MPI_Scatter(g_data, local_N_ROWS, MPI_DOUBLE, proc_g_data, local_N_ROWS, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double max_delta;
    double dot;
    do {
        for (std::size_t i = 0; i < local_N_ROWS; i++) {
            dot = 0.0;
            for (std::size_t j = 0; j < N_COLUMNS; j++)
                dot += proc_B_data[i * N_COLUMNS + j] * x_data[j];
            proc_x_data[i] = dot + proc_g_data[i];
        }

        double delta = 0.0;
        for (std::size_t i = 0; i < local_N_ROWS; i++)
            delta += (proc_x_data[i] - x_data[procs_rank * local_N_ROWS + i]) *
                     (proc_x_data[i] - x_data[procs_rank * local_N_ROWS + i]);
        delta = sqrt(delta);
        max_delta = 0.0;

        MPI_Allgather(proc_x_data, local_N_ROWS, MPI_DOUBLE, x_data, local_N_ROWS, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Reduce(&delta, &max_delta, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        MPI_Bcast(&max_delta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        if (max_delta == std::numeric_limits<double>::infinity()) {
            std::cerr << "Incorrect input data. System of linear equations can`t be solved." << std::endl;
            exit(1);
        }
    } while (max_delta > eps);


    if (proc_B_data) {
        delete[] proc_B_data;
    }
    if (proc_g_data) {
        delete[] proc_g_data;
    }
    if (proc_x_data) {
        delete[] proc_x_data;
    }

    if (procs_rank == 0) {
        if (g_data) {
            delete[] g_data;
        }
        if (B_data) {
            delete[] B_data;
        }
    }

    Vector x;
    x.len = data_vector.len;
    x.data = x_data;

    return x;
}

void init_MPI(int argc, char *argv[]) {
    int namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &procs_rank);
    MPI_Get_processor_name(processor_name, &namelen);
}
