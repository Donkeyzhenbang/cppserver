#pragma once
#include "InetAddress.h"
class InetAddress;
class Socket {
 private:
  int fd_;

 public:
  Socket();
  Socket(int fd);
  ~Socket();

  void bind(InetAddress *);
  void listen();
  void setnonblocking();

  int accept(InetAddress *);
  void connect(InetAddress *);
  int getFd();
};