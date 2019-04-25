#include <scheduler.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];

int quantumm = 0;

void scheduler(int arguments)
{
	int old, next;
	int changethread = 0;
	int waitingthread = 0;

	int event = arguments & 0xFF00;
	int callingthread = arguments & 0xFF;

	if (event == NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status = READY;
		_enqueue(&ready, callingthread);
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
		_enqueue(&ready, callingthread);
	}
	if (event == TIMER)
	{
		quantumm++;
		if (quantumm == 2) // RR con q=2
		{
			threads[callingthread].status = READY; // el hilo actual se pone en listo
			_enqueue(&ready, callingthread);	   // el hilo actual se pone en la cola
			changethread = 1;					   // cambiar de contexto
			quantumm = 0;						   // reiniciar quantumm counter
		}
	}

	if (changethread)
	{
		old = currthread;
		next = _dequeue(&ready);

		threads[next].status = RUNNING;
		_swapthreads(old, next);
	}
}
