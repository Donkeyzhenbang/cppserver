#pragma once

#include <map>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class Server
{
private:
    EventLoop* loop_;
    Acceptor* acceptor_;
    std::map<int, Connection*> connections;//键为该连接客户端的socket fd，值为指向该连接的指针
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket* serv_sock);
    void deleteConnection(Socket* sock);
};