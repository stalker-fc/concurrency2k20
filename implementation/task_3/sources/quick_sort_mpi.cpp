#include "mpi.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>


int procs_rank;
int procs_count;

struct Array {
    std::size_t length;
    int* data;
};

int get_partition(int* array, int left_index, int right_index) {
    int pivot = array[left_index];
    int i = left_index - 1;
    int j = right_index + 1;
    while (true) {
		do {
			i++;
		} while (array[i] < pivot);
		do {
			j--;
		} while (array[j] > pivot);

        if (i >= j) {
			return j;
		}
		array[i] += array[j];
		array[j] = array[i] - array[j];
		array[i] -= array[j];
	}

    return 0;
}

void quick_sort(int* array, int left_index, int right_index) {
    if (left_index < right_index)
	{
		int pivot = get_partition(array, left_index, right_index);
		quick_sort(array, left_index, pivot);
		quick_sort(array, pivot + 1, right_index);
	}
}


//int* quick_sort_mpi(int* array, int array_length) {
//    int local_array_length = array_length / procs_count;
//
//}



bool is_result_correct(int *sorted_array, int array_length) {
    for (std::size_t i = 0; i < array_length; ++i)
	    std::cout << sorted_array[i] << ' ';

	for (std::size_t i = 1; i < array_length; ++i) {
        if (sorted_array[i] < sorted_array[i - 1])
			return false;
	}

	return true;
}

Array get_array(char* filename) {
    std::ifstream input;
    input.open(filename);
    if(!input) {
        std::cout << "Unable to open file";
        std::exit(1);
    }

    int *data = NULL;

    int BUFFER_SIZE = 100;
    int *buffer = new int[BUFFER_SIZE];
    int current_buffer_length = 0;
    std::size_t total_array_length = 0;

    while (!input.eof()) {
        input >> buffer[current_buffer_length];
        if (current_buffer_length == BUFFER_SIZE) {
            current_buffer_length = 0;
            if (data) {
                data = (int*)realloc(data, sizeof(int) * total_array_length);
                memcpy(&data[total_array_length - current_buffer_length], buffer, sizeof(int) * BUFFER_SIZE);
            } else {
                data = (int*)malloc(sizeof(int) * BUFFER_SIZE);
                memcpy(data, buffer, sizeof(int) * BUFFER_SIZE);
            }
        } else {
            current_buffer_length++;
            total_array_length++;
        }
    }

    if (current_buffer_length > 0) {
        if (data) {
            data = (int*)realloc(data, sizeof(int) * total_array_length);
            memcpy(&data[total_array_length - current_buffer_length], buffer, sizeof(int) * current_buffer_length);
        } else {
            data = (int*)malloc(sizeof(int) * current_buffer_length);
            memcpy(data, buffer, sizeof(int) * current_buffer_length);
        }
    }
    Array array;
    array.length = total_array_length;
    array.data = data;
    return array;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " " << "Path to file" << std::endl;
        std::exit(1);
    }
    struct Array array = get_array(argv[1]);

    int namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &procs_rank);
    MPI_Get_processor_name(processor_name, &namelen);

    auto start_time = std::chrono::steady_clock::now();
    quick_sort(array.data, 0, array.length - 1);
    auto end_time = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " ms" << std::endl;

    bool is_correct = is_result_correct(array.data, array.length);
    if (!is_correct) {
        std::cerr << "Array has sorted incorrectly." << std::endl;
        exit(1);
    }
    return 0;
}