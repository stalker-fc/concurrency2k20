#include <iostream>
#include "array.h"
#include "quicksort.h"
#include "quicksort_mpi.h"


int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " "
                  << "`path/to/array.txt` "
                  << "mode: [`1` for sequential solution, `2` for mpi solution]"
                  << std::endl;
        std::exit(1);
    }
    struct Array array = get_array(argv[1]);
    int mode = atoi(argv[2]);

    bool is_correct;
    if (mode == 1) {
        quicksort(array.data, 0, array.length - 1);
        is_correct = is_result_correct(array);
    } else if (mode == 2) {
        init_MPI(argc, argv);
        quicksort_mpi(array.data, 0, array.length - 1);
        is_correct = is_result_correct(array);
    } else {
        std::cerr << "Incorrect mode value. It must be 1 or 2." << std::endl;
        std::exit(1);
    }

    if (!is_correct) {
        std::cerr << "Array has sorted incorrectly." << std::endl;
        std::exit(1);
    }
    return 0;
}