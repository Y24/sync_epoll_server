#ifndef _SOCKET_SERVER_H
#define _SOCKET_SERVER_H 1
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#include "config.h"
#include "server_epoll_manager.h"

/// The Top class running at the server side.
class SocketServer {
 private:
  std::string ip;
  int port;
  int fd;
  int waitings;
  sockaddr_in addr;

 public:
  SocketServer(std::string ip, int port, int waitings = LISTENQ);
  bool init();
  void serve(int size = FDSIZE, int nEvents = EPOLLEVENTS);
  ~SocketServer();
};
#endif  // socket_server.h
