#ifndef _EVENT_MANAGER_H
#define _EVENT_MANAGER_H 1
#include <sys/epoll.h>
class EventManager {
 private:
  int epollFd;
  void do_event(int fd, int state, int type);

 public:
  EventManager(int epollFd);
  void add_event(int fd, int state);

  void modify_event(int fd, int state);

  void delete_event(int fd, int state);
  ~EventManager() = default;
};
#endif  // event_manager.h