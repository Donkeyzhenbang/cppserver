#include <iostream>
#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop* loop, int fd) : loop_(loop), fd_(fd), events_(0), ready_(0), inEpoll_(false)
{

}

Channel::~Channel()
{

}

void Channel::handleEvent()
{
    // callback_();
    //!添加线程实际上向线程任务队列添加任务
    // loop_->addThread(callback_);
    // std::cout << "add thread \n";
    if(ready_ & (EPOLLIN | EPOLLPRI)){
        if(useThreadPool_)
            loop_->addThread(readCallback_);
        else 
            readCallback_();
    }
    if(ready_ & (EPOLLOUT)){
        if(useThreadPool_)
            loop_->addThread(writeCallback_);
        else
            writeCallback_();
    }
}

void Channel::enableReading()
{
    //events表示希望监听这个文件描述符的哪些事件，因为不同事件的处理方式不一样。
    //revents表示在epoll返回该Channel时文件描述符正在发生的事件。
    //!注意Epoll.h中定义的是struct epoll_event events_ 这里定义为uint32_t
    events_ |= EPOLLIN | EPOLLPRI; 
    loop_->updateChannel(this);
}

void Channel::useET()
{
    events_ |= EPOLLET;
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

uint32_t Channel::getReady()
{
    return ready_;
}

bool Channel::getInEpoll()
{
    return inEpoll_;
}

void Channel::setInEpoll(bool in)
{
    inEpoll_ = in;
}

void Channel::setReady(uint32_t ev)
{
    ready_ = ev;
}

void Channel::setReadCallback(std::function<void()> cb)
{
    readCallback_ = cb;
}

void Channel::setUseThreadPool(bool use)
{
    useThreadPool_ = use;
}