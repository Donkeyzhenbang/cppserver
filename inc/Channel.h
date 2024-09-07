#pragma once
#include <sys/epoll.h>
class Epoll;
class Channel
{
private:
    Epoll* ep_;
    int fd_;
    uint32_t events_;
    uint32_t revents_;
    bool in_epoll_;
public:
    Channel(Epoll* ep, int fd);
    ~Channel();

    void enable_reading();
    int get_fd();
    uint32_t get_events();
    uint32_t get_revents();
    bool get_in_epoll();
    void set_in_epoll();
    void set_revents(uint32_t);
};
//events表示希望监听这个文件描述符的哪些事件，因为不同事件的处理方式不一样。
//revents表示在epoll返回该Channel时文件描述符正在发生的事件。
//inEpoll表示当前Channel是否已经在epoll红黑树中，为了注册Channel的时候方便区分使用EPOLL_CTL_ADD还是EPOLL_CTL_MOD。