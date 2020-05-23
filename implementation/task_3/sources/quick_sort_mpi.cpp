#include "mpi.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>


int procs_rank;
int procs_count;

int* quick_sort_mpi(int* array) {
}

bool is_result_correct(int *sorted_array) {
	bool is_correct = true;
	std::size_t n_elems = sizeof(sorted_array) / sizeof(*sorted_array);

	for (std::size_t i = 1; i < n_elems; ++i) {
        if (sorted_array[i] > sorted_array[i - 1)) {
			is_correct = false;
			break;
		}
	}

	return is_correct;
}

int* get_array(char* filename) {
    std::ifstream input;
    input.open(filename);
    if(!input) {
        std::cout << "Unable to open file";
        std::exit(1);
    }

    int BUFFER_SIZE = 1000;
    int *buffer = new double[BUFFER_SIZE];

    int* data;
    for (std::size_t i = 0; i < n_columns * n_rows; ++i)
        input >> data[i];

    Matrix matrix;
    matrix.n_rows = n_rows;
    matrix.n_columns = n_columns;
    matrix.data = data;

    return matrix;
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
    int *solution = quick_sort_mpi();
    auto end_time = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " ms" << std::endl;

    bool is_correct = is_result_correct(solution);
    if (!is_correct) {
        std::cerr << "Array has sorted incorrectly." << std::endl;
        exit(1);
    }
    return 0;
}