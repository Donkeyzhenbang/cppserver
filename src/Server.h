#pragma once

class EventLoop;
class Socket;
class Server
{
private:
    EventLoop* loop_;
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket* serv_sock);
};