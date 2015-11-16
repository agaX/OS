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
  struct node *next;
} node_;

int main(int argc, char*argv[]){

    int true1 = 1;
    int pipe_dsc[2][2];

    while (true1) {
        if (pipe (pipe_dsc[0]) == -1) syserr("Error in pipe\n");
    	if (pipe (pipe_dsc[1]) == -1) syserr("Error in pipe\n");

        true1 = 0;
        switch (fork()) {
            case -1:
                syserr("Error in fork()\n");

            case 0:
                if (close (0) == -1) syserr("W(1), close(0)");
                if (close (1) == -1) syserr("W(1), close(1)");
                if (dup (pipe_dsc[0][0]) == -1) syserr("W(1), dup(pipe_dsc[0][0])");
                if (dup (pipe_dsc[1][1]) == -1) syserr("W(1), dup(pipe_dsc[1][1])");
                if (close (pipe_dsc[0][0]) == -1) syserr("W(1), close(pipe_dsc[0][0])");
                if (close (pipe_dsc[0][1]) == -1) syserr("W(1), close(pipe_dsc[0][1])");
                if (close (pipe_dsc[1][0]) == -1) syserr("W(1), close(pipe_dsc[1][0])");
                if (close (pipe_dsc[1][1]) == -1) syserr("W(1), close(pipe_dsc[1][1])");
                break;

            default:
                if (close(pipe_dsc[1][1]) == -1) syserr("W(), close(pipe_dsc[1][1])");
                if (close(pipe_dsc[0][0]) == -1) syserr("W(), close(pipe_dsc[0][0])");

        }

    }


    char c = ' ';
    node_ *act;
    node_ *guard;
    node_ *new;
    guard = (node_*)malloc(sizeof(node_));
    guard->next = NULL;
    act = guard;

    int to_do = 0; // 1 - liczba lub stała, 2 - lewy nawias, 3- prawy, 4 - znak

    node_ *elem;
    elem = (node_*)malloc(sizeof(node_));
    elem->next = NULL;

    //czytamy wyrazenie do przetworzenia
    while (c == ' ')
      if (read (0, &c, sizeof(char)) == -1) syserr("ToONP---, read(pipe_dsc[0][0])");

    elem->c = c;

    //jesli liczba to musimy doczytac do spacji
    if (isdigit(elem->c)) {
        node_ *pom;
        pom = elem;
        to_do = 1;
        if (read (0, &c, sizeof(char)) == -1) syserr("ToONP---, read(pipe_dsc[0][0])");
        while (c != ' ' && c != '#') {
            new = (node_*)malloc(sizeof(node_));
        	new->c = c;
        	new->next = NULL;
        	pom->next = new;
	        pom = pom->next;
            if (read (0, &c, sizeof(char)) == -1) syserr("ToONP---, read(pipe_dsc[0][0])");
        }
        if (c == '#') {
            new = (node_*)malloc(sizeof(node_));
            new->c = '#';
            new->next = NULL;
            guard->next = new;
            act = new;
        }
    }

    else if (isalpha(elem->c)) to_do = 1;
    else if (elem->c == '(') to_do = 2;
    else if (elem->c == ')') to_do = 3;
    else if (elem->c == '#') to_do = 5;
    else  to_do = 4;
    printf("%d\n", to_do);

    node_ *pom = elem;
    node_ *first_hash = guard;
    node_ *second_hash = guard;

    printf("Wyrazenie do przetworzenia to: ");
    while (pom != NULL) {
      printf("%c", pom->c);
      pom = pom->next;
    }
    printf("\n");

        int left;
        if (c == '#') //wyrazenie bylo ostatnim przeczytanym
            left = 2;
        else
            left = 3;

        for (int i = 0; i < left; i++) {
          c = ' ';
          if (left == 2)
            first_hash = act;
          //printf("to powinien być #:  %c\n", act->c);
          while (c != '#') {
            if (read (0, &c, sizeof(char)) == -1) syserr("ToONP---, read(pipe_dsc[0][0])");
            //printf("wczytalem %c\n", c);
            new = (node_ *)malloc(sizeof(node_));
            new->c = c;
            act->next = new;
            new->next = NULL;
            act = act->next; // pamietac koniec
          }
            // zapamietanie wskaxnikow do pierwszego i drugiego hasah w wyrazeniu
            if (first_hash == guard)
                first_hash = act;
            else if (second_hash == guard)
                second_hash = act;
        }
    if (to_do != 5) {
        while (second_hash->next->c != '#')
            second_hash = second_hash->next;
        //printf("to second: %c\nn\n\n\n\n", second_hash->next->c);
        pom = elem;
        if (to_do == 1) {
            new = (node_ *)malloc(sizeof(node_));
            new->c = ' ';
            new->next = second_hash->next;
            second_hash->next = new;
            second_hash = new;
            while (pom != NULL) {
                pom = pom->next;
                elem->next = second_hash->next;
                second_hash->next = elem;
                second_hash = elem;
                elem = pom;
            }
        } else if (to_do == 2) {
            elem->next = first_hash->next;
            first_hash->next = elem;
        } else if (to_do == 3) {
            pom = first_hash;
            pom = pom->next;
            while (pom->c != '(') {
                new = (node_ *)malloc(sizeof(node_));
                new->c = ' ';
                new->next = second_hash->next;
                second_hash->next = new;
                second_hash = new;
                first_hash->next = pom->next;
                pom->next = second_hash->next;
                second_hash->next = pom;
                pom = first_hash->next;
            }
            first_hash->next = pom->next;
            free(pom);
        } else if (to_do == 4) {
            int x1, x2;
            char a = elem->c;
            if (first_hash->next->c != '#' && first_hash->next->c != '(') {
                char b = first_hash->next->c;
                if (a == '+' || a == '-')  x1 = 3;
                if (a == '*' || a == '/')  x1 = 2;
                if (a == '^')  x1 = 1;
                if (b == '+' || b == '-')  x2 = 3;
                if (b == '*' || b == '/')  x2 = 2;
                if (b == '^')  x2 = 1;
                // to po prostu dokładam na stos
                if (x1 < x2) {
                    elem->next = first_hash->next;
                    first_hash->next = elem;
                // sciagam ze sotsu to co bylo i wrzucam do wyjscia, a - dokladam na stos
            } else {
                    pom = first_hash->next;
                    elem->next = pom->next;
                    first_hash->next = elem;
                    // przesuniecie na element przed ostatnim hashem
                    while (second_hash->next->c != '#')
                        second_hash = second_hash->next;
                    new = (node_ *)malloc(sizeof(node_));
                    new->c = ' ';
                    new->next = second_hash->next;
                    second_hash->next = new;
                    second_hash = new;
                    pom->next = second_hash->next;
                    second_hash->next = pom;
                // dokladam od razu do wyjscia
                }
            } else {
                elem->next = first_hash->next;
                first_hash->next = elem;
            }
        } else {
            /*pom = first_hash->next;
            while (first_hash->next != second_hash) {
                first_hash->next = pom->next;

            }*/
        }
    }

    act = guard;
    printf("Pozostalo do przetworzenia: ");
    while (act != NULL) {
      printf("%c", act->c);
      new = act;
      act = act->next;
      //temporarly:
     // if (act != NULL)
      //  if (write (1, &act->c, sizeof(char)) == -1) syserr("process_w, write 1");
      free(new);
    }
    printf("\n");

}
