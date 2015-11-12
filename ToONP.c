/*
*
* Autor: Agnieszka Kuśnierz
* Numer albumu: 332284
*
*
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

int main(int argc, char* argv[]) {

    // Sprawdzanie, czy program jest wykonywany z dokładnie jednym argumentem.
    if (argc != 2) {
        printf("Wywołaj program z jednym argumentem.\n");
        exit(0);
    }

    int pipe_dsc[2][2];
	if (pipe (pipe_dsc[0]) == -1) syserr("Error in pipe\n");
	if (pipe (pipe_dsc[1]) == -1) syserr("Error in pipe\n");

    // Utworzenie procesu potomnego W(1)
    switch(fork ()) {
        case -1:
            syserr("Error in fork\n");

        case 0:
            if (close (0) == -1)	syserr("W(1), close(0)");
            if (close (1) == -1)	syserr("W(1), close(1)");
            if (dup (pipe_dsc[0][0]) == -1) syserr("W(1), dup(pipe_dsc[0][0])");
            if (dup (pipe_dsc[1][1]) == -1) syserr("W(1), dup(pipe_dsc[1][1])");
            if (close (pipe_dsc[0][0]) == -1) syserr("W(1), close(pipe_dsc[0][0])");
            if (close (pipe_dsc[0][1]) == -1) syserr("W(1), close(pipe_dsc[0][1])");
            if (close (pipe_dsc[1][0]) == -1) syserr("W(1), close(pipe_dsc[1][0])");
            if (close (pipe_dsc[1][1]) == -1) syserr("W(1), close(pipe_dsc[1][1])");
            execlp("./process_w", "./process_w", argv[1], NULL);
            syserr("Error in execlp\n");
            exit(0);

        default:
            if()



}
