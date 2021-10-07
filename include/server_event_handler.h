#ifndef _SERVER_EVENT_HANDLER_H
#define _SERVER_EVENT_HANDLER_H 1
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <unordered_map>

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
  std::vector<std::string> logPool;
  /// Note: handle_accpet don't contains the session-pairing work
  /// when accpet successed, write `fd` immidiately back to the
  /// client.
  void handle_accpet(std::unordered_map<int, DemoData> &data);
  void do_read(int fd, std::unordered_map<int, DemoData> &data);
  void do_write(int fd, std::unordered_map<int, DemoData> &data);

 public:
  EventHandler(int epollFd, int listenFd);
  void handle(epoll_event *events, int num,
              std::unordered_map<int, DemoData> &data);
  ~EventHandler() = default;
};
#endif  // server_event_handler.h