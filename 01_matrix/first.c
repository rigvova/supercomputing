#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/*
 * Multiplicates two matrices (A x B) and writes result into C.
 *  - A should have len==M*N;
 *  - B should have len==N*K;
 *  - C should have len==M*K.
 * All matrices should store values in the column-wise order.
 */
void blas_dgemm(int M, int N, int K, double *A, double *B, double *C)
{
        double sum;

        #pragma omp parallel num_threads(N)
        #pragma omp for reduction(+:sum)
        for (int n = 0; n < N; n++)
                for (int k = 0; k < K; k++)
                        for (int m = 0; m < M; m++) {
                                sum = C[k * M + m] + A[n * M + m] * B[k * N + n];
                                C[k * M + m] = sum;
                        }
}

/*
 * Prints values of matrix C.
 * Matrix should be present as an 1D array 
 * and store values in the column-wise order. 
 */
void print_result(int M, int K, double *C)
{
        for (int j = 0; j < M; j++) {
                printf("\n");
                for (int i = 0; i < K; i++)
                        printf("%f ", C[j + i * K]);
        }
        printf("\n\n");
        for (int i = 0; i < M * K; i++)
                printf("%f ", C[i]);
}

/*
 * Reads values from a text file.
 * First 3 lines should store M, N and K values.
 * Next two lines should store matrix values as 1D array
 * in the column-wise order.
 */
void read_values(int *M, int *N, int *K, double **A, double **B, double **C) {
        FILE *data;

        data = fopen("data.txt", "r");

        fscanf(data, "%d", M);
        fscanf(data, "%d", N);
        fscanf(data, "%d", K);

        *A = malloc(sizeof(double) * *M * *N);
        *B = malloc(sizeof(double) * *N * *K);
        *C = malloc(sizeof(double) * *M * *K);

        for (int i = 0; i < *M * *N; i++)
                fscanf(data, "%lf", &((*A)[i]));

        for (int i = 0; i < *N * *K; i++)
                fscanf(data, "%lf", &((*B)[i]));

        fclose(data);
}

/* 
 * Prints result matrix C into 'cresult.txt'.
 * Used for comparison with results from the Python script ('pyresult.txt').
 */
void save_result(int M, int K, double *C) 
{
        FILE *data;

        data = fopen("cresult.txt", "w");
        
        for (int i = 0; i < M * K; i++){
                if (i == M * K - 1)
                        fprintf(data, "%.14lf", C[i]);
                else
                        fprintf(data, "%.14lf ", C[i]);
        }

        fclose(data);
}

int main(void)
{
        int M, N, K;
        double *A, *B, *C;

        read_values(&M, &N, &K, &A, &B, &C);
        blas_dgemm(M, N, K, A, B, C);
        print_result(M, K, C);
        save_result(M, K, C);

        return 0;
}