#ifndef SORT_HYPERCUBE_QUICKSORT_H
#define SORT_HYPERCUBE_QUICKSORT_H



void distribute_data(char *filename, int **array, int &array_length, int **local_array, int &local_array_length);

int get_pivot_value_position(int *array, int array_length, int pivot_value);

void merge_data(int *new_array, int *array, int array_length, int *buffer, int buffer_length);

int get_pivot_value(int *local_array, int local_array_length, int mask, MPI_Request request, MPI_Status status);

void hypercube_quicksort(int *array, int left_index, int right_index);

void parallel_sort_array(char *filename);

#endif //SORT_HYPERCUBE_QUICKSORT_H
