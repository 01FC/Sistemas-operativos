#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

struct timeval ts; // struct para tiempo

int main()
{
    long long n, i;
    long long tStart, tEnd;
    double acc = 0;

    printf("Ingresar numero de iteraciones: ");
    fflush(stdin);
    scanf("%lld", &n); // numero de iteraciones

    gettimeofday(&ts, NULL);
    tStart = ts.tv_sec * 1000000 + ts.tv_usec; // tiempo inicial

    for (i = 0; i < n; i++)
    {
        acc = acc + pow(-1, i) / (2.0 * i + 1); // sumatoria de leibniz para calcular pi/4
    }

    gettimeofday(&ts, NULL);
    tEnd = ts.tv_sec * 1000000 + ts.tv_usec; // tiempo final

    printf("Tiempo total: %f\n", (float)(tEnd - tStart) / 1000000);
    printf("TT/4 = %lf\n", acc);
    exit(0);
}