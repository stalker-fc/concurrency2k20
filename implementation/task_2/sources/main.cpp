#include <iostream>
#include "jacobi.h"


int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " "
                  << "`path/to/matrix.txt` `path/to/vector.txt` "
                  << "mode: [`1` for sequential solution, `2` for mpi solution]"
                  << std::endl;
        std::exit(1);
    }
    Matrix A;
    Matrix B;
    A = get_matrix_from_file(argv[1]);
    B = get_matrix_from_file(argv[2]);

    int mode = atoi(argv[3]);

    Matrix result;
    if (mode == 1) {
        struct Vector solution = solve_system_of_linear_equations(A, B, 1e-12);
    } else if (mode == 2) {
        init_MPI(argc, argv);
        struct Vector solution = solve_system_of_linear_equations_mpi(A, B, 1e-12);
    } else {
        std::cerr << "Incorrect mode value. It must be 1 or 2." << std::endl;
        std::exit(1);
    }

    bool is_correct = is_result_correct(A, b, solution, 1e-10);
    if (!is_correct) {
        std::cerr << "Solution hasn`t enough accuracy." << std::endl;
    }
    return 0;
}