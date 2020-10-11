#include <iostream>
#include "mpi.h"
#include "array.h"
#include "quicksort.h"
#include "hypercube_quicksort.h"


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
    MPI_Init(nullptr, nullptr);
    if (mode == 1) {
        quicksort(array.data, 0, array.length - 1);
    } else if (mode == 2) {
        hypercube_quicksort(array.data, 0, array.length - 1);
    } else {
        std::cerr << "Incorrect mode value. It must be 1 or 2." << std::endl;
        std::exit(1);
    }
    MPI_Finalize();
    bool is_correct = is_result_correct(array);
    if (!is_correct) {

        std::cerr << "Array has sorted incorrectly." << std::endl;
        std::exit(1);
    } else {
        std::cout << "Everything is OK!" << std::endl;
    }

    return 0;
}