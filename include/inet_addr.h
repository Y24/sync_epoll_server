#ifndef _INET_ADDR_H
#define _INET_ADDR_H 1
#include <string>
struct InetAddr {
  std::string ip;
  int port;
  InetAddr() = default;
  InetAddr(std::string ip, int port);
  bool operator==(const InetAddr& other) const;
};
#endif  // inet_addr.h