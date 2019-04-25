#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <mmu.h>

#define NUMPROCS 4
#define PAGESIZE 4096
#define PHISICALMEMORY 12 * PAGESIZE
#define TOTFRAMES PHISICALMEMORY / PAGESIZE
#define RESIDENTSETSIZE PHISICALMEMORY / (PAGESIZE * NUMPROCS)

extern char *base;
extern int framesbegin;
extern int idproc;
extern int systemframetablesize;
extern int ptlr;

extern struct SYSTEMFRAMETABLE *systemframetable;
extern struct PROCESSPAGETABLE *ptbr;

int getfreeframe();       // Busca un marco físico libre en el sistema
int searchvirtualframe(); // Busca un frame virtual en memoria secundaria
int getfifo();            // Buscar una página a expulsar

int pagefault(char *vaddress)
{
    int i;
    int frame, vframe;
    long pag_a_expulsar;
    int fd;
    char buffer[PAGESIZE];
    int pag_del_proceso;

    // A partir de la dirección que provocó el fallo, calculamos la página
    pag_del_proceso = (long)vaddress >> 12;
    // Si la página del proceso está en un marco virtual del disco
    if ((ptbr + pag_del_proceso)->presente != 1 && (ptbr + pag_del_proceso)->framenumber != -1)
    {
        vframe = (ptbr + pag_del_proceso)->framenumber;
        // Lee el marco virtual al buffer
        readblock(buffer, vframe);
        // Libera el frame virtual
        systemframetable[vframe].assigned = 0;
    }

    // Cuenta los marcos asignados al proceso
    i = countframesassigned();
    //printf("i: %d\n", i);
    //printf("RESIDENT SIZE: %d\n", RESIDENTSETSIZE);
    // Si ya ocupó todos sus marcos, expulsa una página
    if (i >= RESIDENTSETSIZE)
    {
        // Buscar una página a expulsar
        pag_a_expulsar = getfifo();

        // Poner el bit de presente en 0 en la tabla de páginas
        (ptbr + pag_a_expulsar)->presente = 0;

        // Si la página ya fue modificada, grábala en disco
        if ((ptbr + pag_a_expulsar)->modificado == 1)
        {
            frame = (ptbr + pag_a_expulsar)->framenumber;

            // Escribe el frame de la página en el archivo de respaldo y pon en 0 el bit de modificado
            writeblock(buffer, frame);
            (ptbr + pag_a_expulsar)->modificado = 0;
        }

        // Busca un frame virtual en memoria secundaria
        vframe = searchvirtualframe();
        //printf("vframe: %d\n", vframe);
        // Si no hay frames virtuales en memoria secundaria regresa error
        // verficar si la pagina no esta en un marco virtual
        if (vframe == -1)
            return -1;
        // Si la página ya fue modificada, grábala en disco
        // Copia el frame a memoria secundaria, actualiza la tabla de páginas y libera el marco de la memoria principal
        copyframe(frame, vframe);
        (ptbr + pag_a_expulsar)->presente = 0;
        (ptbr + pag_a_expulsar)->framenumber = vframe;

        systemframetable[frame].assigned = 0;
        systemframetable[vframe].assigned = 1;

        return 1;
    }

    // Busca un marco físico libre en el sistema
    // Si no hay marcos físicos libres en el sistema regresa error
    frame = getfreeframe();
    //printf("frame: %d\n", frame);
    if (frame == -1)
        return (-1); // Regresar indicando error de memoria insuficiente

    // Si ya estaba en memoria secundaria cópialo al frame libre encontrado en memoria principal
    // y transfiérelo a la memoria física
    if ((ptbr + pag_del_proceso)->presente != 1)
    {
        copyframe((ptbr + pag_del_proceso)->framenumber, frame);
        loadframe(frame);
    }

    // Poner el bit de presente en 1 en la tabla de páginas y el frame
    (ptbr + pag_del_proceso)->presente = 1;
    (ptbr + pag_del_proceso)->framenumber = frame;

    return (1); // Regresar todo bien
}

//Dirección del marco con la página que tiene más tiempo sin usarse
int getfifo()
{
    int index;
    unsigned long times = 0;

    for (int i = 0; i < ptlr; i++)
    {
        if ((ptbr + i)->presente == 1 && (ptbr + i)->tlastaccess > times)
        {
            index = i;
            times = (ptbr + i)->tlastaccess;
        }
    }
    return index;
}

int getfreeframe()
{
    int i;
    // Busca un marco libre en el sistema
    for (i = framesbegin; i < systemframetablesize + framesbegin; i++)
    {
        if (!systemframetable[i].assigned)
        {
            systemframetable[i].assigned = 1;
            break;
        }
    }

    if (i < systemframetablesize + framesbegin)
        systemframetable[i].assigned = 1;
    else
        i = -1; // Si no hay marcos libres en el sistema regresa error
    return (i);
}

int searchvirtualframe()
{
    int i;
    //Busca un marco virtual en memoria secundaria
    for (i = framesbegin + systemframetablesize; i < framesbegin + systemframetablesize * 2; i++)
    {
        if (!systemframetable[i].assigned)
            break;
    }

    if (i < framesbegin + systemframetablesize * 2)
        systemframetable[i].assigned = 1;
    else
        i = -1; // Si no hay marcos virtuales en memoria secundaria regresa error
    //printf("fetching virtual frame: %d\n", i);
    return i;
}
