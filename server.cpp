// =================day07=========================
#include "src/EventLoop.h"
#include "src/Server.h"


int main() {
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    loop->loop();
    delete loop;
    delete server;
    return 0;
}

// =================day06=========================
// #include "src/EventLoop.h"
// #include "src/Server.h"


// int main() {
//     EventLoop *loop = new EventLoop();
//     Server *server = new Server(loop);
//     loop->loop();
//     delete server;
//     delete loop;
//     return 0;
// }


//=================day05=========================
// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <vector>
// #include "./src/utils.h"
// #include "./src/Epoll.h"
// #include "./src/InetAddress.h"
// #include "./src/Socket.h"
// #include "./src/Channel.h"



// #define MAX_EVENTS 1024
// #define READ_BUFFER 1024
// #define NET_IP "127.0.0.1"
// #define NET_PORT 8888

// void setnonblocking(int fd){
//     fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
// }
// void handleReadEvent(int);

// int main() {
//     Socket *serv_sock = new Socket();
//     InetAddress *serv_addr = new InetAddress(NET_IP, NET_PORT);
//     serv_sock->bind(serv_addr);
//     serv_sock->listen();    
//     Epoll *ep = new Epoll();
//     serv_sock->setnonblocking();
//     Channel *servChannel = new Channel(ep, serv_sock->getFd());
//     servChannel->enableReading();
//     while(true){
//         std::vector<Channel*> activeChannels = ep->poll();//!epoll_wait
//         int nfds = activeChannels.size();
//         for(int i = 0; i < nfds; ++i){
//             int chfd = activeChannels[i]->getFd();
//             if(chfd == serv_sock->getFd()){        //新客户端连接
//                 InetAddress *clnt_addr = new InetAddress();      //会发生内存泄露！没有delete
//                 Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));       //会发生内存泄露！没有delete
//                 printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
//                 clnt_sock->setnonblocking();
//                 Channel *clntChannel = new Channel(ep, clnt_sock->getFd());
//                 clntChannel->enableReading();//!epoll_ctl
//             } else if(activeChannels[i]->getRevents() & EPOLLIN){      //可读事件
//                 handleReadEvent(activeChannels[i]->getFd());
//             } else{         //其他事件，之后的版本实现
//                 printf("something else happened\n");
//             }
//         }
//     }
//     delete serv_sock;
//     delete serv_addr;
//     return 0;
// }

// void handleReadEvent(int sockfd){
//     char buf[READ_BUFFER];
//     while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
//         bzero(&buf, sizeof(buf));
//         ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
//         if(bytes_read > 0){
//             printf("message from client fd %d: %s\n", sockfd, buf);
//             write(sockfd, buf, sizeof(buf));
//         } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
//             printf("continue reading");
//             continue;
//         } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
//             printf("finish reading once, errno: %d\n", errno);
//             break;
//         } else if(bytes_read == 0){  //EOF，客户端断开连接
//             printf("EOF, client fd %d disconnected\n", sockfd);
//             close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
//             break;
//         }
//     }
// }

//=================day04=========================
// #include <iostream>
// #include <cstdio>
// #include <cstring>
// #include <unistd.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <vector>
// #include "./src/utils.h"
// #include "./src/Epoll.h"
// #include "./src/InetAddress.h"
// #include "./src/Socket.h"
// #define MAX_EVENTS 1024
// #define READ_BUFFER 1024
// #define NET_IP "127.0.0.1"
// #define NET_PORT 8888
// void setnonblocking(int fd){
//     fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
// }
// void handleReadEvent(int);
// int main()
// {
//     Socket* serv_sock = new Socket();
//     InetAddress* serv_addr = new InetAddress(NET_IP, NET_PORT);
//     serv_sock->bind(serv_addr);
//     serv_sock->listen();
//     Epoll* ep = new Epoll();
//     serv_sock->setnonblocking();
//     ep->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);
//     std::cout << "服务器启动 \n";
//     while(true){
//         std::vector<epoll_event> events = ep->event_poll(); //!在此执行epoll_wait
//         int nfds = events.size();
//         for(int i = 0; i < nfds; i ++){
//             if(events[i].data.fd == serv_sock->getFd()){ //新客户端连接
//                 InetAddress* clnt_addr = new InetAddress();
//                 Socket* clnt_sock = new Socket(serv_sock->accept(clnt_addr));
//                 std::cout << "new client fd " << clnt_sock->getFd() << " IP " << inet_ntoa(clnt_addr->addr.sin_addr) << " Port " << ntohs(clnt_addr->addr.sin_port) << std::endl;
//                 clnt_sock->setnonblocking();
//                 ep->addFd(clnt_sock->getFd(), EPOLLIN | EPOLLET);//epoll ctl add
//             }else if(events[i].events & EPOLLIN){ //发生可读事件
//                 handleReadEvent(events[i].data.fd);
//             }else{
//                 std::cout << "something else happened \n";
//             }
//         }
//     }
//     delete serv_sock;
//     delete serv_addr;
//     return 0;
// }
// void handleReadEvent(int sockfd){
//     char buf[READ_BUFFER];
//     while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
//         bzero(&buf, sizeof(buf));
//         ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
//         if(bytes_read > 0){
//             printf("message from client fd %d: %s\n", sockfd, buf);
//             write(sockfd, buf, sizeof(buf));
//         } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
//             printf("continue reading");
//             continue;
//         } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
//             printf("finish reading once, errno: %d\n", errno);
//             break;
//         } else if(bytes_read == 0){  //EOF，客户端断开连接 
//             printf("EOF, client fd %d disconnected\n", sockfd);
//             close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
//             break;
//         }
//     }
// }