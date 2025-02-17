#include "Socket.h"
#include <errno.h>
#include <fcntl.h>  //用于文件控制操作（如open）
#include <sys/socket.h>
#include <unistd.h>  //write read等系统调用
#include <cstring>
#include "InetAddress.h"
#include "utils.h"

Socket::Socket() : fd_(-1) {
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  errif(fd_ == -1, "socket create error");
}

Socket::Socket(int fd) : fd_(fd) { errif(fd_ == -1, "socket create error"); }

Socket::~Socket() {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}

void Socket::bind(InetAddress *_addr) {
  struct sockaddr_in addr = _addr->getAddr();
  socklen_t addr_len = _addr->getAddr_len();
  errif(::bind(fd_, (sockaddr *)&addr, addr_len), "socket bind error");
  _addr->setInetAddr(addr, addr_len);
}

void Socket::listen() { errif(::listen(fd_, SOMAXCONN) == -1, "socket listen error"); }

void Socket::setnonblocking() { fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK); }

int Socket::accept(InetAddress *_addr) {
  int clnt_sockfd = -1;
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  bzero(&addr, sizeof(addr));
  if (fcntl(fd_, F_GETFL) & O_NONBLOCK) {
    while (true) {
      clnt_sockfd = ::accept(fd_, (sockaddr *)&addr, &addr_len);
      if (clnt_sockfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
        continue;
      } else if (clnt_sockfd == -1) {
        errif(true, "socket accept error");
      } else {
        break;
      }
    }
  } else {
    clnt_sockfd = ::accept(fd_, (sockaddr *)&addr, &addr_len);
    errif(clnt_sockfd == -1, "socket accept error");
  }
  _addr->setInetAddr(addr, addr_len);
  return clnt_sockfd;
}

void Socket::connect(InetAddress *_addr) {
  struct sockaddr_in addr = _addr->getAddr();
  // socklen_t addr_len = _addr->getAddr_len();
  // errif(::connect(fd_, (sockaddr*)&addr, addr_len) == -1, "socket connect error");
  if (fcntl(fd_, F_GETFL) & O_NONBLOCK) {
    while (true) {
      int ret = ::connect(fd_, (sockaddr *)&addr, sizeof(addr));
      if (ret == 0) {
        break;
      } else if (ret == -1 && (errno == EINPROGRESS)) {
        continue;
        /* 连接非阻塞式sockfd建议的做法：
            The socket is nonblocking and the connection cannot be
          completed immediately.  (UNIX domain sockets failed with
          EAGAIN instead.)  It is possible to select(2) or poll(2)
          for completion by selecting the socket for writing.  After
          select(2) indicates writability, use getsockopt(2) to read
          the SO_ERROR option at level SOL_SOCKET to determine
          whether connect() completed successfully (SO_ERROR is
          zero) or unsuccessfully (SO_ERROR is one of the usual
          error codes listed here, explaining the reason for the
          failure).
          这里为了简单、不断连接直到连接完成，相当于阻塞式
        */
      } else if (ret == -1) {
        errif(true, "socket connect error");
      }
    }
  } else {
    errif(::connect(fd_, (sockaddr *)&addr, sizeof(addr)) == -1, "socket connect error");
  }
}

int Socket::getFd() { return fd_; }
