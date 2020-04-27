#include <iostream>
#include <cstdlib>
#include <ctime>


int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " " << "N_ROWS" << std::endl;
        std::exit(1);
    }

    int n_rows = atoi(argv[2]);

    std::cout << n_rows << std::endl;

    int stime;
    long ltime;

    ltime = std::time(NULL);
    stime = (unsigned int) ltime / 2;
    std::srand(stime);

    for (int i = 0; i < n_rows; i++)
        std::cout << std::rand() % 1000 << std::endl;

    return 0;
}