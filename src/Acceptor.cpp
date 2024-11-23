#include <cstdio>
#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Server.h"
#include "Channel.h"

#define NET_IP "127.0.0.1"
#define NET_PORT 8888

Acceptor::Acceptor(EventLoop* loop) : loop_(loop), sock_(nullptr), acceptChannel_(nullptr)
{
    sock_ = new Socket();
    InetAddress* addr = new InetAddress(NET_IP, NET_PORT);
    sock_->bind(addr); 
    sock_->listen();
    sock_->setnonblocking();
    acceptChannel_ = new Channel(loop_, sock_->getFd());
    //!预先绑定回调函数，服务器每当有一个连接到来，执行newConnection函数
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    acceptChannel_->setCallback(cb);
    acceptChannel_->enableReading();
    delete addr;
}

Acceptor::~Acceptor()
{
    delete sock_;
    delete acceptChannel_;
}

void Acceptor::acceptConnection()
{
    InetAddress* clnt_addr = new InetAddress();
    Socket* clnt_sock = new Socket(sock_->accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->getAddr().sin_addr), ntohs(clnt_addr->getAddr().sin_port));
    clnt_sock->setnonblocking();
    newConnectionCallback(clnt_sock);
    delete clnt_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> cb)
{
    newConnectionCallback = cb;
}