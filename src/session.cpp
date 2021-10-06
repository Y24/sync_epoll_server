#include "session.h"
Session::Session(int fd, InetAddr addr) { this->fd[fd] = addr; }
Session::Session(std::vector<int> fd, std::vector<InetAddr> addrs) {
  for (int i = 0; i < fd.size(); i++) {
    this->fd[fd[i]] = addrs[i];
  }
}
bool Session::contains(int fd) { return this->fd.count(fd); }
bool Session::insert(int fd, InetAddr addr) {
  if (this->fd.count(fd)) return false;
  this->fd[fd] = addr;
  return true;
}
bool Session::isNull() const {
  if (fd.empty()) return true;
  for (auto [fd, _] : fd)
    if (fd == -1) return true;
  return true;
}
bool Session::merge(const Session& other) {
  for (auto [fd, _] : other.getFd()) {
    if (this->contains(fd)) return false;
  }
  for (auto [fd, addr] : other.getFd()) {
    this->insert(fd, addr);
  }
  return true;
}
std::unordered_map<int, InetAddr> Session::getFd() const { return fd; }
