#ifndef _SESSION_H
#define _SESSION_H 1
#include <unordered_map>
#include <vector>

#include "inet_addr.h"

class Session {
 private:
  std::unordered_map<int, InetAddr> fd;

 public:
  Session() = default;
  Session(std::vector<int> fd, std::vector<InetAddr> addrs);
  Session(int fd, InetAddr addr);
  bool insert(int fd, InetAddr addr);
  bool contains(int fd);
  bool merge(const Session& other);
  ~Session() = default;
  bool isNull() const;
  std::unordered_map<int, InetAddr> getFd() const;
};
#endif  // session.h
