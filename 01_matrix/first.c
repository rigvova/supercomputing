#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* 
 * Set 1 to use a simple example.
 * Set 0 to read random arrays from file.
*/
#define READ_EXAMPLE 0

/*
 * Multiplicates two matrices (A x B) and writes result into C.
 *  - A should have len==M*N;
 *  - B should have len==N*K;
 *  - C should have len==M*K.
 * All matrices should store values in a column-major order.
 */
void blas_dgemm(int M, int N, int K, double *A, double *B, double *C)
{
        double sum;

        #pragma omp parallel
        #pragma omp for
        for (int m = 0; m < M; m++)
                for (int k = 0; k < K; k++) {
                        sum = 0;
                        for (int n = 0; n < N; n++)
                                sum += A[n * M + m] * B[k * N + n];  
                        C[k * M + m] = sum;
                }
}

/*
 * Prints values of matrix C.
 * Matrix should be present as an 1D array 
 * and store values in a column-major order. 
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
 * Inits matrices for a simple example:
 *
 *   |1 1 1 1|   |1 1 1|   |10 10 10| 
 *   |2 2 2 2| X |2 2 2| = |20 20 20|
 *   |3 3 3 3|   |3 3 3|   |30 30 30|
 *               |4 4 4|
 * 
 * Matrices are present as 1D arrays with values in a column-major order.
 */
void read_example(int *M, int *N, int *K, double **A, double **B, double **C) 
{
        
        *M = 3;
        *N = 4;
        *K = 3;

        double _A[12] = {
                1., 2., 3.,
                1., 2., 3.,
                1., 2., 3.,
                1., 2., 3.
        };

        double _B[12] = {
                1., 2., 3., 4.,
                1., 2., 3., 4.,
                1., 2., 3., 4.
        };

        *A = malloc(sizeof(double) * 12);
        *B = malloc(sizeof(double) * 12);
        *C = malloc(sizeof(double) * 9);

        for (int i = 0; i < 12; i++) {
                (*A)[i] = _A[i];
                (*B)[i] = _B[i];
        }
}

/*
 * Reads values from a text file.
 * First 3 lines should store M, N and K values.
 * Next two lines should store matrix values as 1D array
 * in a column-major order.
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

        if (READ_EXAMPLE == 1) {
                read_example(&M, &N, &K, &A, &B, &C);
                blas_dgemm(M, N, K, A, B, C);
                print_result(M, K, C);
        } else {
                read_values(&M, &N, &K, &A, &B, &C);
                blas_dgemm(M, N, K, A, B, C);
                print_result(M, K, C);
                save_result(M, K, C);
        }

        return 0;
}