#include <iostream>
#include "mpi.h"
#include "utils.h"
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

    int mode = atoi(argv[2]);

    if (mode == 1) {
        sort_array(argv[1]);
    } else if (mode == 2) {
        parallel_sort_array(argv[1]);
    } else {
        std::cerr << "Incorrect mode value. It must be 1 or 2." << std::endl;
        std::exit(1);
    }

    return 0;
}