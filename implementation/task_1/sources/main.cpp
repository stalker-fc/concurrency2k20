#include <iostream>
#include "matrix.h"


int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " "
                  << "`path/to/first/matrix.txt` `path/to/second/matrix.txt` "
                  << "mode: [`1` for sequential multiplication, `2` for openmp multiplication]"
                  << std::endl;
        std::exit(1);
    }
    struct Matrix A = get_matrix_from_file(argv[1]);
    struct Matrix B = get_matrix_from_file(argv[2]);

    int mode = atoi(argv[3]);

    Matrix result{};
    switch (mode) {
        case 1:
            result = get_sequential_result(A, B);
            break;
        case 2:
            result = get_parallel_for_result(A, B);
            break;
        case 3:
            result = get_static_schedule_result(A, B);
            break;
        case 4:
            result = get_dynamic_schedule_result(A, B);
            break;
        case 5:
            result = get_guided_schedule_result(A, B);
            break;
        default:
            std::cerr << "Incorrect mode value. It must be [1, 2, 3, 4, 5], but it was " << mode << "." << std::endl;
            std::exit(1);
    }
    return 0;
}