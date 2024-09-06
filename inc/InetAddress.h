#pragma once
#include <arpa/inet.h>

/**
 * @brief 注意这里如果将addr addrlen设置为私有变量，后续在Socket.cpp中bind的时候
 *会访问InetAddress类中私有变量导致编译错误
 */
class InetAddress{
private:
    struct sockaddr_in addr_;
    socklen_t addrlen_;   
public:
    InetAddress();
    InetAddress(const char* ip, uint16_t port);
    ~InetAddress();
    const sockaddr_in* get_addr() const {return &addr_;}
    socklen_t& get_addrlen() {return addrlen_;}
    //!注意此处必须加入&符号 没加&为右值临时变量不能被取地址
    //!函数后面的const表示这个成员函数不会修改任何成员变量，不改变对象状态，只读成员函数;前置的const表示返回的指针指向的对象是一个常量，不能通过指针修改他指向对象
};