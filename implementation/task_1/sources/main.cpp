#include <iostream>
#include "matrix.h"


int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " "
                  << "`path/to/first/matrix.txt` `path/to/second/matrix.txt` "
                  << "mode: [`1` for usual multiplication, `2` for openmp multiplication]"
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
        result = multiplication(A, B);
    } else if (mode == 2) {
        result = multiplication_openmp(A, B);
    } else {
        std::cerr << "Incorrect mode value. It must be 1 or 2." << std::endl;
        std::exit(1);
    }
    return 0;
}