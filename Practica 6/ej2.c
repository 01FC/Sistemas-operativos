/*
  CREADO POR CÉSAR VILLARREAL & FRANCISCO CAMACHO
  SÁBADO 20 DE OCTUBRE DE 2018
  TÍTULO: SOLUCIÓN AL PROBLEMA DE LA SECCIÓN CRÍTICA POR SEMÁFOROS
  PRÁCTICA VI
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "semaphores.h"

#define CICLOS 10

char *pais[3] = {"Peru", "Bolivia", "Colombia"};

void proceso(int i, int semid)
{
  int k; //variable contador

  for (k = 0; k < CICLOS; k++)
  {
    sleep(rand() % 3); //espera aleatoria
    semwait(semid);
    // Entrada a la sección crítica
    printf("Entra %s", pais[i]);
    fflush(stdout);    //actualizar buffer
    sleep(rand() % 3); //espera aleatoria
    printf("- %s Sale\n", pais[i]);
    // Salida de la sección crítica
    semsignal(semid);
    sleep(rand() % 3);
  }
  exit(0); // Termina el proceso
}

int main()
{
  int pid;             //identificador de proceso
  int status;          //estatus del proceso
  int i;               //variable contador
  int semid;           //identificador de semáforo
  int sem_key = 10231; //llave del semáforo

  semid = createsem(sem_key, 1); //crear y inicializar semaforo en 1

  srand(getpid()); //obtener identificador aleatorio
  for (i = 0; i < 3; i++)
  {
    pid = fork(); //creación de proceso hijo
    if (pid == 0)
      proceso(i, semid); //función de proceso hijo
  }

  for (i = 0; i < 3; i++)
  {
    pid = wait(&status); //esperar a procesos hijos
  }

  erasesem(semid); //eliminar semáforo
  return 0;
}
