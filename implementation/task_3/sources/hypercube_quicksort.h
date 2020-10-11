#ifndef SORT_HYPERCUBE_QUICKSORT_H
#define SORT_HYPERCUBE_QUICKSORT_H


int get_partition_by_pivot_value(int *array, int left_index, int right_index, int pivot_value);

void init_MPI();

void hypercube_quicksort(int *array, int left_index, int right_index);

#endif //SORT_HYPERCUBE_QUICKSORT_H
