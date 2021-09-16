#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MM 3 // number of rows in A
#define NN 4 // number of cols in A
#define KK 3 // number of cols in B

void mulMat(int M, int N, int K, double *A, double *B, double *C)
{
        double sum;

        #pragma omp parallel num_threads(N)
        #pragma omp for reduction(+:sum)
        for (int n = 0; n < N; n++)
        {
                for (int k = 0; k < K; k++)
                {
                        for (int m = 0; m < M; m++)
                        {
                                sum = C[k * M + m] + A[n * M + m] * B[k * N + n];
                                C[k * M + m] = sum;
                        }
                }
        }
}

void printMat(int M, int K, double *C)
{
        for (int j = 0; j < M; j++)
        {
                printf("\n");
                for (int i = 0; i < K; i++)
                {
                        printf("%f ", C[j + i * K]);
                }
        }
        printf("\n\n");
        for (int kek = 0; kek < M * K; kek++)
                printf("%f ", C[kek]);
}

int main(void)
{

        /*
	double A[MM*NN] = {
        1, 2, 3, 4,
        1, 2, 3, 4,
		1, 2, 3, 4,
		1, 2, 3, 4};

	double B[NN*KK] = {
        1, 2, 3, 4,
        1, 2, 3, 4,
		1, 2, 3, 4,
		1, 2, 3, 4};
    */

        double C[MM * KK] = {0};

        // /*
        // Rectangular Matrices
        // R1 = 3, C1 = 4 and R2 = 4, C2 = 3 (Update these values in MACROs)
        double A[MM * NN] = {
            1, 2, 3,
            1, 2, 3,
            1, 2, 3,
            1, 2, 3};

        double B[NN * KK] = {
            1, 2, 3, 4,
            1, 2, 3, 4,
            1, 2, 3, 4};
        //*/

        mulMat(MM, NN, KK, A, B, C);
        printMat(MM, KK, C);

        return 0;
}