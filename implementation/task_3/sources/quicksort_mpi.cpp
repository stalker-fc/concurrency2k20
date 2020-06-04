#include "mpi.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "quicksort.h"
#include "quicksort_mpi.h"

int procs_rank;
int procs_num;

int get_partition_mpi(int *array, int left_index, int right_index, int pivot) {
    int j = -1;
    int tmp;
    for (std::size_t i = left_index; i < right_index + 1; ++i) {
        //printf( "i: %d \tarr[i]: %lf \t x: %lf \t", i, arr[i], x );
        if (array[i] <= pivot) {
            //printf("true");
            j++;
            //printf("\t\tarr[i]: %lf \tarr[j]: %lf \t\t", arr[i], arr[j]);
            tmp = array[j];
            array[j] = array[i];
            array[i] = tmp;
            //printf("\t\tarr[i]: %lf \tarr[j]: %lf \t\t", arr[i], arr[j]);
        }
        //printf("\n");
    }
    return j;
}

void quicksort_mpi(int *array, int left_index, int right_index) {
    int array_length = right_index - left_index + 1;

    int n_stages = (int) (log(procs_num) / log(2));
    int n_available_procs = (int) (pow(2.0, (double) n_stages));

    int *procs_array_sizes;
    int receive_buffer_size;
    if (procs_rank == 0) {
        procs_array_sizes = (int *) malloc(procs_num * sizeof(int));
        receive_buffer_size = array_length / procs_num;
        for (std::size_t i = 0; i < procs_num - 1; ++i)
            procs_array_sizes[i] = receive_buffer_size;
        procs_array_sizes[procs_num - 1] = array_length - (procs_num - 1) * receive_buffer_size;
    }

    MPI_Scatter(procs_array_sizes, 1, MPI_INT, &receive_buffer_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int *receive_buffer = (int *) malloc(receive_buffer_size * sizeof(int));

    int *procs_array_indices;
    if (procs_rank == 0) {
        procs_array_indices = (int *) malloc(procs_num * sizeof(int));
        procs_array_indices[0] = 0;
        for (std::size_t i = 1; i < n_available_procs; ++i)
            procs_array_indices[i] = procs_array_indices[i - 1] + procs_array_sizes[i - 1];
    }

    MPI_Scatterv(array,
                 procs_array_sizes,
                 procs_array_indices,
                 MPI_INT,
                 receive_buffer,
                 receive_buffer_size,
                 MPI_INT,
                 0,
                 MPI_COMM_WORLD);

    MPI_Request request;
    MPI_Status status;

    //теперь провести log2 стадий
    //if( myID == 0 ){printf("devinded: %d\n", devinded); }


    int partner_procs_rank;
    int send_count;
    int *send_buffer;

    int *cur_recieve_buffer;
    int *new_receive_buffer;
    int cur_receive_count;
    int new_receive_buffer_size;

    int pivot;
    int procs_type;
    for (std::size_t stage_index = 0; stage_index < n_stages; ++stage_index) {
        //разделились на верхние и нижние
        //в type == 0  - меньшие значения
        if (procs_rank % n_available_procs < n_available_procs / 2) {
            procs_type = 0;
        } else {
            procs_type = 1;
        }

        //надо выяснить опорный элемент
        //выяснять будет наименьший в группе
        if (procs_rank % n_available_procs == 0) {
            pivot = 0;
            if (receive_buffer_size != 0) {
                pivot = receive_buffer[0];
            }
            for (std::size_t i = procs_rank + 1; i < procs_rank + n_available_procs; ++i) {
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
            from = procs_rank - procs_rank % n_available_procs;
            MPI_Irecv(&pivot,
                      1,
                      MPI_INT,
                      from,
                      MPI_ANY_TAG,
                      MPI_COMM_WORLD,
                      &request);
        }

        MPI_Wait(&request, &status);

        //сделать partition
        int partition = get_partition_mpi(receive_buffer, 0, receive_buffer_size - 1, pivot);
        //надо обменять половины
        if (procs_type == 0) {
            //собираю меньшие (левые) части
            //моя пара = +n_available_procs/2
            partner_procs_rank = procs_rank + n_available_procs / 2;
            //скажем партнеру, сколько передадим ему
            send_count = receive_buffer_size - partition - 1;

            MPI_Send(&send_count, 1, MPI_INT, partner_procs_rank, 0, MPI_COMM_WORLD);

            //узнаем сколько принимать
            MPI_Recv(&cur_receive_count, 1, MPI_INT, partner_procs_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            //подготовим новый массив побольше
            new_receive_buffer_size = partition + 1 + cur_receive_count;
            new_receive_buffer = (int *) malloc((new_receive_buffer_size) * sizeof(int));
            /*
            if( myID == testID )
                printf( "id: %d \ti: %d \tm: %d \t send_count: %d \tcur_receive_count: %d \tnew_receive_buffer_size: %d\n",
                    myID, i, m, send_count, cur_receive_count, new_receive_buffer_size);
            */

            for (std::size_t j = 0; j < partition + 1; ++j)
                new_receive_buffer[j] = receive_buffer[j];
            /*
            if( myID == testID ) {
                for( j = 0; j < new_receive_buffer_size; ++j ) printf("%lf ", new_receive_buffer[j]);
                printf("\n");
            }
            */
            //и правильный указатель
            send_buffer = receive_buffer;
            if (partition + 1 < receive_buffer_size)
                send_buffer = &(receive_buffer[partition + 1]);

            cur_recieve_buffer = new_receive_buffer;
            if (cur_receive_count > 0)
                cur_recieve_buffer = &(new_receive_buffer[partition + 1]);


            //sendrecv

            MPI_Isend(send_buffer, send_count, MPI_INT,
                      partner_procs_rank, 0,
                      MPI_COMM_WORLD, &request);
            MPI_Irecv(cur_recieve_buffer, cur_receive_count, MPI_INT,
                      partner_procs_rank, MPI_ANY_TAG,
                      MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);

            /*
            if( myID == testID ) {
                for( j = 0; j < new_receive_buffer_size; ++j ) printf("%lf ", new_receive_buffer[j]);
                printf("\n");
            }
            */

            free(receive_buffer);
            receive_buffer = new_receive_buffer;
            receive_buffer_size = new_receive_buffer_size;


        } else {
            //собираю большие, правые части
            //моя пара = -n_available_procs/2
            partner_procs_rank = procs_rank - n_available_procs / 2;

            //узнаем сколько принимать
            MPI_Recv(&cur_receive_count, 1, MPI_INT, partner_procs_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            //скажем паре, сколько передади ему
            send_count = partition + 1;
            MPI_Send(&send_count, 1, MPI_INT, partner_procs_rank, 0, MPI_COMM_WORLD);


            //подготовим новый массив побольше
            new_receive_buffer_size = receive_buffer_size - partition - 1 + cur_receive_count;
            new_receive_buffer = (int *) malloc((new_receive_buffer_size) * sizeof(int));
            /*
            if( myID == testID )
                printf( "id: %d \ti: %d \tm: %d \t send_count: %d \tcur_receive_count: %d \tnew_receive_buffer_size: %d\n",
                    myID, i, m, send_count, cur_receive_count, new_receive_buffer_size);
            */

            for (std::size_t j = 0; j < receive_buffer_size - partition - 1; ++j)
                new_receive_buffer[j] = receive_buffer[j + partition + 1];
            /*
            if( myID == testID ) {
                for( j = 0; j < new_receive_buffer_size; ++j ) printf("%lf ", new_receive_buffer[j]);
                printf("\n");
            }
            */
            //и правильный указатель
            send_buffer = receive_buffer;
            cur_recieve_buffer = new_receive_buffer;
            if (cur_receive_count > 0)
                cur_recieve_buffer = &(new_receive_buffer[receive_buffer_size - partition - 1]);


            //sendrecv
            MPI_Isend(send_buffer, send_count, MPI_INT,
                      partner_procs_rank, 0,
                      MPI_COMM_WORLD, &request);
            MPI_Irecv(cur_recieve_buffer, cur_receive_count, MPI_INT,
                      partner_procs_rank, MPI_ANY_TAG,
                      MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);

            /*
            if( myID == testID ) {
                for( j = 0; j < new_receive_buffer_size; ++j ) printf("%lf ", new_receive_buffer[j]);
                printf("\n");
            }
            */

            free(receive_buffer);
            receive_buffer = new_receive_buffer;
            receive_buffer_size = new_receive_buffer_size;
        }

        n_available_procs = n_available_procs / 2;
        MPI_Barrier(MPI_COMM_WORLD);
    }

    quicksort(receive_buffer, 0, receive_buffer_size - 1);
    MPI_Gather(&receive_buffer_size, 1, MPI_INT, procs_array_sizes, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (procs_rank == 0) {
        procs_array_indices[0] = 0;
        for (std::size_t i = 1; i < procs_num; ++i)
            procs_array_indices[i] = procs_array_indices[i - 1] + procs_array_sizes[i - 1];
    }

    MPI_Gatherv(receive_buffer,
                receive_buffer_size,
                MPI_INT, array,
                procs_array_sizes,
                procs_array_indices,
                MPI_INT,
                0,
                MPI_COMM_WORLD);

    if (procs_rank == 0) {
        free(procs_array_sizes);
        free(procs_array_indices);
    }

    free(receive_buffer);
}

void init_MPI(int argc, char *argv[]) {
    int namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &procs_rank);
    MPI_Get_processor_name(processor_name, &namelen);
}

