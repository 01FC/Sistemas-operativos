#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "commondata.h"

#define CICLOS 10 //ciclos de ejecución de la sección crítica

char *pais[3]={"Peru","Bolvia","Colombia"}; //identificador de procesos "países"
int *g; //variable global compartida

void proceso(int i, SEMAFORO *s)
{
  int k; //contador

  for(k=0; k < CICLOS ; k++)
  {
    sleep(rand()%3); //espera aleatoria
    waitsem(s); //función atómica: proberen
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    printf("Entra %s",pais[i]);
    fflush(stdout); //actualizar buffer
    sleep(rand()%3); //espera aleatoria
    printf("- %s Sale\n",pais[i]);
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    signalsem(s); //función atómica: vrijgave
    sleep(rand()%3); //espera aleatoria
  }
  exit(0); //finalizar proceso
}

int main()
{
  int i;
  int pid; //identificador de proceso
  int status; //estatus del proceso
  int shmid,shmid2; //identificadores de segmento de memoria compartida
  SEMAFORO *s; //variable semaforo s

  // Solicitar memoria compartida
  shmid  = shmget(0x1234, sizeof(g),0666|IPC_CREAT); //reserva de e. de memoria tamaño int
  shmid2 = shmget(0x1234, sizeof(s),0666|IPC_CREAT); //reserva de e. de memoria tamaño semaforo

  if(shmid == -1 || shmid2 == -1) //manejo de error durante creación de la memoria compartida
  {
    perror("Error en la memoria compartida\n"); //mostrar mensaje de error
    exit(-1); //salir
  }

  g = shmat(shmid,  NULL, 0); //adjuntar espacio de memoria shmid con variable entera g
  s = shmat(shmid2, NULL, 0); //adjuntar espacio de memoria shmid con variable semaforo s

  if(g == NULL || s == NULL) //manejo de error durante el adjuntamiento de  la memoria
  {
    perror("Error en el shmat\n"); //mostrar mensaje de error
    exit(-1);  //salir
  }

  initsem(s, 1); //inicializar semáforo en 1
  *g=0; //variable global g es zero

  srand(getpid()); //asignar identificador aleatorio

  for(i=0; i < 3; i++)
  {
    pid = fork(); //creación de proceso hijo
    if(pid == 0) proceso(i, s); //función de proceso hijo

  }

  for(i=0;i<3;i++) pid = wait(&status); //esperar a que todos los procesos finalicen

  shmdt(g); //desadjuntar memoria compartida shmid (memory dettachment)
  shmdt(s);//desadjuntar memoria compartida shmid2 (memory dettachment)

  return 0;
}
