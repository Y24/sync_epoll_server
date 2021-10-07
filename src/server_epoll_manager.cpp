#include "server_epoll_manager.h"
EpollManager::EpollManager(EpollManager& other)
    : listenFd(other.listenFd),
      size(other.size),
      epollFd(other.epollFd),
      nEvents(other.nEvents),
      events(new epoll_event[nEvents]),
      data(other.data),
      handler(other.handler) {}
EpollManager::EpollManager(int listenFd, int size, int nEvents)
    : listenFd(listenFd),
      size(size),
      nEvents(nEvents),
      events(new epoll_event[nEvents]),
      epollFd(epoll_create(size)),
      data({}),
      handler(EventHandler(epollFd, listenFd)) {}
void EpollManager::work() {
  data = {};
  while (true) {
    int ret = epoll_wait(epollFd, events, nEvents, -1);
    handler.handle(events, ret, data);
  }
}
EpollManager::~EpollManager() {
  delete[] events;
  close(epollFd);
}
