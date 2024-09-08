#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include "Epoll.h"
#include "utils.h"
#include "Channel.h"
#define MAX_EVENTS 1000

Epoll::Epoll() : epfd_(-1), events_(nullptr)
{
    epfd_ = epoll_create1(0);
    errif(epfd_ == -1, "epoll create error");
    events_ = new epoll_event[MAX_EVENTS];
    bzero(events_, sizeof(*events_) * MAX_EVENTS);  //!在类中初次定义时已经是指针类型struct epoll_event *events_
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


/**
 * @brief poll是针对已经存进epoll实例中的待监听端口，核心函数epoll_wait()
 * 
 * @param timeout 
 * @return std::vector<Channel*> 
 */
std::vector<Channel*> Epoll::poll(int timeout)
{
    std::vector<Channel*> active_events;
    int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; i ++){
        Channel* ch = (Channel*)events_[i].data.ptr;
        ch->set_revents(events_[i].events);
        active_events.push_back(ch);
    }
    return active_events;
} 

/**
 * @brief update_channel核心函数时
 * epoll_ctl将需要监听事件丢进epoll红黑树 这里让ev.data.ptr (void*)类型指向channel
 * 后面发生响应时再将其赋值给Channel* ch 将这些push进vector<Channel*>数组中
 * 
 * @param channel 
 */
void Epoll::update_channel(Channel* channel)
{
    int fd = channel->get_fd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->get_events();//先前events_已经设置为EPOLLIN | EPOLLET
    if(!channel->get_in_epoll()){
        errif(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        channel->set_in_epoll();
    }else{
        errif(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
    }
}