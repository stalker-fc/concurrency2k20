#include <iostream>
#include "Matrix.h"
#include "MatrixMultiplier.h"


int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " "
                  << "`path/to/first/matrix.txt` `path/to/second/matrix.txt` "
                  << "modes:" << std::endl
                  << "`1` sequential multiplication" << std::endl
                  << "`2` parallel for multiplication" << std::endl
                  << "`3` parallel for static scheduled multiplication" << std::endl
                  << "`4` parallel for dynamic scheduled multiplication" << std::endl
                  << "`5` parallel for guided scheduled multiplication" << std::endl;
        std::exit(1);
    }

    Matrix A = Matrix(argv[1]);
    Matrix B = Matrix(argv[2]);

    MatrixMultiplier matrix_multiplier = MatrixMultiplier(A, B);
    Matrix result;
    int mode = atoi(argv[3]);
    switch (mode) {
        case 1:
            result = matrix_multiplier.get_sequential_result();
            break;
        case 2:
            result = matrix_multiplier.get_parallel_for_result();
            break;
        case 3:
            result = matrix_multiplier.get_static_schedule_result();
            break;
        case 4:
            result = matrix_multiplier.get_dynamic_schedule_result();
            break;
        case 5:
            result = matrix_multiplier.get_guided_schedule_result();
            break;
        default:
            std::cerr << "Incorrect mode value. It must be [1, 2, 3, 4, 5], but it was " << mode << "." << std::endl;
            std::exit(1);
    }
    return 0;
}