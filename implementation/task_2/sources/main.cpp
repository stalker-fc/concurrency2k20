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
    Vector b;
    A = get_matrix(argv[1]);
    b = get_vector(argv[2]);

    int mode = atoi(argv[3]);

    Vector solution;
    if (mode == 1) {
        struct Vector data = solve_system_of_linear_equations(A, b, 1e-12);
        solution.len = data.len;
        solution.data = data.data;
    } else if (mode == 2) {
        init_MPI(argc, argv);
        struct Vector data = solve_system_of_linear_equations_mpi(A, b, 1e-12);
        solution.len = data.len;
        solution.data = data.data;
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