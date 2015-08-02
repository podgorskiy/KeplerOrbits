CC=g++
CFLAGS= -Wall -Os
all: solarsystem.o
	$(CC) solarsystem.o -o solarsystem

solarsystem.o: solarsystem.cpp
	$(CC) -c $(CFLAGS)  solarsystem.cpp

clean:
	rm -rf *.o solarsystem