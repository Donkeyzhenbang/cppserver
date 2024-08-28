## day01

### 注意事项
- 在使用 accept 和 connect 之前，需要确保套接字已经正确创建并配置（例如，对于 accept，需要先绑定（bind）和监听（listen）套接字；对于 connect，需要先创建套接字并指定服务器的地址）。


### bzero 
- 可用于初始化结构体 入口参数是addr和size 

### socklen_t
- 用于表示套接字地址结构的长度
- 传递地址长度：在调用如 bind、connect、accept、getsockname、getpeername 等函数时，socklen_t 用于指定套接字地址结构的长度，或者用于接收函数返回的实际长度。
- 确保类型安全：由于历史原因，一些旧的网络函数可能期望地址长度参数是一个特定类型的指针（如 int *），而不是 socklen_t *。使用 socklen_t 可以帮助确保类型的正确性和兼容性。
- 避免溢出：由于 socklen_t 是无符号类型，它可以防止地址长度的溢出，这在处理来自不受信任来源的数据时尤其重要。

### bind函数原型
```cpp
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
参数说明：

- sockfd：套接字文件描述符，这个套接字应该是之前已经创建的。
- addr：指向 sockaddr 结构的指针，该结构包含了套接字要绑定的地址信息。对于 IPv4，这通常是 struct sockaddr_in 结构；对于 IPv6，则是 struct sockaddr_in6 结构。
- addrlen：addr 指向的地址结构的大小。

返回值：

- 成功时，bind 函数返回 0。
- 失败时，返回 -1 并设置全局变量 errno 以指示错误原因。

### listen函数原型
```cpp
int listen(int sockfd, int backlog)
```
参数说明：
- sockfd：套接字文件描述符，这个套接字应该是之前已经创建并且绑定到一个地址上的。
- backlog：这个参数指定了内核应该为相应套接字排队的最大连接个数。这个数值不能超过系统特定的上限，通常在 /proc/sys/net/core/somaxconn 文件中定义。

返回值：
- 成功时，listen 函数返回 0。
- 失败时，返回 -1 并设置全局变量 errno 以指示错误原因。

### accept函数原型
```cpp
int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
```
参数说明：
- sockfd：监听套接字的文件描述符。
- addr：（可选）指向 sockaddr 结构的指针，用于存储连接客户端的地址信息。如果不需要客户端地址，可以设置为 NULL。
- addrlen：（可选）指向 socklen_t 类型变量的指针，该变量在调用 accept 时应该包含 addr 指向的缓冲区的大小。accept 成功返回后，该变量会包含实际客户端地址的大小。如果不需要客户端地址，可以设置为 NULL。
- accept函数会阻塞当前程序，直至有一个客户端socket被接受程序才继续往下进行

### connect函数原型
```cpp
int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
```
参数说明：
- sockfd：客户端的套接字文件描述符。
- addr：指向 sockaddr 结构的指针，该结构包含了服务器端套接字的地址信息。
- addrlen：addr 指向的 sockaddr 结构的大小。
- connect函数也是阻塞调用 连接成功/失败/超时

返回值：
- accept 和 connect 函数在成功时返回非负值。
- accept 返回一个新的文件描述符，用于与客户端通信的已连接套接字。
- connect 返回 0 表示连接成功。
- 在失败时，这两个函数都会返回 -1，并设置全局变量 errno 以指示错误的具体原因。

## day02
### 注意事项
- printf输出到屏幕记得加\n 否则会暂存在缓冲区
- inet_ntop 和 inet_pton 函数是线程安全的。
- read 和 write 函数通常用于文件 I/O，但也可以用于套接字 I/O，尤其是在使用 Unix 域套接字时。
recv send函数专门用于网络套接字，它提供了一些特定于网络操作的标志，如 MSG_DONTWAIT 来执行非阻塞接收。

### size_t
- size_t 是 C 语言标准库中定义的一个数据类型，它是一个无符号整数类型，通常用于表示对象的大小或数组中的元素数量。在内存分配、数组索引和其他需要表示内存大小或对象数量的上下文中，size_t 是一个常用的类型。它的定义通常在头文件 <stddef.h> 或 <sys/types.h> 中。
- 可用于内存分配，数组大小，字符串长度，I/O操作
```cpp
//内存操作
size_t num_bytes = 1024;
void* buf = malloc(num_types)
//I/O操作
size_t bytes_to_read = 512;
ssize_t bytes_read = read(file_descriptor, buffer, bytes_to_read);
```

### ssize_t 
参数说明：
- ssize_t 通常在 <sys/types.h> 头文件中定义
- 文件读写：如 read 和 write 系统调用，它们返回从文件中读取或写入文件的字节数，或者在出错时返回 -1。

- 字符串和内存操作：如 strlen 函数，它返回字符串的长度，使用 ssize_t 类型可以确保即使是非常长的字符串也能正确返回其长度。

- 网络编程：如 send 和 recv 函数，它们返回成功发送或接收的字节数，或者在出错时返回 -1。

ssize_t和size_t区别：

1.有符号与无符号：

- size_t 是一个无符号整数类型，用于表示对象的大小或者数组中元素的数量。由于它不能表示负值，所以它只能用于表示非负的内存大小或计数。
- ssize_t 是一个有符号整数类型，通常用于表示可以失败的系统调用的返回值，比如文件的读写操作。它可以表示负值，用于表示错误或者特定的状态。

2.用途：

- size_t 通常用于内存分配（如 malloc、calloc 等函数的返回值），数组索引，以及任何需要表示大小或计数的上下文。
- ssize_t 通常用于系统调用和库函数的返回值，如 read、write、send、recv 等，这些函数可能会失败并返回错误码（负值）。

### read
read函数用于从文件描述符读取数据，它也可以用于从套接字读取数据。阻塞函数，直到有数据可读
```cpp
#include <unistd.h>
ssize_t read(int fd, void* buf, size_t count);
```
参数说明：
- fd：文件描述符，可以是任何类型的文件、设备或套接字。
- buf：指向缓冲区的指针，用于存储从文件描述符读取的数据。
- count：要读取的最大字节数。
返回值：
- read 成功时返回读取的字节数，出错时返回 -1。
- 如果到达文件或套接字的末尾（EOF），read 函数返回 0

### write
write 函数用于将数据写入文件描述符，它也可以用于将数据发送到套接字。
```cpp
#include <sys/socket.h>
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```
参数说明：
- fd：文件描述符，可以是任何类型的文件、设备或套接字。
- buf：指向缓冲区的指针，包含要写入文件描述符的数据。
- count：要写入的最大字节数。
返回值：
- write 成功时返回写入的字节数，出错时返回 -1。

### recv
recv 函数是 BSD 套接字 API 的一部分，专门用于从套接字接收数据。
```cpp
#include <sys/socket.h>
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```
参数说明：
- sockfd：套接字文件描述符。
- buf：指向缓冲区的指针，用于存储从套接字接收的数据。
- len：要接收的最大字节数。
- flags：指定接收操作的行为，通常设置为 0。可以设置 MSG_OOB 来接收带外数据，MSG_PEEK 来窥视数据但不从队列中移除，MSG_WAITALL 来等待所有数据到达等。
返回值：
- recv 成功时返回读取的字节数，出错时返回 -1。

### send 
send 函数用于向与套接字相关联的对等点发送数据。
```cpp
ssize_t send(int sockfd, const void* buf, size_t len, int flags)
```
参数说明：
- sockfd：套接字文件描述符。
- buf：指向要发送数据的缓冲区的指针。
- len：要发送的数据的长度。
- flags：指定发送操作的行为，通常设置为 0。

返回值：
- send 成功时返回发送的字节数，出错时返回 -1。

### inet_ntop
inet_ntop 函数用于将网络地址从二进制形式转换为文本形式的 IP 地址。
- n 代表 "number"，这里指的是网络地址的数值形式，通常是二进制形式的网络地址。
- p 代表 "presentation"，这里指的是网络地址的表示形式，即人类可读的格式，如 IPv4 或 IPv6 的点分十进制表示。
```cpp
#include <arpa/inet.h>
const char* inet_ntop(int af, const void* src, char* dst, socklen_t size);
```
参数说明：
- af：地址族，例如 AF_INET 或 AF_INET6。
- src：指向二进制网络地址的指针。
- dst：指向用于存储转换后的文本地址的缓冲区的指针。
- size：dst 缓冲区的大小。

返回值：
- inet_ntop 返回一个指向转换后的文本地址的指针，或者在出错时返回 NULL。

### inet_pton
inet_pton 函数用于将文本形式的 IP 地址转换为二进制形式的网络地址。
```cpp
#include <arpa/inet.h>
int inet_pton(int af, const char* src, void* dst);
```
参数说明：
- af：地址族，例如 AF_INET 或 AF_INET6。
- src：指向文本形式的 IP 地址字符串的指针。
- dst：指向用于存储转换后的二进制网络地址的缓冲区的指针。

返回值：
- inet_pton 成功时返回 1，出错时返回 -1，如果输入地址无效则返回 0。



## day03