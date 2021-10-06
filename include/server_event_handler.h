#ifndef _SERVER_EVENT_HANDLER_H
#define _SERVER_EVENT_HANDLER_H 1
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "event_manager.h"
#include "io_handler.h"
#include "session_manager.h"
class EventHandler {
 private:
  int epollFd;
  int listenFd;
  EventManager eventManager;
  SessionManager sessionManager;
  DataFactory factory;
  /// Note: handle_accpet don't contains the session-pairing work
  /// when accpet successed, write `fd` immidiately back to the
  /// client.
  void handle_accpet(DemoData &data);
  void do_read(int fd, DemoData &data);
  void do_write(int fd, DemoData &data);

 public:
  EventHandler(int epollFd, int listenFd);
  void handle(epoll_event *events, int num, DemoData &data);
  ~EventHandler() = default;
};
#endif  // server_event_handler.h