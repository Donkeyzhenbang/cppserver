#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "utils.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Epoll.h"

#define READ_BUFFER 1024

void read_event_handler(int fd)
{
    char buf[READ_BUFFER];
    while(true){
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(fd, buf, sizeof(buf));
        if(bytes_read > 0){
            printf("message from client fd %d : %s \n", fd, buf);
            write(fd, buf, sizeof(buf));
        }else if(bytes_read == -1 && errno == EINTR){ //read调用信号中断客户端正常中断 继续读取
            printf("continue reading \n");
            continue;
        }else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//如果 read 调用失败并且 errno 设置为 EAGAIN 或 EWOULDBLOCK，这表示缓冲区中没有更多数据可读（在非阻塞模式下）
            printf("finish reading once, errno: %d \n", errno);
            break;
        }else if(bytes_read == 0){//EOF 客户端断开连接
            printf("EOF, client fd %d disconnected \n", fd);
            close(fd);
            break;
        }
    }
}

/**
 * @brief ev用于将服务器fd/客户端sockfd添加到epoll实例中 events用于接收epoll_wait返回的事件集合
 * int epoll_wait (int __epfd, struct epoll_event *__events, int __maxevents, int __timeout);
 * */
int main()
{
    //创建Socket
    Socket* serv_sock = new Socket();
    //初始化服务器sockaddr_in结构体
    InetAddress* serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    Epoll* ep = new Epoll(); 
    serv_sock->set_nonblocking();
    ep->add_fd(serv_sock->get_sockfd(), EPOLLIN | EPOLLET);
    while(1){
        std::vector<epoll_event> event = ep->poll();
        int nfds = event.size();
        for(int i = 0; i < nfds; i ++){
            if(event[i].data.fd == serv_sock->get_sockfd()){ //新的客户端连接
                InetAddress* client_addr = new InetAddress();
                Socket* client_sock = new Socket(serv_sock->accept(client_addr));
                printf("new client fd %d IP:%s Port: %d \n",client_sock->get_sockfd(), inet_ntoa(client_addr->get_addr()->sin_addr), ntohs(client_addr->get_addr()->sin_port));
                client_sock->set_nonblocking();
                ep->add_fd(client_sock->get_sockfd(), EPOLLIN | EPOLLET);
            }else if(event[i].events & EPOLLIN){//不是服务器fd发生可读事件 表示客户端发来消息
                read_event_handler(event[i].data.fd);
            }else{
                printf("something else happened \n");
            }
        }

    }
    delete serv_sock;
    delete serv_addr;
    return 0;
    
}