#pragma once

class InetAddress;
class Socket
{
private: 
    int fd_;
public:
    Socket();
    Socket(int fd);
    ~Socket();
    void bind(InetAddress* addr);
    void listen();
    void set_nonblocking();
    int accept(InetAddress* addr);
    int get_sockfd();
};