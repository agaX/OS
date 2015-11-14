#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

int main(int argc, char*argv[]){

    int true = 1;
    int pipe_dsc[2][2];

    while (true) {
        if (pipe (pipe_dsc[0]) == -1) syserr("Error in pipe\n");
    	if (pipe (pipe_dsc[1]) == -1) syserr("Error in pipe\n");

        true = 0;
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

            default:
                if (close(pipe_dsc[1][1]) == -1) syserr("W(), close(pipe_dsc[1][1])");
                if (close(pipe_dsc[0][0]) == -1) syserr("W(), close(pipe_dsc[0][0])");



        }

    }


}
