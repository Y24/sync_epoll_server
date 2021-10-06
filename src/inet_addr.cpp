#include "inet_addr.h"
InetAddr::InetAddr(std::string ip, int port) : ip(ip), port(port) {}
bool InetAddr::operator==(const InetAddr& other) const {
  return ip == other.ip && port == other.port;
}