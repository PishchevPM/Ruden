CC = g++
CFLAGS = -Wall -g -O2 -I include

all: integral

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -c -o main.o

integral: main.o
	$(CC) $(CFLAGS) main.o -o integral

clean:
	rm -r -f main.o integral