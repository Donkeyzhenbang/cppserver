#include <cstdio>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"
#include "utils.h"

#define READ_BUFFER 1024

Connection::Connection(EventLoop* loop, Socket* sock) : loop_(loop), sock_(sock), channel_(nullptr), inBuffer(new std::string()), readBuffer(nullptr)
{
    channel_ = new Channel(loop_, sock_->getFd());
    channel_->enableReading();
    channel_->useET();
    std::function<void()> cb = std::bind(&Connection::echo, this, sock_->getFd());
    channel_->setReadCallback(cb);
    channel_->setUseThreadPool(false);
    readBuffer = new Buffer();
}

Connection::~Connection()
{
    delete readBuffer;
    delete channel_;
    delete sock_;
}

void Connection::echo(int sockfd){
    char buf[1024];
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            readBuffer->append(buf, bytes_read);
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            std::cout << "continue reading \n";
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            std::cout << "finish reading once\n";
            std::cout << "message from client fd " << sockfd << " data : " << readBuffer->c_str() << std::endl;
            // errif(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "socket write error");
            send(sockfd);
            readBuffer->clear();
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            std::cout << "EOF, client fd " << sockfd << "disconnected\n";
            // close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            deleteConnectionCallback(sockfd);//!注意这里会执行客户端断开连接的回调函数，会点函数注册在Server类中newConnection中
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> cb)
{
    deleteConnectionCallback = cb;
}

void Connection::send(int sockfd)
{
    char buf[readBuffer->size()];
    strcpy(buf, readBuffer->c_str());
    int data_size = readBuffer->size();
    int data_left = data_size;
    while(data_left > 0){
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
        if(bytes_write == -1 && errno == EAGAIN){
            break;
        }
        data_left -= bytes_write;
    }
}