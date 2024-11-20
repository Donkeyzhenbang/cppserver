#include <unistd.h>     //write read等系统调用
#include <fcntl.h>      //用于文件控制操作（如open）
#include <sys/socket.h>
#include "Socket.h"
#include "InetAddress.h"
#include "utils.h"

Socket::Socket() : fd_(-1)
{
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd_ == -1, "socket create error");
}

Socket::Socket(int fd) : fd_(fd)
{
    errif(fd_ == -1, "socket create error");
}

Socket::~Socket()
{
    if(fd_ != -1){
        close(fd_);
        fd_  = -1;
    }
}

void Socket::bind(InetAddress* addr)
{
    errif(::bind(fd_, (sockaddr*)&addr->addr, addr->addr_len), "socket bind error");
}

void Socket::listen(){
    errif(::listen(fd_, SOMAXCONN) == -1, "socket listen error");
}

int Socket::accept(InetAddress* addr)
{   
    int clnt_sockfd = ::accept(fd_, (sockaddr*)&addr->addr, &addr->addr_len);
    errif(clnt_sockfd == -1, "socket accept error");
    return clnt_sockfd;
}

void Socket::setnonblocking()
{
    fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
}

int Socket::getFd()
{
    return fd_;
}
