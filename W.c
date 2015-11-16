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
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "err.h"

typedef struct node {
  char c;
  struct node* next;
} node_;

typedef enum { argument, lbracket, rbracket, operator} operation;

node_* new_node(char c) {
  node_* new = (node_*)malloc(sizeof(node_));
  new->c = c;
  new->next = NULL;
  return new;
}

// Funkcja wykonująca jeden krok algorytmu
node_* run() {
  int x1, x2, left = 3;
  char c = ' ', a;
  operation to_do;
  node_ *act, *guard, *new, *pom, *first, *second, *elem;
  guard = (node_*)malloc(sizeof(node_));
  guard = new_node('&');
  first = guard;
  second = guard;
  act = guard;
  elem = new_node('&');
  // czytamy wyrazenie do przetworzenia
  while (c == ' ')
    if (read(0, &c, sizeof(char)) == -1) syserr("W, read(0)");
  elem->c = c;

  // jesli liczba to musimy doczytac do spacji
  if (isdigit(elem->c)) {
    pom = elem;
    to_do = argument;
    if (read(0, &c, sizeof(char)) == -1) syserr("W, read(0)");
    while (c != ' ' && c != '#') {
      new = new_node(c);
      pom->next = new;
      pom = pom->next;
      if (read(0, &c, sizeof(char)) == -1) syserr("W, read(0)");
    }
    if (c == '#') {
      new = new_node(c);
      guard->next = new;
      act = new;
      left = 2;
      first = act;
    }
  } else if (isalpha(elem->c))
    to_do = argument;
  else if (elem->c == '(')
    to_do = lbracket;
  else if (elem->c == ')')
    to_do = rbracket;
  else
    to_do = operator;

  for (int i = 0; i < left; i++) {
    c = ' ';
    while (c != '#') {
      if (read(0, &c, sizeof(char)) == -1) syserr("W, read(0)");
      new = new_node(c);
      act->next = new;
      act = act->next;  // pamietac koniec
    }
    // zapamietanie wskaznikow do pierwszego i drugiego hasha w wyrazeniu
    if (first == guard)
      first = act;
    else if (second == guard)
      second = act;
  }
  while (second->next->c != '#') second = second->next;
  second->next->next = NULL;
  pom = elem;
  switch (to_do) {
    case argument:
      new = new_node(' ');
      new->next = second->next;
      second->next = new;
      second = new;
      pom = new->next;
      second->next = elem;
      while (second->next != NULL) second = second->next;
      second->next = pom;
      break;
    case lbracket:
      elem->next = first->next;
      first->next = elem;
      break;
    case rbracket:
      pom = first->next;
      while (pom->c != '(') {
        new = new_node(' ');
        new->next = second->next;
        second->next = new;
        second = new;
        first->next = pom->next;
        pom->next = second->next;
        second->next = pom;
        second = pom;
        pom = first->next;
      }
      first->next = pom->next;
      free(pom);
      free(elem);
      break;
    case operator:
      a = elem->c;
      if (first->next->c != '#' && first->next->c != '(') {
        char b = first->next->c;
        if (a == '+' || a == '-') x1 = 3;
        if (a == '*' || a == '/') x1 = 2;
        if (a == '^') x1 = 1;
        if (b == '+' || b == '-') x2 = 3;
        if (b == '*' || b == '/') x2 = 2;
        if (b == '^') x2 = 1;
        if (x1 < x2) {
          elem->next = first->next;
          first->next = elem;
        } else {
          pom = first->next;
          elem->next = pom->next;
          first->next = elem;
          // przesuniecie na element przed ostatnim hashem
          while (second->next->c != '#') second = second->next;
          new = new_node(' ');
          new->next = second->next;
          second->next = new;
          second = new;
          pom->next = second->next;
          second->next = pom;
          // dokladam od razu do wyjscia
        }
      } else {
        elem->next = first->next;
        first->next = elem;
      }
      break;
  }

  while (guard->next->c == ' ') {
    pom = guard->next;
    guard->next = pom->next;
    free(pom);
  }

  // sprawdzamy czy to juz nie koniec
  if (guard->next->c == '#') {
    pom = second->next;
    if (first->next->c != '#') {
      act = first;
      while (act->next->c != '#') {
        new = new_node(' ');
        pom = act->next;
        act->next = new;
        new->next = pom;
        act = pom;
      }
      pom = first->next;
      first->next = act->next;
      act->next = second->next;
      second->next = pom;
    }
    pom = guard->next->next->next->next;
    free(guard->next->next->next);  // space
    free(guard->next->next);        // second hash
    free(guard->next);              // first hash
    free(guard);                    // guard
    guard = pom;
  }

  return guard;
}

