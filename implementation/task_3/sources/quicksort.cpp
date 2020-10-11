#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "utils.h"
#include "quicksort.h"


int partition(int *array, int left_index, int right_index) {
    auto pivot_value = array[left_index];
    auto i = left_index - 1;
    auto j = right_index + 1;
    while (true) {
        do {
            i++;
        } while (array[i] < pivot_value);
        do {
            j--;
        } while (array[j] > pivot_value);

        if (i >= j) {
            return j;
        }
        std::swap(array[i], array[j]);
    }
}

void quicksort(int *array, int left_index, int right_index) {
    if (left_index < right_index) {
        int pivot_index = partition(array, left_index, right_index);
        quicksort(array, left_index, pivot_index);
        quicksort(array, pivot_index + 1, right_index);
    }
}

void sort_array(char *filename) {
    int *array = nullptr;
    int array_length;
    get_array(filename, &array, array_length);

    quicksort(array, 0, array_length - 1);
    bool is_correct = is_result_correct(array, array_length);
    if (!is_correct) {
        std::cerr << "Array has sorted incorrectly." << std::endl;
        std::exit(1);
    }
}
