CC=g++ -std=c++11
LFLAG=-pthread

default: test

test: main.cpp ThreadPool.h
	$(CC) main.cpp -o test $(LFLAG)

clean:
	rm -rf test