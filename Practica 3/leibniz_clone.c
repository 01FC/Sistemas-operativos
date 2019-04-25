
#define _GNU_SOURCE
#include <linux/sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>
#include <malloc.h>

#define NUMTHREADS 4
#define STACK_SIZE (1024 * 1024) // memoria asignada 1 Mbit

struct timeval ts; // estructura para el tiempo
double totalSum = 0;
long long n; // numero de iteraciones

void *leibniz_clone(void *clonid); // funcion de clones

int main()
{
    void *childrenStack;
    long long tStart, tEnd, i;
    int waitId;
    pid_t child;

    childrenStack = malloc(STACK_SIZE * NUMTHREADS);

    printf("Ingresar numero de iteraciones: ");
    fflush(stdin);
    scanf("%lld", &n);

    gettimeofday(&ts, NULL);
    tStart = ts.tv_sec * 1000000 + ts.tv_usec; // tiempo inicial

    for (i = 0; i < NUMTHREADS; i++) // creacion de clones con flag signals
    {
        child = clone((void *)&leibniz_clone, childrenStack + (STACK_SIZE * (i + 1)), CLONE_FS | SIGCHLD | CLONE_VM, (void *)i); // (creacion de clones,stack creciente, <file system| signalhandler|memoria compartida>,hijo)

        if (child == -1)
        {
            printf("Error:unable to create child %d\n", child);
            printf("clone() returned %ld\n", (long)child);
            exit(-1);
        }
    }

    for (i = 0; i < NUMTHREADS; i++) // finalizacion de clones
    {
        wait(&waitId);
    }

    gettimeofday(&ts, NULL);
    tEnd = ts.tv_sec * 1000000 + ts.tv_usec; // tiempo final
    printf("Tiempo total: %f\n", (float)(tEnd - tStart) / 1000000);
    printf("TT/4 = %lf\n", totalSum);
    free(childrenStack);

    exit(0);
}

void *leibniz_clone(void *clonid) // funcion de clones para calcular leibniz de pi/4
{
    int i;
    int index, limit; // inicio y fin de cada operacion de cada clon
    double acc = 0;
    long cid; //id del thread

    cid = (long)clonid;
    index = n / NUMTHREADS * cid;
    limit = n / NUMTHREADS * (cid + 1);

    for (i = index; i < limit; i++)
    {
        acc = acc + pow(-1, i) / (2.0 * i + 1); // sumatoria de leibniz para calcular pi/4
    }
    totalSum += acc;

    return NULL;
}