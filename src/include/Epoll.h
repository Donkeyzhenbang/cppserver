#pragma once
#include <sys/epoll.h>

#include <vector>

class Channel;
class Epoll {
 private:
  int epfd_;
  struct epoll_event *events_;

 public:
  Epoll();
  ~Epoll();

  // void addFd(int fd, uint32_t op);
  void updateChannel(Channel *);
  void deleteChannel(Channel *);
  // std::vector<epoll_event> event_poll(int timeout = -1);
  std::vector<Channel *> poll(int timeout = -1);
};