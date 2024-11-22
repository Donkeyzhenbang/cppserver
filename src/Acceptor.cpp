#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Server.h"
#include "Channel.h"

#define NET_IP "127.0.0.1"
#define NET_PORT 8888

Acceptor::Acceptor(EventLoop* loop) : loop_(loop)
{
    sock_ = new Socket();
    addr_ = new InetAddress(NET_IP, NET_PORT);
    sock_->bind(addr_); 
    sock_->listen();
    sock_->setnonblocking();
    acceptChannel_ = new Channel(loop_, sock_->getFd());
    //!预先绑定回调函数，服务器每当有一个连接到来，执行newConnection函数
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    acceptChannel_->setCallback(cb);
    acceptChannel_->enableReading();
}

Acceptor::~Acceptor()
{
    delete sock_;
    delete addr_;
    delete acceptChannel_;
}

void Acceptor::acceptConnection()
{
    newConnectionCallback(sock_);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> cb)
{
    newConnectionCallback = cb;
}