int main(int argc, char* argv[]) {
  int stop = 0;
  int pipe_dsc[2][2];
  node_ *guard, *act;
  char c;
  while (!stop) {
    // Wykonanie kroku algorytmu
    guard = run();
    if (guard->c == '&') {
      // Otwarcie nowych łączy
      if (pipe(pipe_dsc[0]) == -1) syserr("Error in pipe\n");
      if (pipe(pipe_dsc[1]) == -1) syserr("Error in pipe\n");

      switch (fork()) {
        case -1:
          syserr("Error in fork()\n");
        // Proces potomny
        case 0:
          if (close(0) == -1) syserr("W, close(0)");
          if (close(1) == -1) syserr("W, close(1)");
          if (dup(pipe_dsc[0][0]) == -1) syserr("W, dup(pipe_dsc[0][0])");
          if (dup(pipe_dsc[1][1]) == -1) syserr("W, dup(pipe_dsc[1][1])");
          if (close(pipe_dsc[0][0]) == -1) syserr("W, close(pipe_dsc[0][0])");
          if (close(pipe_dsc[0][1]) == -1) syserr("W, close(pipe_dsc[0][1])");
          if (close(pipe_dsc[1][0]) == -1) syserr("W, close(pipe_dsc[1][0])");
          if (close(pipe_dsc[1][1]) == -1) syserr("W, close(pipe_dsc[1][1])");
          break;
        // Proces macierzysty
        default:
          // Proces tylko raz ma się powielić
          stop = 1;
          // Zamknięcie nieużywanych łączy
          if (close(pipe_dsc[1][1]) == -1) syserr("W, close(pipe_dsc[1][1])");
          if (close(pipe_dsc[0][0]) == -1) syserr("W, close(pipe_dsc[0][0])");
          act = guard->next;
          free(guard);
          guard = act;
          // Przesłanie napisu do procesu potomnego
          while (act != NULL) {
            if (write(pipe_dsc[0][1], &act->c, sizeof(char)) == -1)
              syserr("W, write(pipe_dsc[0][1])");
            act = act->next;
            free(guard);
            guard = act;
          }
          c = ' ';
          // Odczytanie wyniku od procesu potomnego i przesłanie do
          // procesu macierzystego
          while (c != '#') {
            if (read(pipe_dsc[1][0], &c, sizeof(char)) == -1)
              syserr("W, read(pipe_dsc[1][0])");
            if (write(1, &c, sizeof(char)) == -1) syserr("W, write(1)");
          }
          // Oczekiwanie na zamknięcie procesu potomnego, zamknięcie łączy
          // i zakończenie pracy
          if (wait(0) == -1) syserr("wait");
          if (close(pipe_dsc[1][0]) == -1) syserr("W, close(pipe_dsc[1][0])");
          if (close(pipe_dsc[0][1]) == -1) syserr("W, close(pipe_dsc[0][1])");
          if (close(0) == -1) syserr("W, close(0)");
          if (close(1) == -1) syserr("W, close(1)");
          exit(0);
      }
    // Proces wykonał ostatni krok algorytmu
    } else {
      act = guard;
      // Przesłanie wyniku do procesu macierzystego
      while (act != NULL) {
        if (write(1, &act->c, sizeof(char)) == -1) syserr("W(final), write(1)");
        act = act->next;
        free(guard);
        guard = act;
      }
      if (close(0) == -1) syserr("W, close(0)");
      if (close(1) == -1) syserr("W, close(1)");
      exit(0);
    }
  }
}
