#include <iostream>
#include <functional>
#include <cstring>
#include <unistd.h>
#include "Server.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Connection.h"

#define READ_BUFFER 1024


Server::Server(EventLoop* loop) : loop_(loop), acceptor_(nullptr)
{
    acceptor_ = new Acceptor(loop_);
    //!std::placeholders::_1 是一个占位符，表示绑定的函数对象在调用时会接收的第一个参数。
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor_->setNewConnectionCallback(cb);
}

Server::~Server()
{
    delete acceptor_;
}


//!客户端每次连接上上来，会先执行这个函数，为客户端绑定事件回调函数handleReadEvent，当发生可读事件时相应。
void Server::newConnection(Socket* serv_sock)
{
    if(serv_sock->getFd() != -1){
        Connection* conn = new Connection(loop_, serv_sock);
        std::function<void(int)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        conn->setDeleteConnectionCallback(cb);
        connections[serv_sock->getFd()] = conn;
    }


    // InetAddress* clnt_addr = new InetAddress();
    // Socket* clnt_sock = new Socket(serv_sock->accept(clnt_addr));
    // printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
    // clnt_sock->setnonblocking();
    // Channel* clnt_channel = new Channel(loop_, clnt_sock->getFd());
    // std::function<void()> cb = std::bind(&Server::handleReadEvent, this, clnt_sock->getFd());
    // clnt_channel->setCallback(cb);
    // clnt_channel->enableReading();
}

void Server::deleteConnection(int sockfd)
{
    // Connection* conn = connections[sock->getFd()];
    // connections.erase(sock->getFd());
    // delete conn;
    if(sockfd != -1){
        auto it = connections.find(sockfd);
        if(it != connections.end()){
            Connection *conn = connections[sockfd];
            connections.erase(sockfd);
            delete conn;
        }
    }

}