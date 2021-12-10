#include "stdio.h"
#include "stdlib.h"
#define _USE_MATH_DEFINES
#include <math.h>

double u(const double x, const double t, const double k,
         const double u0, const double l)
{
    double sum = 0;
    for (int m = 0; m < 10000; m++) {
        sum += (std::exp(-k * M_PI * M_PI 
                         * (2 * m+1) * (2 * m+1) * t 
                         / (l * l))
                * std::sin(M_PI * x * (2 * m + 1) / l)
                / (2 * m + 1));
    }
    return 4 * u0 * sum / M_PI;
}

bool f(const int n, double* arr, double* temp_arr, double k, double T, double tau, double h)
{
    bool flag = true; 

    for (double curr_t = 0.0; curr_t <= T; curr_t += tau) {
        if (flag) {
            for (int i = 1; i < n-1; i++) 
                temp_arr[i] = arr[i] + k * tau * (arr[i-1] - 2 * arr[i] + arr[i+1]) / h / h;
            flag = false;
        }
        else {
            for (int i = 1; i < n-1; i++) 
                arr[i] = temp_arr[i] + k * tau * (temp_arr[i-1] - 2 * temp_arr[i] + temp_arr[i+1]) / h / h;
            flag = true;
        }
    }
    return flag;
}

int main()
{
    // Params
    double T = 0.1;      // конечное время
    double k = 1.0;      // коэффициент теплопередачи
    double h = 0.02;     // шаг по пространству
    double tau = 0.0002; // шаг по времени
    double u0 = 1.0;     // начальная температура
    double l = 1.0;      // длина стержня

    // Precise value computation
    int npoints = 11;
    double step = 0.1;

    double *A = new double[npoints];
    std::fill(A, A + npoints, 0);
    for (int i = 1; i < npoints; i++)
        A[i] = A[i-1] + step; 

    printf("Precise value computation:\n");
    printf("[");
    for (int i = 0; i < npoints; i++) {
        printf("%4.2f, ", u(A[i], T, k, u0, l));
    }
    printf("]");    

    // Main algo
    int n = (1 / h) + 1; // кол-во точек на стержне
    double* arr = new double[n]; // массив температур (точек на стержне)
    double* temp_arr = new double[n];

    std::fill(temp_arr, temp_arr + n, 0.0);
    std::fill(arr, arr + n, u0);
    arr[0] = 0;
    arr[n-1] = 0;

    bool flag;
    flag = f(n, arr, temp_arr, k, T, tau, h);

    if (!flag)
    for (int i = 0; i < n; i++) {
        arr[i] = temp_arr[i];
    }

    printf("\nApprox value computation:\n");
    printf("[");
    for (int i = 0; i < n; i += 5) {
        printf("%4.2f, ", arr[i]);
    }
    printf("]");  

    return 0;
}