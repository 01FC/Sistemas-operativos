/*
  CREADO POR CÉSAR VILLARREAL & FRANCISCO CAMACHO
  SÁBADO 20 DE OCTUBRE DE 2018
  TÍTULO: SOLUCIÓN AL PROBLEMA DE LA SECCIÓN CRÍTICA POR COLA DE MENSAJES
  PRÁCTICA VI
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define CICLOS 10  //ciclos de la sección crítica
#define MSGLEN 100 //tamaño máximo del mensaje

char *pais[3] = {"Peru", "Bolivia", "Colombia"};

struct msgbuf
{
  long mtype;         //message type, must be > 0
  char mtext[MSGLEN]; //message data
} exmut;

void proceso(int i, int *msg_key)
{
  int k;            //variable contador
  int msgqueue_ver; //verificador de cola de mensajes

  for (k = 0; k < CICLOS; k++)
  {
    /*receptor de mensaje bloqueante*/
    msgqueue_ver = msgrcv(*msg_key, &exmut, sizeof(exmut.mtext), exmut.mtype, 0);
    if (msgqueue_ver == -1)
    {
      perror("Error while receiving the message");
      exit(-1);
    }

    /* Entrada a la sección crítica*/
    printf("Entra %s", pais[i]);
    fflush(stdout);
    sleep(rand() % 3);
    printf(" - %s Sale\n", pais[i]);
    /* Salida de la sección crítica*/

    /*envío de mensaje no bloqueante*/
    msgqueue_ver = msgsnd(*msg_key, &exmut, sizeof(exmut.mtext), IPC_NOWAIT);
    if (msgqueue_ver == -1)
    {
      perror("Error while sending the message");
      exit(-1);
    }
    sleep(rand() % 3);
  }
  exit(0); // Termina el proceso
}

int main()
{
  int pid;          //identificador del proceso
  int status;       //status del proceso
  int i;            //variable contador
  int *msg_key;     //llave de paso de mensajes
  int msgqueue_ver; //bandera de verificación para cola de mensajes
  int shmid;        //identificador de memoria compartida
  exmut.mtype = 1;  //definir tipo de mensaje

  // Solicitar memoria compartida
  shmid = shmget(0x1234, sizeof(msg_key), 0666 | IPC_CREAT); //reserva de memoria
  if (shmid == -1)                                           //manejo de error durante creación de la memoria compartida
  {
    perror("Error en la memoria compartida\n"); //mostrar mensaje de error
    exit(-1);                                   //salir
  }

  msg_key = shmat(shmid, NULL, 0); //adjuntar espacio de memoria shmid a msg_key
  if (*msg_key == -1)              //manejo de error durante el adjuntamiento de  la memoria
  {
    perror("Error en el shmat\n"); //mostrar mensaje de error
    exit(-1);                      //salir
  }

  //obtener identificador de la cola asociada con los mensajes
  *msg_key = msgget(IPC_PRIVATE, 0600 | IPC_CREAT | IPC_EXCL);
  /*  IPC_ṔRIVATE: indica que la cola de mensajes a crear es privada
      0600: permisos de lectura y escritura
      IPC_EXCL: asegura la creación de la cola de mensajes
  */

  if (*msg_key == -1) //manejar error de creación de llave
  {
    perror("Ha ocurrido un error durante la creación de la llave");
    exit(1);
  }

  msgqueue_ver = msgsnd(*msg_key, &exmut, 0, 0); //inicializar cola de mensajes
  if (msgqueue_ver == -1)
  {
    perror("Ha ocurrido un error al inicializar la cola de mensajes");
    exit(1);
  }

  srand(getpid()); //obtener identificador de proceso aleatorio

  for (i = 0; i < 3; i++)
  {
    pid = fork(); //creación de proceso hijo
    if (pid == 0)
      proceso(i, msg_key); //función de proceso hijo
  }

  for (i = 0; i < 3; i++)
    pid = wait(&status); //esperar a procesos hijos

  shmdt(msg_key); //desadjuntar identificador del espacio de memoria
  return 0;
}
