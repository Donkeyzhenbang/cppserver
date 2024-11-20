#pragma once
#include <sys/epoll.h>
#include <functional>
class EventLoop;
class Channel{
private:
    EventLoop* loop_;
    int fd_;
    uint32_t events_;
    uint32_t revents_;
    bool inEpoll_;
    std::function<void()> callback_;
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent();
    void enableReading();

    int  getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();

    void setRevents(uint32_t);
    void setCallback(std::function<void()>);
};