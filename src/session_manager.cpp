#include "session_manager.h"

#include <unistd.h>
Session SessionManager::get(int fd) {
  if (this->pool.count(fd)) return this->pool[fd];
  return Session();
}
bool SessionManager::attach(Session s) {
  auto fdMap = s.getFd();
  for (auto&& [fd, _] : fdMap) {
    if (pool.count(fd)) return false;
  }
  for (auto&& [fd, _] : fdMap) {
    this->pool[fd] = s;
  }
  return true;
}
bool SessionManager::merge(std::vector<int> fd) {
  for (int i : fd) {
    if (this->pool.count(i) == 0) return false;
  }
  Session res;
  for (int i : fd) {
    if (!res.merge(this->pool[i])) return false;
  }
  for (int i : fd) {
    this->pool[i] = res;
  }
  return true;
}
bool SessionManager::detach(int fd) {
  close(fd);
  if (this->pool.count(fd) == 0) return false;
  auto fdMap = this->pool[fd].getFd();
  for (auto&& [i, _] : fdMap) {
    if (this->pool.count(i) == 0) return false;
  }
  for (auto&& [i, _] : fdMap) {
    this->pool.erase(i);
  }
  return true;
}
std::vector<int> SessionManager::getDest(int fd) {
  if (this->pool.count(fd) == 0) return {};
  auto fdMap = this->pool[fd].getFd();
  std::vector<int> res;
  for (auto&& [i, _] : fdMap) {
    if (i != fd) res.emplace_back(i);
  }
  return res;
}
void SessionManager::clear() {
  for (auto [fd, _] : this->pool) {
    close(fd);
  }
  this->pool.clear();
}
SessionManager::~SessionManager() { clear(); }
