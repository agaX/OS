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

#define BUF_SIZE 100

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Wywołaj program z jednym argumentem.\n");
    exit(0);
  }

  int pipe_dsc[2][2];
  if (pipe(pipe_dsc[0]) == -1) syserr("Error in pipe\n");
  if (pipe(pipe_dsc[1]) == -1) syserr("Error in pipe\n");

  // Utworzenie procesu potomnego W(1)
  switch (fork()) {
    case -1:
      syserr("Error in fork\n");

    // proces potomny
    case 0:
      if (close(0) == -1) syserr("W, close(0)");
      if (close(1) == -1) syserr("W, close(1)");
      if (dup(pipe_dsc[0][0]) == -1) syserr("W, dup(pipe_dsc[0][0])");
      if (dup(pipe_dsc[1][1]) == -1) syserr("W, dup(pipe_dsc[1][1])");
      if (close(pipe_dsc[0][0]) == -1) syserr("W, close(pipe_dsc[0][0])");
      if (close(pipe_dsc[0][1]) == -1) syserr("W, close(pipe_dsc[0][1])");
      if (close(pipe_dsc[1][0]) == -1) syserr("W, close(pipe_dsc[1][0])");
      if (close(pipe_dsc[1][1]) == -1) syserr("W, close(pipe_dsc[1][1])");
      execlp("./W", "./W", NULL);
      syserr("Error in execlp\n");
      exit(0);

    // proces macierzysty
    default:
      if (close(pipe_dsc[1][1]) == -1) syserr("ToONP, close(pipe_dsc[1][1])");
      if (close(pipe_dsc[0][0]) == -1) syserr("ToONP, close(pipe_dsc[0][0])");

      // Wysyłanie danych do procesu w(1).
      for (int i = 0; i < strlen(argv[1]); i++)
        if (write(pipe_dsc[0][1], &argv[1][i], sizeof(char)) == -1)
          syserr("ToONP, write(pipe_dsc[0][1])");

      // "Dodanie" trzech '#', które oddzielają poszczególne części napisu
      char c = '#';
      for (int i = 0; i < 3; i++)
        if (write(pipe_dsc[0][1], &c, sizeof(char)) == -1)
          syserr("ToONP, write(pipe_dsc[0][1])#");

      // Odczytanie wyniku.
      if (read(pipe_dsc[1][0], &c, sizeof(char)) == -1)
        syserr("ToONP, read(pipe_dsc[1][0])");

      // Wypisywanie wyniku konwersji wyrażenia na standardowe wyjście
      while (c != '#') {
        write(1, &c, sizeof(char));
        if (read(pipe_dsc[1][0], &c, sizeof(char)) == -1)
          syserr("ToONP, read(pipe_dsc[1][0])");
      }
      printf("\n");

      // Oczekiwanie za zakończenie procesów potomnych.
      if (wait(0) == -1) syserr("wait");
      if (close(pipe_dsc[1][0]) == -1) syserr("ToONP, close(pipe_dsc[1][0])");
      if (close(pipe_dsc[0][1]) == -1) syserr("ToONP, close(pipe_dsc[0][1])");
      exit(0);
  }
}
