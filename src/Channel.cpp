#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop* loop, int fd) : loop_(loop), fd_(fd), events_(0), revents_(0), inEpoll_(false)
{

}

Channel::~Channel()
{

}

void Channel::handleEvent()
{
    callback_();
}

void Channel::enableReading()
{
    //events表示希望监听这个文件描述符的哪些事件，因为不同事件的处理方式不一样。
    //revents表示在epoll返回该Channel时文件描述符正在发生的事件。
    //!注意Epoll.h中定义的是struct epoll_event events_ 这里定义为uint32_t
    events_ = EPOLLIN | EPOLLET; 
    loop_->updateChannel(this);
}

int Channel::getFd()
{
    return fd_;
}

uint32_t Channel::getEvents()
{
    return events_;
}

uint32_t Channel::getRevents()
{
    return revents_;
}

bool Channel::getInEpoll()
{
    return inEpoll_;
}

void Channel::setInEpoll()
{
    inEpoll_ = true;
}

void Channel::setRevents(uint32_t ev)
{
    revents_ = ev;
}

void Channel::setCallback(std::function<void()> cb)
{
    callback_ = cb;
}