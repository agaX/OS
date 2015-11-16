all: ToONP W

ToONP: ToONP.o err.o
	cc -Wall -std=c99 -o ToONP ToONP.o err.o

ToONP.o: ToONP.c err.h
	cc -Wall -std=c99 -c ToONP.c

W: W.o err.o
	cc -Wall -std=c99 -o W W.o err.o

W.o: W.c err.h
	cc -Wall -std=c99 -c W.c

err.o: err.c err.h
	cc -Wall -c err.c

clean:
	rm -f *.o ToONP W
