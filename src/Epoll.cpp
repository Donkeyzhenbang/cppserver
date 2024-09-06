#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include "Epoll.h"
#include "utils.h"
#define MAX_EVENTS 1000

Epoll::Epoll() : epfd_(-1), events_(nullptr)
{
    epfd_ = epoll_create1(0);
    errif(epfd_ == -1, "epoll create error");
    events_ = new epoll_event[MAX_EVENTS];
    bzero(&events_, sizeof(*events_) * MAX_EVENTS);
}

Epoll::~Epoll()
{
    if(epfd_ != -1){
        close(epfd_);
        epfd_ = -1;
    }
    delete [] events_;
}

void Epoll::add_fd(int fd, uint32_t op)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev), "epoll add event error");
}

std::vector<epoll_event> Epoll::poll(int timeout)
{
    std::vector<epoll_event> active_events;
    int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; i ++){
        active_events.push_back(events_[i]);
    }
    return active_events;
}

