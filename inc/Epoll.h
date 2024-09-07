#pragma once
#include <sys/epoll.h>
#include <vector>
class Channel;
class Epoll
{
private:
    int epfd_;
    struct epoll_event *events_;
public:
    Epoll();
    ~Epoll();

    void add_fd(int fd, uint32_t op);
    std::vector<Channel*> poll(int timeout = -1);
};