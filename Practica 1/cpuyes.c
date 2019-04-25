#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#define CICLOS 1869999 // Modifique este valor para que la duración sea 10 segundos

int main(int argc,char *argv[])
{
	long long start_ts;
	long long stop_ts;
	int elapsed_time;
	long lElapsedTime;
	struct timeval ts;

	int i,j;
	int porc;

	if(argc<2)
		porc=100;
	else
		porc=atoi(argv[1]);

	printf("CPU=%d, E/S=%d\n",porc,100-porc);

	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Tiempo inicial


	for (i=0; i<porc ; i++)     // Ejecutar instrucciones del CPU
		for(j=0;j<CICLOS;j++); // Uso intensivo del CPU


	usleep((100-porc)*100000);

	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Tiempo final

	elapsed_time = (int) (stop_ts - start_ts);
	printf("proceso %d, %d microsegundos\n",getpid(),elapsed_time);

}
