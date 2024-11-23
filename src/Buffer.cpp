#include <iostream>
#include <cstring>
#include <unistd.h>
#include "Buffer.h"

Buffer::Buffer()
{

}

Buffer::~Buffer()
{

}

void Buffer::append(const char* str, int size)
{
    for(int i = 0; i < size; i ++){
        if(str[i] == '\0') break;
        buf.push_back(str[i]); //!buf定义为string 也可以使用std::vector
    }
}

ssize_t Buffer::size()
{
    return buf.size();
}

const char* Buffer::c_str()
{
    return buf.c_str();
}

void Buffer::clear()
{
    buf.clear();
}

void Buffer::getline()
{
    buf.clear();
    std::getline(std::cin, buf);// 从标准输入读取一整行数据，并存储到 buf
}