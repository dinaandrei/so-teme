CC = gcc
CFLAGS = -c -Wall -fpic

build: libso_stdio.so

libso_stdio.so: so_stdio.o
	$(CC) -shared -o libso_stdio.so so_stdio.o

so_stdio.o: so_stdio.c
	$(CC) $(CFLAGS) so_stdio.c

clean:
	rm so_stdio.o libso_stdio.so
