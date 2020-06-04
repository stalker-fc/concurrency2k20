#ifndef SORT_QUICKSORT_MPI_H
#define SORT_QUICKSORT_MPI_H

int get_partition_mpi(int* array, int left_index, int right_index, int pivot);
void quicksort_mpi(int* array, int left_index, int right_index);
void init_MPI(int argc, char *argv[]);

#endif //SORT_QUICKSORT_MPI_H
