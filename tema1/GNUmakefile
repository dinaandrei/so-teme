CC = gcc
CFLAGS = -Wall -g

build: tema1

tema1: tema1.o
	$(CC) $(CFLAGS) -o tema1 tema1.o -lcompare -L.

tema1.o: tema1.c

clean:
	rm -f *.o *~ tema1
