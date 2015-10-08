#include "Server.h"

int main(int argc, char** argv) {
  SimpleServer server(8081);
  server.Start();
}
