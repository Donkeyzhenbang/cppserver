#pragma once


#include <map>
#include <vector>
#include "ThreadPool.h"

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class Server
{
private:
    // EventLoop* loop_;
    EventLoop* mainReactor_;
    Acceptor* acceptor_;
    std::map<int, Connection*> connections_;//键为该连接客户端的socket fd，值为指向该连接的指针
    std::vector<EventLoop*> subReactors_;
    ThreadPool* thpool_;
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket* serv_sock);
    void deleteConnection(int sockfd);
};