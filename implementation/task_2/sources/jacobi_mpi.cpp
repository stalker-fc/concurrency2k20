#include "mpi.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>


int procs_rank;
int procs_count;

struct Matrix {
    int n_columns;
    int n_rows;
    double *data;
};

struct Vector {
    int len;
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
    for (std::size_t i = 0; i < n_columns * n_rows; ++i)
        input >> data[i];

    Matrix matrix;
    matrix.n_rows = n_rows;
    matrix.n_columns = n_columns;
    matrix.data = data;

    return matrix;
}

Vector get_vector(char* filename) {
    std::ifstream input;
    input.open(filename);
    if(!input) {
        std::cout << "Unable to open file";
        std::exit(1);
    }
    int len;
    input >> len;

    double* data = new double[len];
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
			dot += A.data[i*A.n_columns + j] * b.data[j];
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
	//Print (data_matrix, data_vector);
	//Print (result);
	for (std::size_t i = 0; i < vector.len; ++i)
	{
	//	printf("%.2f = %.2f\n", ans[i], data_vector[i]);
		if (fabs(answer.data[i] - vector.data[i]) > eps)
		{
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

Vector solve_system_of_linear_equations_mpi(Matrix data_matrix, Vector data_vector, double eps) {
	// declare data for main process
	double *x_data;
	double *x1_data;
    double *B_data;
    double *g_data;

	// declare data for child processes
	int local_n_rows = data_vector.len / procs_count;
	double *proc_B_data = new double[data_matrix.n_columns * local_n_rows];
	double *proc_x_data = new double[local_n_rows];
	double *proc_g_data = new double[local_n_rows];

	if (procs_rank == 0) {
	    double *A_data = new double[data_matrix.n_columns * data_matrix.n_rows];
	    double *D_data = new double[data_matrix.n_columns * data_matrix.n_rows];
	    double *invD_data = new double[data_matrix.n_columns * data_matrix.n_rows];

	    for (std::size_t i = 0; i < data_matrix.n_columns * data_matrix.n_rows; ++i) {
            A_data[i] = data_matrix.data[i];
            D_data[i] = 0.0;
            invD_data[i] = 0.0;
        }

	    double *b_data = new double[data_vector.len];
	    x_data = new double[data_vector.len];
	    x1_data = new double[data_vector.len];
	    for (std::size_t i = 0; i < data_vector.len; ++i) {
            b_data[i] = data_vector.data[i];
            x1_data[i] = 0.0;
            x_data[i] = 0.0;
        }

	    for (std::size_t i = 0; i < data_matrix.n_columns; ++i)	{
            D_data[i * data_matrix.n_columns + i] = A_data[i * data_matrix.n_columns + i];
            invD_data[i * data_matrix.n_columns + i] = 1.0 / A_data[i * data_matrix.n_columns + i];
        }

        Matrix A;
        A.n_columns = data_matrix.n_columns;
        A.n_rows = data_matrix.n_rows;
        A.data = A_data;

        Matrix D;
        D.n_columns = data_matrix.n_columns;
        D.n_rows = data_matrix.n_rows;
        D.data = D_data;

        Matrix invD;
        invD.n_columns = data_matrix.n_columns;
        invD.n_rows = data_matrix.n_rows;
        invD.data = invD_data;

        Vector b;
        b.len = data_vector.len;
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


	MPI_Bcast(pX, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(pB, local_N * N, MPI_DOUBLE, pProcB, local_N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(pG, local_N, MPI_DOUBLE, pProcG, local_N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double t_start = MPI_Wtime();
    double max_delta;
    do{
	    for (int i = 0; i < local_N; i++)
        {
			pProcX[i] = 0.0;
			for (int j = 0; j < N; j++)
				pProcX[i] += pProcB[i*N + j] * pX[j];
			pProcX[i] += pProcG[i];
		}
		//if(procs_rank == 0) 	printf("%f\n", pProcX[2] );

		double delta = 0.0;
		for (int i = 0; i < local_N; i++)
			delta += (pProcX[i] - pX[procs_rank * local_N + i]) * (pProcX[i] - pX[procs_rank * local_N + i]);

		delta = sqrt(delta);
		max_delta = 0.0;

		MPI_Allgather(pProcX, local_N, MPI_DOUBLE, pX, local_N, MPI_DOUBLE, MPI_COMM_WORLD);
		MPI_Reduce(&delta, &max_delta, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
		MPI_Bcast(&max_delta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    } while (max_delta > eps);


    if (pProcG.data) {
        delete[] pProcG.data;
    }
    if (pProcB.data) {
        delete[] pProcB.data;
    }
    if (pProcX.data) {
        delete[] pProcX.data;
    }

	if (procs_rank == 0) {
	    if (pG.data) {
	        delete[] pG.data;
	    }
	    if (pB.data) {
	        delete[] pB.data;
	    }
	}

	return pX;
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " " << "N_COLUMNS N_ROWS" << std::endl;
        std::exit(1);
    }
    struct Matrix A = get_matrix(argv[1]);
    struct Vector b = get_vector(argv[2]);

    int namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &procs_rank);
    MPI_Get_processor_name(processor_name, &namelen);

    auto start_time = std::chrono::steady_clock::now();
    struct Vector solution = solve_system_of_linear_equations_mpi(A, b, 1e-12);
    bool is_correct = is_result_correct(A, b, solution, 1e-12);
    if (!is_correct) {
        std::exit(1);
    }
    auto end_time = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " ms" << std::endl;

    return 0;
}