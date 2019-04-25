#include <scheduler.h>
#include <stdlib.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

typedef enum
{
	false,
	true
} bool;

int numOfQs;

QUEUE *ready, *queue1, *queue2; // Queue 0, Queue 1, Queue2
QUEUE *remaining()				// la cola tiene datos
{
	int i = 0;
	for (i < 0; i < numOfQs; ++i)
	{
		if (!_emptyq(&ready[i])) // si hay datos regresarlos
			return &ready[i];
	}
	return NULL;
}
QUEUE waitinginevent[MAXTHREAD];

void scheduler(int arguments)
{
	int old, next;
	int changethread = 0;
	int waitingthread = 0;

	int event = arguments & 0xFF00;
	int callingthread = arguments & 0xFF;

	numOfQs = 1;
	bool last = true;

	if (ready == NULL)
	{
		ready = malloc(sizeof(QUEUE) * 20000); //memoria para la cola
		queue2 = queue1 = &ready[0];		   // inicializacion de colas
	}

	if (event == NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		queue1 = &ready[0];
		threads[callingthread].status = READY;
		_enqueue(queue1, callingthread);
		last = false;
	}

	if (event == BLOCKTHREAD)
	{

		threads[callingthread].status = BLOCKED;
		_enqueue(&waitinginevent[blockevent], callingthread);

		changethread = 1;
	}

	if (event == ENDTHREAD)
	{
		threads[callingthread].status = END;
		changethread = 1;
	}

	if (event == UNBLOCKTHREAD)
	{
		threads[callingthread].status = READY;
		queue1 = &ready[0];
		_enqueue(queue1, callingthread);
	}

	if (event == TIMER)
	{
		if (last)
			_enqueue(&(*queue1), callingthread); // encolar la cola 1 (solo hay uno)
		else
			_enqueue(&(*(queue1 + 1)), callingthread);

		if (queue1 == queue2)
			queue2 = &ready[++numOfQs - 1];
		threads[callingthread].status = READY;
		changethread = 1;
	}

	if (changethread)
	{
		queue1 = remaining(); // verificar si hay datos en la cola y obtener
		old = currthread;
		next = _dequeue(queue1); // tomar el proximo

		threads[next].status = RUNNING;
		_swapthreads(old, next);
	}
}

/*
           +---+---+---+
enqueue -> | 3 | 2 | 1 | -> dequeue
           +---+---+---+
*/