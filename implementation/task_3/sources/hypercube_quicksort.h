#ifndef SORT_HYPERCUBE_QUICKSORT_H
#define SORT_HYPERCUBE_QUICKSORT_H



void distribute_data(char *filename, int **array, int &array_length, int **local_array, int &local_array_length);

int get_partition_by_pivot_value(int *array, int left_index, int right_index, int pivot_value);

int get_pivot_value(int *local_array, int local_array_length, int mask, MPI_Request request, MPI_Status status);

void hypercube_quicksort(int *array, int left_index, int right_index);

void parallel_sort_array(char *filename);

#endif //SORT_HYPERCUBE_QUICKSORT_H
