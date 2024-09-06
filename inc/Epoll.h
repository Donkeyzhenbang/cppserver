#pragma once
#include <sys/epoll.h>
#include <vector>
class Epoll
{
private:
    int epfd_;
    struct epoll_event *events_;
public:
    Epoll();
    ~Epoll();

    void add_fd(int fd, uint32_t op);
    std::vector<epoll_event> poll(int timeout = -1);
};