/*
*
* Autor: Agnieszka Kuśnierz
* Numer albumu: 332284
*
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include "err.h"

#define BUF_SIZE            1024

int main(int argc, char* argv[]) {

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
        //proces potomny
        case 0:
            if (close (0) == -1) syserr("W(1), close(0)");
            if (close (1) == -1) syserr("W(1), close(1)");
            if (dup (pipe_dsc[0][0]) == -1) syserr("W(1), dup(pipe_dsc[0][0])");
            if (dup (pipe_dsc[1][1]) == -1) syserr("W(1), dup(pipe_dsc[1][1])");
            if (close (pipe_dsc[0][0]) == -1) syserr("W(1), close(pipe_dsc[0][0])");
            if (close (pipe_dsc[0][1]) == -1) syserr("W(1), close(pipe_dsc[0][1])");
            if (close (pipe_dsc[1][0]) == -1) syserr("W(1), close(pipe_dsc[1][0])");
            if (close (pipe_dsc[1][1]) == -1) syserr("W(1), close(pipe_dsc[1][1])");
            execlp("./process_w", "./process_w", argv[1], NULL);
            syserr("Error in execlp\n");
            exit(0);
        //proces macierzysty
        default:
            if (close (pipe_dsc[1][1]) == -1) syserr("ToONP, close(pipe_dsc[1][1])");
            if (close (pipe_dsc[0][0]) == -1) syserr("ToONP, close(pipe_dsc[0][0])");

            char converted[BUF_SIZE];
            //Wysyłanie danych do procesu w(1).
            if (write (pipe_dsc[0][1], &argv, sizeof(*argv) - 1) == -1) syserr("ToONP, write(pipe_dsc[1][0])");

            // Odczytanie wyniku.
            if (read (pipe_dsc[1][0], &converted, sizeof(converted) - 1) == -1) syserr("ToONP, read(pipe_dsc[0][1])");

            //Oczekiwanie za zakończenie procesów potomnych.
            if (wait (0) == -1)
                syserr("wait");

            // Wypisanie wyniku na standardowe wyjście.
            //if(write (1, &converted, sizeof(converted) - 1) == -1) syserr("ToONP, write(1, converted, sizeof(converted))");

            if (close (pipe_dsc[1][0]) == -1) syserr("ToONP, close(pipe_dsc[1][0])");
            if (close (pipe_dsc[0][1]) == -1) syserr("ToONP, close(pipe_dsc[0][1])");
            exit(0);
        }

}
