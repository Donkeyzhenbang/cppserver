#include "Acceptor.h"

#include <cstdio>

#include "Channel.h"
#include "InetAddress.h"
#include "Server.h"
#include "Socket.h"

#define NET_IP "127.0.0.1"
#define NET_PORT 8888

Acceptor::Acceptor(EventLoop *loop) : loop_(loop), sock_(nullptr), acceptChannel_(nullptr) {
  sock_ = new Socket();
  InetAddress *addr = new InetAddress(NET_IP, NET_PORT);
  sock_->bind(addr);
  sock_->listen();
  // sock_->setnonblocking(); /acceptor使用阻塞式IO比较好
  acceptChannel_ = new Channel(loop_, sock_->getFd());
  //!预先绑定回调函数，服务器每当有一个连接到来，执行newConnection函数
  std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
  acceptChannel_->setReadCallback(cb);
  acceptChannel_->enableReading();
  // acceptChannel_->setUseThreadPool(false);
  delete addr;
}

Acceptor::~Acceptor() {
  delete sock_;
  delete acceptChannel_;
}

void Acceptor::acceptConnection() {
  InetAddress *clnt_addr = new InetAddress();
  Socket *clnt_sock = new Socket(sock_->accept(clnt_addr));
  printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->getAddr().sin_addr),
         ntohs(clnt_addr->getAddr().sin_port));
  clnt_sock->setnonblocking();
  /*服务器只有一个Acceptor，每个Acceptor和Connection都有各自的Channel绑定回调函数
  当Acceptor实现构造函数之后，其Channel会绑定好acceptConnection函数，当客户端连接上之后，会先执行
  Acceptor预先绑定好的回调函数acceptConnection后，再执行在Server类中使用构造函数绑定的
  newConnectionCallback函数*/
  newConnectionCallback(clnt_sock);
  delete clnt_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket *)> cb) { newConnectionCallback = cb; }