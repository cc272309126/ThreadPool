CC=g++ -std=c++11 -Wall -Werror
LFLAG=-pthread
OBJ = ThreadPool.o \
      EventManager.o \
      Server.o \
      Client.o \
      Socket.o \
      FileDescriptorInterface.o \
      BufferedDataReader.o \
      Strings.o \
      Utils.o \
      StringBuilder.o \

default: test client server

ThreadPool.o: ThreadPool.h ThreadPool.cpp
	$(CC) -c ThreadPool.cpp

EventManager.o: EventManager.h EventManager.cpp
	$(CC) -c EventManager.cpp

Server.o: Server.h Server.cpp
	$(CC) -c Server.cpp

Client.o: Client.h Client.cpp
	$(CC) -c Client.cpp

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

Client_main.o: Client_main.cpp
	$(CC) -c Client_main.cpp

Server_main.o: Server_main.cpp
	$(CC) -c Server_main.cpp

test: $(OBJ) main.o
	$(CC) $(OBJ) main.o -o test $(LFLAG)

client: $(OBJ) Client_main.o
	$(CC) $(OBJ) Client_main.o -o client $(LFLAG)

server: $(OBJ) Server_main.o
	$(CC) $(OBJ) Server_main.o -o server $(LFLAG)

clean:
	rm -rf test server client
	rm -rf *.o
