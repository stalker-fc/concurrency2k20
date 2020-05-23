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

    std::cout << n_columns << " " << n_rows << std::endl;

    int stime;
    long ltime;

    ltime = std::time(NULL);
    stime = (unsigned int) ltime / 2;
    std::srand(stime);

    double *data_matrix = new double[n_columns * n_rows];

    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_columns; j++) {
            data_matrix[i * n_rows + j] = std::rand() % 10000 / 1000.0;
        }
    }

    for (int i = 0; i < n_rows; i++) {
		for (int j = 0; j < n_columns; j++) {
		    data_matrix[i * n_rows + i] += data_matrix[i * n_rows + j];
		}
	}

    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_columns; j++) {
            std::cout << data_matrix[i * n_rows + j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}