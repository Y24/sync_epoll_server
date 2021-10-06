#ifndef _SERVER_EPOLL_MANAGER_H
#define _SERVER_EPOLL_MANAGER_H 1
#include <sys/epoll.h>
#include <unistd.h>

#include "server_event_handler.h"
class EpollManager {
 private:
  int listenFd;
  int size;
  int epollFd;
  epoll_event* events;
  int nEvents;
  DemoData data;

  EventHandler handler;

 public:
  EpollManager(int listenFd, int size, int nEvents);
  EpollManager(EpollManager& other);
  void work();
  ~EpollManager();
};
#endif // server_epoll_manager.h