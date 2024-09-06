#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include "Socket.h"
#include "InetAddress.h"
#include "utils.h"

Socket::Socket() : fd_(-1)
{
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd_ == -1, "Socket create error");
}

Socket::Socket(int fd) : fd_(fd)
{
    errif(fd_ == -1, "Socket create error");
}

Socket::~Socket()
{
    if(fd_ != -1){
        close(fd_);
        fd_ = -1;
    }
}

/**
 * @brief socket-bind
 * 
 * @param addr 
 */
void Socket::bind(InetAddress* addr)
{
    //::全局作用域bind函数
    errif(::bind(fd_, (sockaddr*)addr->get_addr(),  addr->get_addrlen()), "Socket bind error");
}

/**
 * @brief socket-listen
 * 
 */
void Socket::listen()
{
    errif(::listen(fd_, SOMAXCONN), "Socket listen error");
}

/**
 * @brief 设置socket连接非阻塞模式 获取当前fd文件状态标志条件|不改变其他标志位
 * 
 */
void Socket::set_nonblocking()
{
    fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK); 
}
/**
 * @brief 
 * 
 * @param addr 
 * @return int 
 */
int Socket::accept(InetAddress* addr)
{
    int client_sockfd = ::accept(fd_, (sockaddr*)addr->get_addr(), &(addr->get_addrlen()));
    errif(client_sockfd == -1, "Sockfd accept error");
    return client_sockfd;
}
/**
 * @brief Get the sockfd object
 * 
 * @return int 
 */
int Socket::get_sockfd()
{
    return fd_;
}