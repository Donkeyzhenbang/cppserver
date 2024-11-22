#include <vector>
#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"

//!事件驱动
EventLoop::EventLoop() : ep_(nullptr), quit(false)
{
    ep_ = new Epoll();
}

EventLoop::~EventLoop()
{
    delete ep_;
}

void EventLoop::loop()
{
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