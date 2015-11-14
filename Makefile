all: ToONP process_w

ToONP: ToONP.o err.o
	cc -Wall -std=c99 -o ToONP ToONP.o err.o

ToONP.o: ToONP.c err.h
	cc -Wall -std=c99 -c ToONP.c

process_w: process_w.o err.o
	cc -Wall -std=c99 -o process_w process_w.o err.o

process_w.o: process_w.c err.h
	cc -Wall -std=c99 -c process_w.c

err.o: err.c err.h
	cc -Wall -c err.c

clean:
	rm -f *.o ToONP process_w
