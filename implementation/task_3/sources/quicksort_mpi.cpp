#include "mpi.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "quicksort_mpi.h"

int procs_rank;
int procs_num;

int get_partition_mpi(int* array, int left_index, int right_index, int pivot){
    int j = -1;
    int tmp;
    for(std::size_t i = left_index; i < right_index + 1; ++i) {
        //printf( "i: %d \tarr[i]: %lf \t x: %lf \t", i, arr[i], x );
        if (array[i] <= pivot){
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

int* quicksort_mpi(int* array, int left_index, int right_index) {
    int array_length = right_index - left_index + 1;

    int stages_num = (int)(log(procs_num)/log(2));
    procNum = (int)(pow(2.0, (double)stages));

    int* procs_array_sizes;
    int receive_buffer_size;
    if (procs_rank == 0) {
        procs_array_sizes = (int*)malloc(procs_num * sizeof(int));
        receive_buffer_size = array_length / procs_num;
        for(std::size_t i = 0; i < procs_num - 1; ++i)
            procs_array_sizes[i] = receive_buffer_size;
        procs_array_sizes[procs_num - 1] = array_length - (procs_num - 1) * receive_buffer_size;
    }

    MPI_Scatter(procs_array_sizes, 1, MPI_INT, &receive_buffer_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int* receive_buffer = (int*)malloc(receive_buffer_size * sizeof(int));

    int *procs_array_indices;
    if (procs_rank == 0) {
        procs_array_indices = (int*)malloc(procs_num * sizeof(int));
        procs_array_indices[0] = 0;
        for(std::size_t i = 1; i < proc; ++i)
            procs_array_indices[i] = procs_array_indices[i - 1] + procs_array_sizes[i - 1];
    }

    MPI_Scatterv(array,
                 procs_array_sizes,
                 procs_array_indices,
                 MPI_INT,
                 receive_buffer,
                 receive_buffer_size,
                 MPI_DOUBLE,
                 0,
                 MPI_COMM_WORLD);

    MPI_Request request;
    MPI_Status status;

    //теперь провести log2 стадий
    int devinded = procNum;
    //if( myID == 0 ){printf("devinded: %d\n", devinded); }

    double x;

    int m;
    int partner_procs_rank;
    int sendcount;
    double * sendbuf;
    int recvcount;
    double * recvbuf;
    double* newrbuf;
    int newrbufSize;

    int pivot;
    int procs_type;
    for (std::size_t stage_index = 0; stage_index < stages_num; ++stage_index) {
        //разделились на верхние и нижние
        //в type == 0  - меньшие значения
        if (procs_rank % devinded < devinded / 2) {
            procs_type = 0;
        } else {
            procs_type = 1;
        }

        //надо выяснить опорный элемент
        //выяснять будет наименьший в группе
        if (procs_rank % devinded == 0) {
            pivot = 0;
            if (receive_buffer_size != 0) {
                pivot = receive_buffer[0];
            }
            for (std::size_t i = procs_rank + 1; i < procs_rank + devinded; ++i) {
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
            from = procs_rank - procs_rank % devinded;
            MPI_Irecv(&pivot,
                      1,
                      MPI_DOUBLE,
                      from,
                      MPI_ANY_TAG,
                      MPI_COMM_WORLD,
                      &request);
        }

        MPI_Wait(&request, &status);

        //сделать partition
        new_pivot = get_partition_1(receive_buffer, 0, receive_buffer_size - 1, pivot);
        //надо обменять половины
        if (procs_type == 0) {
            //собираю меньшие (левые) части
            //моя пара = +devinded/2
            partner_procs_rank = procs_rank + devinded / 2;
            //скажем партнеру, сколько передадим ему
            sendcount = receive_buffer_size - new_pivot - 1;

            MPI_Send(&sendcount, 1, MPI_INT, partner_procs_rank, 0, MPI_COMM_WORLD);

            //узнаем сколько принимать
            MPI_Recv(&recvcount, 1, MPI_INT, partner_procs_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            //подготовим новый массив побольше
            newrbufSize = m+1+recvcount;
            newrbuf = (double*) malloc ((newrbufSize)*sizeof(double));
            /*
            if( myID == testID )
                printf( "id: %d \ti: %d \tm: %d \t sendcount: %d \trecvcount: %d \tnewrbufSize: %d\n",
                    myID, i, m, sendcount, recvcount, newrbufSize);
            */

            for( j = 0; j < m+1; ++j ) newrbuf[j] = rbuf[j];
            /*
            if( myID == testID ) {
                for( j = 0; j < newrbufSize; ++j ) printf("%lf ", newrbuf[j]);
                printf("\n");
            }
            */
            //и правильный указатель
            sendbuf = rbuf;
            if ( m+1 < rbufSize) sendbuf = &(rbuf[m+1]);
            recvbuf = newrbuf;
            if ( recvcount > 0 ) recvbuf = &(newrbuf[m+1] );


            //sendrecv

            MPI_Isend( sendbuf, sendcount, MPI_DOUBLE,
                       partner, 0,
                       MPI_COMM_WORLD, &request);
            MPI_Irecv( recvbuf, recvcount, MPI_DOUBLE,
                       partner, MPI_ANY_TAG,
                       MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);

            /*
            if( myID == testID ) {
                for( j = 0; j < newrbufSize; ++j ) printf("%lf ", newrbuf[j]);
                printf("\n");
            }
            */

            free(rbuf);
            rbuf = newrbuf;
            rbufSize = newrbufSize;


        } else {
            //собираю большие, правые части
            //моя пара = -devinded/2
            partner_procs_rank = procs_rank - devinded / 2;

            //узнаем сколько принимать
            MPI_Recv(&recvcount, 1, MPI_INT, partner, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            //скажем паре, сколько передади ему
            sendcount = m+1;
            MPI_Send(&sendcount, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);


            //подготовим новый массив побольше
            newrbufSize = rbufSize - m - 1 + recvcount;
            newrbuf = (double*) malloc ((newrbufSize)*sizeof(double));
            /*
            if( myID == testID )
                printf( "id: %d \ti: %d \tm: %d \t sendcount: %d \trecvcount: %d \tnewrbufSize: %d\n",
                    myID, i, m, sendcount, recvcount, newrbufSize);
            */

            for( j = 0; j < rbufSize - m - 1; ++j ) newrbuf[j] = rbuf[j+m+1];
            /*
            if( myID == testID ) {
                for( j = 0; j < newrbufSize; ++j ) printf("%lf ", newrbuf[j]);
                printf("\n");
            }
            */
            //и правильный указатель
            sendbuf = rbuf;
            recvbuf = newrbuf;
            if (recvcount > 0) recvbuf = &(newrbuf[rbufSize - m - 1]);


            //sendrecv
            MPI_Isend( sendbuf, sendcount, MPI_DOUBLE,
                       partner, 0,
                       MPI_COMM_WORLD, &request);
            MPI_Irecv( recvbuf, recvcount, MPI_DOUBLE,
                       partner, MPI_ANY_TAG,
                       MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);

            /*
            if( myID == testID ) {
                for( j = 0; j < newrbufSize; ++j ) printf("%lf ", newrbuf[j]);
                printf("\n");
            }
            */

            free(rbuf);
            rbuf = newrbuf;
            rbufSize = newrbufSize;
        }

        devinded = devinded / 2;
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
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &procs_rank);
    MPI_Get_processor_name(processor_name, &namelen);
}

