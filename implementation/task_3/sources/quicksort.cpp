#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "quicksort.h"


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

void quicksort(int* array, int left_index, int right_index) {
    if (left_index < right_index)
	{
		int pivot = get_partition(array, left_index, right_index);
		quick_sort(array, left_index, pivot);
		quick_sort(array, pivot + 1, right_index);
	}
}
