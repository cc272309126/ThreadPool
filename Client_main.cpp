#include "Client.h"

int main(int argc, char** argv) {
  SimpleClient client("localhost", 8081);
  for (int i = 0; i < 1; i++) {
    client.StartSession();
  }
}