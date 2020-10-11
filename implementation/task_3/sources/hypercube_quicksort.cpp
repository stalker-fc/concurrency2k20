#include "mpi.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include "quicksort.h"
#include "hypercube_quicksort.h"
#include "utils.h"


int procs_rank;
int procs_num;


void distribute_data(char *filename, int **array, int &array_length, int **local_array, int &local_array_length) {
    int *procs_array_lengths = new int[procs_num];
    int *procs_array_indices = new int[procs_num];;
    if (procs_rank == 0) {
        get_array(filename, array, array_length);
        int chunk_size = array_length / procs_num;
        int bonus = array_length % procs_num;
        for (int i = 0; i < procs_num; ++i) {
            procs_array_lengths[i] = chunk_size;
            procs_array_indices[i] = i * chunk_size;
        }
        procs_array_lengths[procs_num - 1] += bonus;
    }

    MPI_Scatter(procs_array_lengths, 1, MPI_INT, &local_array_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
    *local_array = new int[local_array_length];
    MPI_Scatterv(*array, procs_array_lengths, procs_array_indices, MPI_INT,
                 *local_array, local_array_length, MPI_INT, 0, MPI_COMM_WORLD);

    delete[] procs_array_lengths;
    delete[] procs_array_indices;
}

int get_partition_by_pivot_value(int *array, int left_index, int right_index, int pivot_value) {
    int pivot_index = -1;
    for (std::size_t i = left_index; i < right_index + 1; ++i) {
        if (array[i] <= pivot_value) {
            pivot_index++;
            std::swap(array[pivot_index], array[i]);
        }
    }
    return pivot_index;
}

int get_pivot_value(int *local_array, int local_array_length, int mask, MPI_Request request, MPI_Status status) {
    //вычисление опорного элемента происходит в процессе с меньшими элементами
    int pivot = 0;
    if (procs_rank % mask == 0) {
        pivot = 0;
        if (local_array_length != 0) {
            pivot = local_array[0];
        }
        for (std::size_t i = procs_rank + 1; i < procs_rank + mask; ++i) {
            MPI_Isend(&pivot,
                      1,
                      MPI_INT,
                      i,
                      0,
                      MPI_COMM_WORLD,
                      &request);
        }
    } else {
        int from;
        from = procs_rank - procs_rank % mask;
        MPI_Irecv(&pivot,
                  1,
                  MPI_INT,
                  from,
                  MPI_ANY_TAG,
                  MPI_COMM_WORLD,
                  &request);
    }

    MPI_Wait(&request, &status);
    return pivot;
}


void hypercube_quicksort(int *&local_array, int &local_array_length) {
    int debug_procs = 0;
    int mask = procs_num; // используется, как делитель для определения типа процесса (принимающий или отправляющий)

    int hypercube_dimension = (int) ceil(log(procs_num) / log(2));
    if ((procs_num & (procs_num - 1)) != 0) {
        std::cerr << "Total numbers of processes must be a power of two." << std::endl;
        std::exit(1);
    }

    MPI_Request request;
    MPI_Status status;

    int partner_procs_rank;
    int send_count;
    int *send_buffer;

    int cur_receive_count;
    int *cur_recieve_buffer;

    int *new_local_array;
    int new_local_array_length;

    int pivot_value;
    int procs_type;
    for (std::size_t stage_index = 0; stage_index < hypercube_dimension; ++stage_index) {
        // при `procs_type == 0` в блоке значения элементов массива меньше опорного элемента
        if (procs_rank % mask < mask / 2) {
            procs_type = 0;
        } else {
            procs_type = 1;
        }
        std::cout << "Procs rank " << procs_rank << " Procs type " << procs_type << std::endl;
        // получаем опорное значение
        pivot_value = get_pivot_value(local_array, local_array_length, mask, request, status);
        if (procs_rank == debug_procs) {
            std::cout << "Pivot value: " << pivot_value << std::endl;
        }
        // разбиваем массив на 2 половины согласно значению опорного элемента
        int pivot_index = get_partition_by_pivot_value(local_array, 0, local_array_length - 1, pivot_value);
        // производим обмен половин
        if (procs_type == 0) {
            //собираем части меньшие опорного элемента (левые)
            //моя пара = +mask / 2
            partner_procs_rank = procs_rank + mask / 2;
            //скажем партнеру, сколько передадим ему
            send_count = local_array_length - pivot_index - 1;

            MPI_Send(&send_count, 1, MPI_INT, partner_procs_rank, 0, MPI_COMM_WORLD);

            //узнаем сколько принимать
            MPI_Recv(&cur_receive_count, 1, MPI_INT, partner_procs_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            //подготовим новый массив побольше
            new_local_array_length = pivot_index + 1 + cur_receive_count;
            new_local_array = new int[new_local_array_length];

            for (std::size_t j = 0; j < pivot_index + 1; ++j)
                new_local_array[j] = local_array[j];

            //и правильный указатель
            send_buffer = local_array;
            if (pivot_index + 1 < local_array_length)
                send_buffer = &(local_array[pivot_index + 1]);

            cur_recieve_buffer = new_local_array;
            if (cur_receive_count > 0)
                cur_recieve_buffer = &(new_local_array[pivot_index + 1]);

            //sendrecv
            MPI_Isend(send_buffer, send_count, MPI_INT,
                      partner_procs_rank, 0,
                      MPI_COMM_WORLD, &request);
            MPI_Irecv(cur_recieve_buffer, cur_receive_count, MPI_INT,
                      partner_procs_rank, MPI_ANY_TAG,
                      MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
        } else {
            // собираю части большие опорного элемента (правые)
            // моя пара = -procs_num/2
            partner_procs_rank = procs_rank - procs_num / 2;

            //узнаем сколько принимать
            MPI_Recv(&cur_receive_count, 1, MPI_INT, partner_procs_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            //скажем паре, сколько передади ему
            send_count = pivot_index + 1;
            MPI_Send(&send_count, 1, MPI_INT, partner_procs_rank, 0, MPI_COMM_WORLD);


            //подготовим новый массив побольше
            new_local_array_length = local_array_length - pivot_index - 1 + cur_receive_count;
            new_local_array = (int *) malloc((new_local_array_length) * sizeof(int));

            for (std::size_t j = 0; j < local_array_length - pivot_index - 1; ++j)
                new_local_array[j] = local_array[j + pivot_index + 1];

            //и правильный указатель
            send_buffer = local_array;
            cur_recieve_buffer = new_local_array;
            if (cur_receive_count > 0)
                cur_recieve_buffer = &(new_local_array[local_array_length - pivot_index - 1]);

            //sendrecv
            MPI_Isend(send_buffer, send_count, MPI_INT,
                      partner_procs_rank, 0,
                      MPI_COMM_WORLD, &request);
            MPI_Irecv(cur_recieve_buffer, cur_receive_count, MPI_INT,
                      partner_procs_rank, MPI_ANY_TAG,
                      MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
        }

        delete[] local_array;
        local_array = new_local_array;
        local_array_length = new_local_array_length;

        mask = mask / 2;
        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (procs_rank == debug_procs) {
        std::cout << "second data:" << std::endl;
        for (int i = 0; i < local_array_length; i++)
            std::cout << local_array[i] << " ";
        std::cout << std::endl << "*****" << std::endl;
    }

    quicksort(local_array, 0, local_array_length - 1);
}


void merge_local_arrays(int *array, int array_length, int *local_array, int local_array_length) {
    auto procs_array_lengths = new int[procs_num];
    auto procs_array_indices = new int[procs_num];

    MPI_Gather(&local_array_length, 1, MPI_INT, procs_array_lengths, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (procs_rank == 0) {
        procs_array_indices[0] = 0;
        for (std::size_t i = 1; i < procs_num; ++i)
            procs_array_indices[i] = procs_array_indices[i - 1] + procs_array_lengths[i - 1];
    }

    MPI_Gatherv(local_array, local_array_length, MPI_INT,
                array, procs_array_lengths, procs_array_indices,
                MPI_INT, 0, MPI_COMM_WORLD);


    delete[] procs_array_lengths;
    delete[] procs_array_indices;
}


void parallel_sort_array(char *filename) {
    int *array = nullptr;
    int *local_array = nullptr;
    int array_length, local_array_length;

    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &procs_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &procs_rank);

    distribute_data(filename, &array, array_length, &local_array, local_array_length);
    hypercube_quicksort(local_array, local_array_length);
    merge_local_arrays(array, array_length, local_array, local_array_length);

    if (procs_rank == 0) {
        bool is_correct = is_result_correct(array, array_length);
        if (!is_correct) {
            std::cerr << "Array has sorted incorrectly." << std::endl;
            std::exit(1);
        }
    }
    delete[] array;
    delete[] local_array;
    MPI_Finalize();
}
