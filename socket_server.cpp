#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "utils.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void set_non_blocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main()
{
    //创建socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    //初始化服务器sockaddr_in结构体
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    //bind
    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

    //listen
    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    //初始化客户端sockaddr_in结构体    
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr, sizeof(client_addr));

    //accept接受客户端连接 打印socket描述符 IP Port
    // int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
    // errif(client_sockfd == -1, "socket accept error");
    // printf("new client fd %d ! IP: %s Port : %d \n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");
    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));

    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    set_non_blocking(sockfd);
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    while(true){
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        errif(-1 == nfds, "epoll wait error");
        for(int i = 0; i < nfds; i ++){
            if(events[i].data.fd == sockfd){
                struct sockaddr_in client_addr;
                bzero(&client_addr, sizeof(client_addr));
                socklen_t client_addr_len = sizeof(client_addr);

                int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len); 
                errif(client_sockfd == -1, "socket accept error");
                printf("new client fd %d IP: %s Port: %d \n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = client_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                set_non_blocking(client_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockfd, &ev);
            }else if(events[i].events & EPOLLIN){
                char buf[READ_BUFFER];
                while(true){
                    printf("test \n");
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if(bytes_read > 0){
                        printf("message from client fd %d : %s \n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));
                    }else if(bytes_read == -1 && errno == EINTR){
                        printf("continue reading");
                        continue;
                    }else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){ //非阻塞IO
                        printf("finish reading once, errno : %d \n", errno);
                        break;    
                    }else if(bytes_read == 0){
                        printf("EOF, client fd %d disconnected \n", events[i].data.fd);
                        close(events[i].data.fd);
                        break;
                    }
                }            
            }
            else {
                printf("something else happened");
            }
        }
    }
    close(sockfd);
    return 0;
    
}