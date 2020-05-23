#include <iostream>
#include <cstdlib>
#include <ctime>


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " " << "NUM_OF_ELEMENTS" << std::endl;
        std::exit(1);
    }

    int N = atoi(argv[1]);

    int stime;
    long ltime;

    ltime = std::time(NULL);
    stime = (unsigned int) ltime / 2;
    std::srand(stime);

    for (int i = 0; i < N; i++)
        std::cout << std::rand() % (2 * N) << " ";

    return 0;
}