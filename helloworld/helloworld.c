#include <stdio.h>
#include <omp.h>


int main (void)
{
    #pragma omp parallel num_threads(6)
    {
        printf("Hello world!\n");
        printf("OMP thread id: %d\n", omp_get_thread_num());
    }

    return 0;
}