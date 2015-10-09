CC=g++ -std=c++11
CFLAGS=-Wall -Werror
LFLAG=-pthread
SRC_DIR=src
OBJ_DIR=lib

OBJ = lib/ThreadPool.o \
      lib/EventManager.o \
      lib/Server.o \
      lib/Client.o \
      lib/Socket.o \
      lib/FileDescriptorInterface.o \
      lib/BufferedDataReader.o \
      lib/Strings.o \
      lib/Utils.o \
      lib/StringBuilder.o \

default: test client server

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp
	$(CC) -c $(SRC_DIR)/main.cpp -o $@

$(OBJ_DIR)/Client_main.o: $(SRC_DIR)/Client_main.cpp
	$(CC) -c $(SRC_DIR)/Client_main.cpp -o $@

$(OBJ_DIR)/Server_main.o: $(SRC_DIR)/Server_main.cpp
	$(CC) -c $(SRC_DIR)/Server_main.cpp -o $@

test: $(OBJ) $(OBJ_DIR)/main.o
	$(CC) $(OBJ) $(OBJ_DIR)/main.o -o test $(LFLAG)

client: $(OBJ) $(OBJ_DIR)/Client_main.o
	$(CC) $(OBJ) $(OBJ_DIR)/Client_main.o -o client $(LFLAG)

server: $(OBJ) $(OBJ_DIR)/Server_main.o
	$(CC) $(OBJ) $(OBJ_DIR)/Server_main.o -o server $(LFLAG)

clean:
	rm -rf test server client
	rm -rf lib/*.o
