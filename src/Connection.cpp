#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include "Connection.h"
#include "Socket.h"
#include "Channel.h"

#define READ_BUFFER 1024

Connection::Connection(EventLoop* loop, Socket* sock) : loop_(loop), sock_(sock), channel_(nullptr)
{
    channel_ = new Channel(loop_, sock_->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, sock_->getFd());
    channel_->setCallback(cb);
    channel_->enableReading();
}

Connection::~Connection()
{
    delete channel_;
    delete sock_;
}

void Connection::echo(int sockfd){
    char buf[READ_BUFFER];
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            // close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            deleteConnectionCallback(sock_);//!注意这里会执行客户端断开连接的回调函数，会点函数注册在Server类中newConnection中
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket*)> cb)
{
    deleteConnectionCallback = cb;
}