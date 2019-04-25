#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#define SIGHUP 1

void shutdownFile();

int main()
{
    //char **argv;  // argument vector
    //int argc = 0; // argument count
    char instr[1000];
    char *argval[1000];
    char *string;
    int pId = -1;
    int status;

    while (1)
    {
        printf("\nsh > ");
        fflush(stdin);
        fgets(instr, sizeof(instr), stdin);
        int i = 0;
        string = strtok(instr, " \n\0"); //hacer tokens
        while (string != NULL)
        {
            argval[i++] = string;
            //printf("%s", string);
            string = strtok(NULL, " \n\0");
            //i++;
        }
        argval[i] = '\0';

        if (strcmp(argval[0], "exit") == 0)
        {
            exit(0);
        }
        else if (strcmp(argval[0], "shutdown") == 0)
        {
            shutdownFile();
            exit(0);
        }
        else
        {
            pId = fork();
            if (pId == 0) // procesos hijo
            {
                execvp(*argval, argval);
            }
            wait(&status);
        }
    }
}

void shutdownFile()
{
    FILE *file;
    file = fopen("shutdown", "a");
    fputs("Shutting down..", file);
    fclose(file);
}
