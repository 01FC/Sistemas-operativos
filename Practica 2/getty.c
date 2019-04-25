#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

//void shutdown();
void login();
bool loginValidation(FILE *file);
bool salir = 0;
int status;
int childPiD;

int main()
{

    while (!salir)
    {
        login();
    }
    return 0;
}

void login()
{
    FILE *file;
    file = fopen("passwd", "r");
    if (file == NULL)
    {
        printf("Error! no passwd file");
        salir = 1;
    }
    else
    {
        bool successful = loginValidation(file);
        if (successful)
        {
            childPiD = fork();
            if (childPiD == 0) //hijo
            {
                execlp("./shell", "./shell", NULL);
            }

            wait(&status);
            if (WIFEXITED(status) != 0) //returns a nonzero value if the child process terminated normally with exit or _exit
            {
                exit(0); //EXIT_SUCCESS
            }
            else
            {
                exit(1); //EXIT_FAILURE
            }
        }
    }
}

bool loginValidation(FILE *file)
{
    char line[101];
    char user[50];
    char pass[50];
    char *loginLine;
    char *temp;

    printf("\nUser: ");
    scanf("%s", user);
    fflush(stdin);
    printf("Password: ");
    scanf("%s", pass);

    temp = strcat(user, ":"); // concatenar :
    loginLine = strcat(temp, pass);
    temp = NULL;

    while (fgets(line, sizeof(line), file) != NULL) // leer lineas del archivo passwd
    {
        line[strlen(line) - 1] = '\0'; // quitar linea demas que regreas fgets
        if (strcmp(line, loginLine) == 0)
        {
            printf("Authentication successful\n");
            return 1;
        }
    }
    printf("Authentication failed\n");
    return 0;
}