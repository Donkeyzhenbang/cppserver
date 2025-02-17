#include "Epoll.h"

#include <unistd.h>

#include <cstring>

#include "Channel.h"
#include "utils.h"

#define MAX_EVENTS 1000

Epoll::Epoll() : epfd_(-1), events_(nullptr) {
  epfd_ = epoll_create1(0);
  errif(epfd_ == -1, "epoll create error");
  events_ = new epoll_event[MAX_EVENTS];
  bzero(events_, sizeof(*events_) * MAX_EVENTS);
}

Epoll::~Epoll() {
  if (epfd_ != -1) {
    close(epfd_);
    epfd_ = -1;
  }
  delete[] events_;
}

// void Epoll::addFd(int fd, uint32_t op)
// {
//     struct epoll_event ev;
//     bzero(&ev, sizeof(ev));
//     ev.data.fd = fd;
//     ev.events = op;
//     errif(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
// }

// std::vector<epoll_event> Epoll::event_poll(int timeout)
// {
//     std::vector<epoll_event> activeEvents;
//     int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
//     errif(nfds == -1, "epoll wait error");
//     for(int i = 0; i < nfds; i ++){
//         activeEvents.push_back(events_[i]);
//     }
//     return activeEvents;
// }

std::vector<Channel *> Epoll::poll(int timeout) {
  std::vector<Channel *> activeEvents;
  int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
  errif(nfds == -1, "epoll wait error");
  for (int i = 0; i < nfds; i++) {
    Channel *ch = (Channel *)events_[i].data.ptr;
    ch->setReady(events_[i].events);
    activeEvents.push_back(ch);
  }
  return activeEvents;
}

void Epoll::updateChannel(Channel *Channel) {
  int fd = Channel->getFd();
  struct epoll_event ev;
  bzero(&ev, sizeof(ev));
  ev.data.ptr = Channel;
  ev.events = Channel->getEvents();
  if (!Channel->getInEpoll()) {
    errif(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
    Channel->setInEpoll();
  } else {
    errif(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
  }
}

void Epoll::deleteChannel(Channel *channel) {
  int fd = channel->getFd();
  errif(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) == -1, "epoll delete error");
  channel->setInEpoll(false);
}