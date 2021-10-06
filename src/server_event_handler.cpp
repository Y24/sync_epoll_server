#include "server_event_handler.h"

/// Note: handle_accpet don't contains the session-pairing work
/// when accpet successed, write `fd` immidiately back to the
/// client.
void EventHandler::handle_accpet(DemoData &data) {
  sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  int clientFd =
      accept(listenFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
  if (clientFd == -1)
    perror("accpet error:");
  else {
    std::string addr(inet_ntoa(clientAddr.sin_addr));
    int port = clientAddr.sin_port;
    printf("accept a new client: %s:%d\n", addr.data(), port);
    data = DemoData(session_init, factory.toString<int>(clientFd));
    eventManager.add_event(clientFd, EPOLLOUT);
    if (!sessionManager.attach(Session(clientFd,InetAddr(addr, port)))) {
      fprintf(stderr, "ServerEventHandler handle_accpet attach fails\n");
    }
  }
}
void EventHandler::do_read(int fd, DemoData &data) {
  IOHandler ioHandler(fd);
  auto res = ioHandler.read();
  std::vector<int> destination;
  int targetFd;
  switch (res.getHeader().type) {
    case data_invalid:
      eventManager.delete_event(fd, EPOLLIN);
      sessionManager.detach(fd);
      break;
    case conn_close:
      if (!sessionManager.detach(fd)) {
        fprintf(stderr, "ServerEventHandler do_read: conn_close fails\n");
      }
      break;
    case session_init:
      // this cannot happen in server side.
      fprintf(stderr, "ServerEventHandler do_read meets session_init!\n");
      break;
    case session_pair:
      targetFd = factory.stringTo<int>(res.getBody().content);
      if (!sessionManager.merge({fd, targetFd})) {
        fprintf(stderr, "ServerEventHandler do_read: session_pair fails\n");
      }
      break;
    case delivery_data:
      destination = sessionManager.getDest(fd);
      if (destination.size() != 1) {
        fprintf(stderr,
                "ServerEventHandler do_read: delivery_data size == %!\n",
                destination.size());
      }
      eventManager.add_event(destination[0], EPOLLOUT);
      break;
    default:
      fprintf(stderr, "ServerEventHandler do_read reach default case!\n");
      break;
  }
}
void EventHandler::do_write(int fd, DemoData &data) {
  IOHandler ioHandler(fd);
  if (!ioHandler.write(data)) {
    eventManager.delete_event(fd, EPOLLOUT);
  }
  // clean work
  data = DemoData();
}

EventHandler::EventHandler(int epollFd, int listenFd)
    : epollFd(epollFd),
      listenFd(listenFd),
      eventManager(EventManager(epollFd)) {
  eventManager.add_event(listenFd, EPOLLIN);
}
void EventHandler::handle(epoll_event *events, int num, DemoData &data) {
  for (int i = 0; i < num; i++) {
    int fd = events[i].data.fd;
    if ((fd == listenFd) && (events[i].events & EPOLLIN))
      handle_accpet(data);
    else if (events[i].events & EPOLLIN)
      do_read(fd, data);
    else if (events[i].events & EPOLLOUT)
      do_write(fd, data);
  }
}