#include "InetAddress.h"

#include <cstring>

InetAddress::InetAddress() : addr_len_(sizeof(addr_)) { bzero(&addr_, sizeof(addr_)); }

InetAddress::InetAddress(const char *ip, uint16_t port) : addr_len_(sizeof(addr_)) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip);
    addr_.sin_port = htons(port);
    addr_len_ = sizeof(addr_);
}

InetAddress::~InetAddress() {}

void InetAddress::setInetAddr(sockaddr_in addr, socklen_t addr_len) {
    addr_ = addr;
    addr_len_ = addr_len;
}

sockaddr_in InetAddress::getAddr() { return addr_; }
socklen_t InetAddress::getAddr_len() { return addr_len_; }