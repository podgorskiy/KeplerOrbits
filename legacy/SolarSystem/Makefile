CC=g++
CFLAGS= -Wall -Os
all: solarsystem.o
	$(CC) solarsystem.o -o solarsystem

solarsystem.o: solarsystem.c
	$(CC) -c $(CFLAGS)  solarsystem.c

clean:
	rm -rf *.o solarsystem