#include "Buffer.h"

#include <unistd.h>

#include <cstring>
#include <iostream>

Buffer::Buffer() {}

Buffer::~Buffer() {}

void Buffer::append(const char *str, int size) {
  for (int i = 0; i < size; i++) {
    if (str[i] == '\0') break;
    buf_.push_back(str[i]);  //! buf定义为string 也可以使用std::vector
  }
}

ssize_t Buffer::size() { return buf_.size(); }

const char *Buffer::c_str() { return buf_.c_str(); }

void Buffer::clear() { buf_.clear(); }

void Buffer::getline() {
  buf_.clear();
  std::getline(std::cin, buf_);  // 从标准输入读取一整行数据，并存储到 buf
}

void Buffer::setBuff(const char *buf) {
  buf_.clear();
  buf_.append(buf);
}