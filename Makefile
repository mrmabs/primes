CC=gcc
CFLAGS=-Wall -O3 -ffast-math -funroll-loops
LFLAGS=-lm

all: gen-primes

gen-primes: gen-primes.o
	$(CC) $(LFLAGS) -o gen-primes gen-primes.o
	strip gen-primes

clean:
	rm -fr gen-primes *.o
