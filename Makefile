CC=g++ -std=c++11 -Wall -Werror
LFLAG=-pthread

default: test

ThreadPool.o: ThreadPool.h ThreadPool.cpp
	$(CC) -c ThreadPool.cpp

main.o: main.cpp
	$(CC) -c main.cpp

test: main.o ThreadPool.o
	$(CC) main.o ThreadPool.o -o test $(LFLAG)

clean:
	rm -rf test
	rm *.o