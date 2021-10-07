#include "server_event_handler.h"

/// Note: handle_accpet don't contains the session-pairing work
/// when accpet successed, write `fd` immidiately back to the
/// client.
void EventHandler::handle_accpet(std::unordered_map<int, DemoData> &data) {
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
    data[clientFd] = DemoData(session_init, factory.toString<int>(clientFd));
    eventManager.add_event(clientFd, EPOLLOUT);
    if (!sessionManager.attach(Session(clientFd, InetAddr(addr, port)))) {
      fprintf(stderr, "ServerEventHandler handle_accpet attach fails\n");
    }
  }
}
void EventHandler::do_read(int fd, std::unordered_map<int, DemoData> &data) {
  IOHandler ioHandler(fd);
  auto res = ioHandler.read();
  std::string log =
      "do_read: fd:" + std::to_string(fd) + " ,res: " + res.toStr().c_str();
  logPool.emplace_back(log);
  std::vector<int> destination;
  int targetFd;
  switch (res.getHeader().type) {
    case data_invalid:
      eventManager.delete_event(fd, EPOLLIN);
      eventManager.delete_event(fd, EPOLLOUT);
      sessionManager.detach(fd);
      break;
    case conn_close:
      eventManager.delete_event(fd, EPOLLIN);
      eventManager.delete_event(fd, EPOLLOUT);
      sessionManager.detach(fd);
      break;
    case session_init:
      // this cannot happen in server side.
      fprintf(stderr, "ServerEventHandler do_read meets session_init!\n");
      break;
    case session_pair:
      targetFd = factory.stringTo<int>(res.getBody().content);
      if (!sessionManager.merge({fd, targetFd})) {
        fprintf(stderr, "ServerEventHandler do_read: session_pair fails\n");
        data[fd] = DemoData(session_pair, "NOK");
      } else {
        data[fd] = DemoData(session_pair, "OK");
      }
      eventManager.modify_event(fd, EPOLLOUT);
      break;
    case delivery_data:
      destination = sessionManager.getDest(fd);
      if (destination.size() != 1) {
        fprintf(stderr,
                "ServerEventHandler do_read: delivery_data size == %d!\n",
                destination.size());
      }
      if (!IOHandler(destination[0]).write(res)) {
        fprintf(stderr, "ServerEventHandler delivery_data fails!\n");
      }
      /* data[destination[0]] = res;
      eventManager.add_event(destination[0], EPOLLOUT); */
      break;
    default:
      fprintf(stderr, "ServerEventHandler do_read reach default case!\n");
      break;
  }
}
void EventHandler::do_write(int fd, std::unordered_map<int, DemoData> &data) {
  IOHandler ioHandler(fd);
  std::string log = "do_write: fd:" + std::to_string(fd) +
                    " ,res: " + data[fd].toStr().c_str();
  logPool.emplace_back(log);
  if (!ioHandler.write(data[fd])) {
    fprintf(stderr, "do_write fails: fd(%d),data(%s)", fd,
            data[fd].toStr().c_str());
  }
  eventManager.modify_event(fd, EPOLLIN);
  // clean work
  data.erase(fd);
}

EventHandler::EventHandler(int epollFd, int listenFd)
    : epollFd(epollFd),
      listenFd(listenFd),
      eventManager(EventManager(epollFd)) {
  eventManager.add_event(listenFd, EPOLLIN);
}
void EventHandler::handle(epoll_event *events, int num,
                          std::unordered_map<int, DemoData> &data) {
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