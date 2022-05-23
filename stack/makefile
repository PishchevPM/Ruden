CC = g++
CFLAGS = -std=c++11 -g -O2 -Wall -I include

all: mystack

run: mystack
	./mystack

cleanrun: cleanmake
	./mystack

cleanmake: clean mystack

mystack.o: source/mystack_test.cpp
	$(CC) $(CFLAGS) source/mystack_test.cpp -c -o mystack.o

mystack: mystack.o
	$(CC) $(CFLAGS) mystack.o -o mystack

clean:
	rm -r -f mystack.o mystack