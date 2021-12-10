#include "stdio.h"
#include "stdlib.h"
#include <chrono>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int this_rank, total_ranks;
    MPI_Status status;
    int n = atoi(argv[1]);
    double tau = atof(argv[2]);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &total_ranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &this_rank);
    
    // Параметры
    double T  = 1.0;       // конечное время
    double k  = 1.0;       // коэффициент теплопередачи
    double u0 = 1.0;      // начальная температура
    double l  = 1.0;       // длина стержня
    double h  = l / (n-1); // шаг по пространству

    auto t_start = std::chrono::high_resolution_clock::now();
    auto t_end = std::chrono::high_resolution_clock::now();

    // Main algo
    double* arr; // массив температур (точек на стержне)
    if (this_rank == 0) {
        arr = (double*) calloc(n, sizeof(double));
        std::fill(arr, arr + n, u0);
        arr[0] = 0;
        arr[n-1] = 0;        

        t_start = std::chrono::high_resolution_clock::now(); 
    }

    // массивы с информацией о точках для каждого процесса
    int* lefts   = (int*) calloc(total_ranks, sizeof(int));
    int* rights  = (int*) calloc(total_ranks, sizeof(int));
    int* lengths = (int*) calloc(total_ranks, sizeof(int));

    int batch_len = n / total_ranks; // floor

    // в последний массив записываем остаток
    // т.к. остаток пренебрежительно мал по сравнению
    // с количеством точек на стержне
    int curr_left, curr_right, curr_len;
    for (int rank = 0; rank < total_ranks; rank++) {
        curr_left  = rank * batch_len;
        curr_right = curr_left + batch_len;
        curr_len   = batch_len + 1;
        if (rank > 0) {
            curr_left--;
            curr_len++;
        }
        if (rank == total_ranks - 1) {
            curr_right = n - 1;
            curr_len = curr_right - curr_left + 1;
        }       

        lefts[rank]   = curr_left;
        rights[rank]  = curr_right;
        lengths[rank] = curr_len;
    }
    
    int this_left  = lefts[this_rank];
    int this_right = rights[this_rank];
    int this_len   = lengths[this_rank];

    double* this_arr      = (double*) calloc(this_len, sizeof(double));
    double* this_temp_arr = (double*) calloc(this_len, sizeof(double));

    /* 0-процесс посылает иниц. данные,
       остальные процессы получают */
    if (this_rank == 0) {
        for (int rank = 1; rank < total_ranks; rank++) {
            MPI_Send(arr + lefts[rank], lengths[rank], MPI_DOUBLE, rank, 1, MPI_COMM_WORLD);
        }
        for (int i = this_left; i < this_right; i++)
            this_arr[i] = arr[i];
    } else {
        MPI_Recv(this_arr, this_len, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, NULL);
    }

    // расcчет тепла каждым процессом
    int first = 1;           // "своя" крайняя левая точка
    int last = this_len - 2; // "своя" крайняя правая точка

    int iters_n = T / tau; // кол-во шагов по времени
    double coeff = k * tau / h / h;
    for (int iter = 0; iter <= iters_n; iter++) {
        for (int i = first; i <= last; i++) {
            this_temp_arr[i] = this_arr[i] + coeff * (this_arr[i + 1] - 2 * this_arr[i] + this_arr[i - 1]);
        }

        /* передача граничных точек
           чередуем порядок чтобы избежать дедлока */
        if (this_rank % 2) {
            if (this_rank < total_ranks - 1) {
                MPI_Send(&this_temp_arr[last], 1, MPI_DOUBLE, this_rank + 1, 2, MPI_COMM_WORLD);
            }
            if (this_rank > 0) {
                MPI_Send(&this_temp_arr[first], 1, MPI_DOUBLE, this_rank - 1, 2, MPI_COMM_WORLD);
            }

            if (this_rank < total_ranks - 1) {
                MPI_Recv(&this_temp_arr[this_len - 1], 1, MPI_DOUBLE, this_rank + 1, 2, MPI_COMM_WORLD, NULL);
            }
            if (this_rank > 0) {
                MPI_Recv(&this_temp_arr[0], 1, MPI_DOUBLE, this_rank - 1, 2, MPI_COMM_WORLD, NULL);
            }
        }
        else {
            if (this_rank > 0) {
                MPI_Recv(&this_temp_arr[0], 1, MPI_DOUBLE, this_rank - 1, 2, MPI_COMM_WORLD, NULL);
            }
            if (this_rank < total_ranks - 1) {
                MPI_Recv(&this_temp_arr[this_len - 1], 1, MPI_DOUBLE, this_rank + 1, 2, MPI_COMM_WORLD, NULL);
            }

            if (this_rank > 0) {
                MPI_Send(&this_temp_arr[first], 1, MPI_DOUBLE, this_rank - 1, 2, MPI_COMM_WORLD);
            }
            if (this_rank < total_ranks - 1) {
                MPI_Send(&this_temp_arr[last], 1, MPI_DOUBLE, this_rank + 1, 2, MPI_COMM_WORLD);
            }
        }
        std::swap(this_arr, this_temp_arr);
    }

    // сбор данных с каждого процесса
    if (this_rank == 0) {
        // сбор с нулевого
        for (int i = 0; i <= this_right - 1; i++) {
            arr[i] = this_arr[i];
        }
        // сбор с остальных
        for (int rank = 1; rank < total_ranks - 1; rank++) {
            MPI_Recv(arr + lefts[rank] + 1, lengths[rank] - 2, MPI_DOUBLE, rank, 3, MPI_COMM_WORLD, NULL);
        }
        // сбор с последнего
        if (total_ranks > 1)
            MPI_Recv(arr + lefts[total_ranks - 1] + 1, lengths[total_ranks - 1] - 1, MPI_DOUBLE, total_ranks - 1, 3, MPI_COMM_WORLD, NULL);
    } else {
        // отправка с остальных
        int count = this_len - 2;
        if (this_rank == total_ranks - 1)
            count = this_len - 1; // если последний процесс
        MPI_Send(&this_arr[1], count, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
    }
   
    if (this_rank == 0) {
        t_end = std::chrono::high_resolution_clock::now();
        auto avgIterTime = std::chrono::duration<double, std::milli>(t_end-t_start).count() / 1000.0;
        fprintf(stdout, "Seconds: %f\n", avgIterTime);
        // вывод 10 точек
        fprintf(stdout, "Results:\n");
        for (int i = 0; i <= n; i += n / 10)
            fprintf(stdout, "%f\n", arr[i]);

        free(arr);
        free(lefts);
        free(rights);
        free(lengths);
    }

    MPI_Finalize();

    return 0;
}
