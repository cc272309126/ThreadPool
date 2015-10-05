CC=g++ -std=c++11 -Wall -Werror
LFLAG=-pthread
OBJ = main.o \
      ThreadPool.o \
      EventManager.o \
      Server.o \
      Socket.o \
      FileDescriptorInterface.o \
      BufferedDataReader.o \
      Strings.o \
      Utils.o \
      StringBuilder.o \

default: test

ThreadPool.o: ThreadPool.h ThreadPool.cpp
	$(CC) -c ThreadPool.cpp

EventManager.o: EventManager.h EventManager.cpp
	$(CC) -c EventManager.cpp

Server.o: Server.h Server.cpp
	$(CC) -c Server.cpp

Socket.o: Socket.h Socket.cpp
	$(CC) -c Socket.cpp

FileDescriptorInterface.o: FileDescriptorInterface.h FileDescriptorInterface.cpp
	$(CC) -c FileDescriptorInterface.cpp

BufferedDataReader.o: BufferedDataReader.h BufferedDataReader.cpp
	$(CC) -c BufferedDataReader.cpp

Strings.o: Strings.h Strings.cpp
	$(CC) -c Strings.cpp

Utils.o: Utils.h Utils.cpp
	$(CC) -c Utils.cpp

StringBuilder.o: StringBuilder.h StringBuilder.cpp
	$(CC) -c StringBuilder.cpp

main.o: main.cpp
	$(CC) -c main.cpp

test: $(OBJ)
	$(CC) $(OBJ) -o test $(LFLAG)

clean:
	rm -rf test
	rm -rf *.o