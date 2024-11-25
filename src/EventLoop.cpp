#include <iostream>
#include <vector>
#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "ThreadPool.h"
//!事件驱动
EventLoop::EventLoop() : ep_(nullptr), quit(false), threadpool_(nullptr)
{
    ep_ = new Epoll();
    threadpool_ = new ThreadPool();
}

EventLoop::~EventLoop()
{
    delete ep_;
}

void EventLoop::loop()
{
    std::cout << "Server Starting \n";
    while(!quit){
        std::vector<Channel*> chs;
        chs = ep_->poll(); //!在此执行epoll_wait
        for(auto it = chs.begin(); it != chs.end(); it ++){
            (*it)->handleEvent(); //! update_channel中执行epoll_ctl
        }
    }
}

void EventLoop::updateChannel(Channel* channel)
{
    ep_->updateChannel(channel);
}

void EventLoop::addThread(std::function<void()> func)
{
    threadpool_->add(func);
}