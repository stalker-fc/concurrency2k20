#include "mpi.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "quicksort.h"
#include "hypercube_quicksort.h"

int procs_rank;
int procs_num;

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

int get_pivot_value(int* local_array, int local_array_length, int mask, MPI_Request request, MPI_Status status) {
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


void hypercube_quicksort(int *array, int left_index, int right_index) {
    init_MPI();
    int array_length = right_index - left_index + 1;
    int mask = procs_num; // используется, как делитель для определения типа процесса (принимающий или отправляющий)

    int hypercube_dimension = (int) ceil(log(procs_num) / log(2));
    if ((procs_num & (procs_num - 1)) != 0) {
        std::cerr << "Total numbers of processes must be a power of two." << std::endl;
        std::exit(1);
    }

    // отправляем части массива процессам
    int *procs_array_lengths;
    int *procs_array_indices;
    if (procs_rank == 0) {
        int chunk_size = array_length / procs_num;
        int bonus = array_length % procs_num;
        procs_array_lengths = new int[procs_num];
        procs_array_indices = new int[procs_num];
        for (int i = 0; i < procs_num; ++i) {
            procs_array_lengths[i] = chunk_size;
            procs_array_indices[i] = i * chunk_size;
        }
        procs_array_lengths[procs_num - 1] += bonus;
    }

    int local_array_length;
    MPI_Scatter(procs_array_lengths, 1, MPI_INT, &local_array_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
    auto *local_array = new int[local_array_length];
    MPI_Scatterv(array, procs_array_lengths, procs_array_indices, MPI_INT,
                 local_array, local_array_length, MPI_INT, 0, MPI_COMM_WORLD);

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
        // в зависимости от номера процесса будут
        //в type == 0  - меньшие значения
        if (procs_rank % mask < mask / 2) {
            procs_type = 0;
        } else {
            procs_type = 1;
        }

        // получаем опорное значение
        pivot_value = get_pivot_value(local_array, local_array_length, mask, request, status);
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


            free(local_array);
            local_array = new_local_array;
            local_array_length = new_local_array_length;


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

            free(local_array);
            local_array = new_local_array;
            local_array_length = new_local_array_length;
        }

        mask = mask >> 1;
        MPI_Barrier(MPI_COMM_WORLD);
    }

    quicksort(local_array, 0, local_array_length - 1);
    MPI_Gather(&local_array_length, 1, MPI_INT, procs_array_lengths, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (procs_rank == 0) {
        procs_array_indices[0] = 0;
        for (std::size_t i = 1; i < procs_num; ++i)
            procs_array_indices[i] = procs_array_indices[i - 1] + procs_array_lengths[i - 1];
    }

    MPI_Gatherv(local_array, local_array_length, MPI_INT,
                array, procs_array_lengths, procs_array_indices,
                MPI_INT, 0, MPI_COMM_WORLD);

    if (procs_rank == 0) {
        free(procs_array_lengths);
        free(procs_array_indices);
    }

    free(local_array);
}

void init_MPI() {
    MPI_Comm_size(MPI_COMM_WORLD, &procs_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &procs_rank);
}

