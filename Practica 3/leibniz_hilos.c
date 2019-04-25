#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

#define NUMTHREADS 4

pthread_mutex_t mutex; // mutex para retener el recurso
struct timeval ts;     // estructura para el tiempo
double totalSum = 0;
long long n; // numero de iteraciones

void *leibniz_hilos(void *threadid); // funcion de hilos

int main()
{
    long long tStart, tEnd, i;
    int hilo;
    printf("Ingresar numero de iteraciones: ");
    fflush(stdin);
    scanf("%lld", &n);

    gettimeofday(&ts, NULL);
    tStart = ts.tv_sec * 1000000 + ts.tv_usec; // tiempo inicial

    pthread_t threads[NUMTHREADS];    // arreglo de 4 hilos
    pthread_mutex_init(&mutex, NULL); // inicializacion de mutex

    for (i = 0; i < NUMTHREADS; i++)
    {
        hilo = pthread_create(&threads[i], NULL, leibniz_hilos, (void *)i); // creacion de hilos

        if (hilo)
        {
            printf("Error:unable to create thread %d\n", hilo);
            exit(-1);
        }
    }

    for (i = 0; i < NUMTHREADS; i++) // finalizacion de hilos
    {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&ts, NULL);
    tEnd = ts.tv_sec * 1000000 + ts.tv_usec; // tiempo final
    printf("Tiempo total: %f\n", (float)(tEnd - tStart) / 1000000);
    printf("TT/4 = %lf\n", totalSum);

    exit(0);
}

void *leibniz_hilos(void *threadid) // funcion de hilos para calcular leibniz de pi/4
{
    int i;
    int index, limit; // inicio y fin de cada operacion de cada hilo
    double acc = 0;
    long tid; //id del thread

    tid = (long)threadid;
    index = n / NUMTHREADS * tid;
    limit = n / NUMTHREADS * (tid + 1);

    for (i = index; i < limit; i++)
    {
        acc = acc + pow(-1, i) / (2.0 * i + 1); // sumatoria de leibniz para calcular pi/4
    }
    pthread_mutex_lock(&mutex); // bloquear el recurso para otros hilos
    totalSum += acc;
    pthread_mutex_unlock(&mutex); // liberar recurso para otros hilos

    return NULL;
}