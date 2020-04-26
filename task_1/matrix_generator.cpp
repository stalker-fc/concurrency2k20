#include <iostream>
#include <cstdlib>
#include <ctime>


int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " " << "N_COLUMNS N_ROWS" << std::endl;
        std::exit(1);
    }

    int n_columns = atoi(argv[1]);
    int n_rows = atoi(argv[2]);

    std::srand(std::time(NULL));
    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_columns; j++) {
            std::cout << std::rand() % n_columns << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}