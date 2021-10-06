#include "socket_server.h"
SocketServer::SocketServer(std::string ip, int port, int waitings)
    : ip(ip), port(port), waitings(waitings) {
  fd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, ip.data(), &addr.sin_addr);
}
bool SocketServer::init() {
  return bind(fd, (sockaddr*)&addr, sizeof(addr)) != -1 &&
         listen(fd, waitings) != -1;
}

void SocketServer::serve(int size, int nEvents) {
  EpollManager mangager(fd, size, nEvents);
  mangager.work();
}

SocketServer::~SocketServer() {
  close(fd);
  printf("SocketServer destruct !");
}
