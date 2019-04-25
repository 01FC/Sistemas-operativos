#include "xchg.h"

#define QMAXSIZE 50 //tamaño máximo de la cola

//definición función atómica xchg
#define atomic_xchg(A,B) __asm__ __volatile__( \
                                              " lock xchg %1,%0 ;\n" \
                                              : "=ir" (A) \
                                              : "m" (B), "ir" (A) \
                                              );

/*~~~~~~~~~~~~~~~~~~~~~~~~~~DEFINCIÓN DE QUEUE~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct _QUEUE //variable QUEUE tipo estructura: definición de cola
{
	int elements[QMAXSIZE]; //elementos de la cola
	int head; //punta
	int tail; //cola
} QUEUE; //definición de primitiva QUEUE

void _enqueue(QUEUE *q, int val) //función para encolar (enqueue)
{
	q->elements[q->head] = val; //agregar val al último elemento de la cola
	q->head++; //incrementar apuntador de la punta
	q->head = q->head%QMAXSIZE; //verificar que head no sea mayor que QMAXSIZE
}

int _dequeue(QUEUE *q) //función para desencolar (dequeue)
{
	int valret = q->elements[q->tail]; //devolver primer valor de la cola
	q->tail++; // Incrementa al apuntador
	q->tail=q->tail%QMAXSIZE; //verificar que tail no sea mayor que QMAXSIZE
	return(valret); //regresar valor obtenido
}

int _emptyqueue(QUEUE *q) //verificar si la cola está vacía
{
	return(q->head == q->tail); //si la cola está vacía devolverá 1
}

void _initqueue(QUEUE *q)
{
	q->head=0; //inicializar en cero
	q->tail=0; //inicializar en cero
}

/*~~~~~~~~~~~~~~~~~~~~DEFINCIÓN DE SEMAFORO~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct _SEMAFORO{ //variable QUEUE tipo estructura: definición de cola
  unsigned int contador; //variable contador
  unsigned int bloqueados; //indica cuántos procesos bloqueados hay
  QUEUE cola_de_bloqueados; //cola de bloqueados
} SEMAFORO; //definición de primitiva SEMAFORO


void waitsem(SEMAFORO *s) //función proberen para sección crítica
{
  int l = 1; //solicitar sección crítica
  do { atomic_xchg(l, *g);} while(l!=0); //función atómica xchg (spin-lock)
  if(s->contador == 0){ //si un proceso ya ha tomado la sección crítica
    _enqueue(&s->cola_de_bloqueados, getpid()); //encolar proceso a la cola de bloqueados
	  kill(getpid(), SIGSTOP);//detener proceso
    s->bloqueados++; //incrementar el número de procesos bloqueados
  }else{
    s->contador--; //liberar sección crítica
  }
}

void signalsem(SEMAFORO *s) //función vrijgave para sección crítica
{
  pid_t last_processid; //identificador del proceso anterior
  if(s->bloqueados == 0){ //si no hay procesos bloqueados
     s->contador++; //liberar sección critica
  }else{
    last_processid = _dequeue(&s->cola_de_bloqueados); //desencolar y obtener id
    kill(last_processid, SIGCONT); //detener proceso
    s->bloqueados--; //decrementa el número de procesos bloqueados
  }

  *g = 0; //Libera la sección critica
}

void initsem(SEMAFORO *s, int i){ //función para inicializar semáforo 
  s->bloqueados = 0; //no hay procesos bloqueados
  s->contador = i; //el contador del semáforo se inicializa en i
}
