#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H 1
#include <map>
#include <string>
#include <unordered_map>

#include "session.h"
class SessionManager {
 private:
  // fd: Session
  std::unordered_map<int, Session> pool;

 public:
  SessionManager() = default;
  bool attach(Session s);
  bool merge(std::vector<int> fd);
  bool detach(int fd);
  Session get(int fd);
  std::vector<int> getDest(int fd);
  void clear();
  ~SessionManager();
};

#endif  // session_manager.h