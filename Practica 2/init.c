#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>

void generateProcesses();
void onStopSignal();
void killProcesses();
int getPid(int pId, int *processes);
bool isShutdownFile();

int status;
int processes[6];
int pId;
int pStopped = -1;
int wStatus = -1;

int main()
{
    generateProcesses();
    onStopSignal();
    exit(0);
}

void generateProcesses()
{

    int i = 0;
    for (i = 0; i < 6; i++)
    {
        pId = fork();

        if (pId == 0)
        { //hijo
            //printf("Hijo pId  %d\n", pId);
            execlp("xterm", "xterm", "-e", "./getty", NULL);
        }
        else
        { //padre
            //printf("Padre pId  %d\n", pId);
            processes[i] = pId;
        }
    }
}

void onStopSignal()
{
    while (1)
    {
        wStatus = wait(&status);
        if (wStatus == -1)
        {
            //printf("Wait response error!");
            return;
        }
        else //wStatus = id de proceso
        {
            bool sFile = isShutdownFile();
            if (sFile)
            {
                //printf("shutting down...\n");
                killProcesses();
                return;
            }
            else
            {
                pStopped = getPid(wStatus, processes);
                pId = fork();
                processes[pStopped] = pId;
                if (pId == 0) //hijo
                {
                    execlp("xterm", "xterm", "-e", "./getty", NULL);
                }
            }
        }
    }
}
void killProcesses()
{
    int i = 0;
    for (i; i < 6; i++)
    {
        //printf("killing process %d\n", processes[i]);
        kill(processes[i], SIGKILL);
    }
}

int getPid(int processId, int *processes)
{
    int i = 0;
    for (i; i < 6; i++)
    {
        if (processId == processes[i])
        {
            return i;
        }
    }
    return -1;
}

bool isShutdownFile()
{
    FILE *file;
    file = fopen("shutdown", "r");
    if (file == NULL) // shutdown no existe..
    {
        //printf("Waiting for shutdown file ");
        return 0;
    }
    else // shutdown si existe..
    {
        fclose(file);
        remove("shutdown");
        return 1;
    }
}
