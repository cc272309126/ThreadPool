#include "Client.h"

int main(int argc, char** argv) {
  int port = 8081;
  if (argc > 1) {
    port = std::stoi(std::string(argv[1]));
  }
  SimpleClient client("localhost", port);

  // for (int i = 0; i < 1000; i++) {
  //   if (client.RunOneSession() != 0) {
  //     fprintf(stderr, "FAILED\n");
  //     return -1;
  //   }
  //   std::cout << "Success :) " << i << std::endl;
  // }
  client.RunConcurrentSessions(1000);
  return 0;
}
