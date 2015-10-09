#include <string>
#include "Server.h"

int main(int argc, char** argv) {
  int port = 8081;
  if (argc > 1) {
    port = std::stoi(std::string(argv[1]));
  }
  SimpleServer server(port);
  server.Start();
}
