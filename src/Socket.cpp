#include <unistd.h>     //write read等系统调用
#include <fcntl.h>      //用于文件控制操作（如open）
#include <sys/socket.h>
#include <cstring>
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

void Socket::bind(InetAddress* _addr)
{
    struct sockaddr_in addr = _addr->getAddr();
    socklen_t addr_len  = _addr->getAddr_len();
    errif(::bind(fd_, (sockaddr*)&addr, addr_len), "socket bind error");
    _addr->setInetAddr(addr, addr_len);
}

void Socket::listen(){
    errif(::listen(fd_, SOMAXCONN) == -1, "socket listen error");
}

void Socket::setnonblocking()
{
    fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* _addr)
{   
    struct sockaddr_in addr;
    socklen_t addr_len  = sizeof(addr);
    bzero(&addr, sizeof(addr));
    int clnt_sockfd = ::accept(fd_, (sockaddr*)&addr, &addr_len);
    errif(clnt_sockfd == -1, "socket accept error");
    _addr->setInetAddr(addr, addr_len);
    return clnt_sockfd;
}

int Socket::getFd()
{
    return fd_;
}
