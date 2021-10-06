#include "socket_server.h"
int main(int argc, char const *argv[]) {
  SocketServer server("127.0.0.1", 8786);
  if (!server.init())
    perror("SocketServer init fails: ");
  else
    server.serve();
  return 0;
}
