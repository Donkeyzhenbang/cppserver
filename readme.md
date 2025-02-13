## day01 Socket编程

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

## day02 错误处理
### 注意事项

- 注意send函数的MSG_DONTWAIT是非阻塞模式，缓冲区满了直接爆掉 使用write默认阻塞模式
- printf输出到屏幕记得加\n 否则会暂存在缓冲区
- inet_ntop 和 inet_pton 函数是线程安全的。
- read 和 write 函数通常用于文件 I/O，但也可以用于套接字 I/O，尤其是在使用 Unix 域套接字时。
- recv send函数专门用于网络套接字，它提供了一些特定于网络操作的标志，如 MSG_DONTWAIT 来执行非阻塞接收。

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



## day03 epoll与高并发
### 基本思想
- 在创建了服务器socket fd后，将这个fd添加到epoll，只要这个fd上发生可读事件，表示有一个新的客户端连接。然后accept这个客户端并将客户端的socket fd添加到epoll，epoll会监听客户端socket fd是否有事件发生，如果发生则处理事件。

![alt text](assets/info_day03_epoll.png)

### 注意事项
- IO多路复用与多线程有相似之处，但不是一个概念。IO复用时针对IO接口，多线程针对CPU。IO复用使用select，poll,epoll
- epoll默认采用LT触发模式，即水平触发，只要fd上有事件，就会一直通知内核。这样可以保证所有事件都得到处理、不容易丢失，但可能发生的大量重复通知也会影响epoll的性能。如使用ET模式，即边缘触法，fd从无事件到有事件的变化会通知内核一次，之后就不会再次通知内核。这种方式十分高效，可以大大提高支持的并发度，但程序逻辑必须一次性很好地处理该fd上的事件，编程比LT更繁琐。注意ET模式必须搭配非阻塞式socket使用。

### epoll_create / epoll_create1
> 用于创建一个新的epoll文件描述符
```cpp
int epoll_create(int size);
int epoll_create1(int flags);
```
参数说明：
- size：指示可以监控的文件描述符的最大数量（epoll_create）。
- flags：指定创建 epoll 实例时的一些选项（epoll_create1），如 EPOLL_CLOEXEC。

### epoll_ctl
> 用于添加/修改/删除epoll实例中的文件描述符
```cpp
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
```
- epfd：epoll 文件描述符。
- op：操作类型，可以是 EPOLL_CTL_ADD（添加）、EPOLL_CTL_MOD（修改）或 EPOLL_CTL_DEL（删除）。
- fd：要操作的文件描述符。
- event：指向 epoll_event 结构体的指针，指定了文件描述符的事件类型和回调函数。

### epoll_wait
> 用于等待epoll实例中的事件的发生
```cpp
int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);
```
- epfd：epoll 文件描述符。
- events：指向 epoll_event 结构体数组的指针，用于接收发生的事件。
- maxevents：events 数组的最大长度。
- timeout：等待事件的最长时间，单位为毫秒。如果设置为 -1，则无限等待。

### epoll_pwait
> 与epoll_wait类似，但增加了对信号的屏蔽
```cpp
int epoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout, const sigset_t *sigmask);
```
- epfd：epoll 文件描述符。
- events：指向 epoll_event 结构体数组的指针，用于接收发生的事件。
- maxevents：events 数组的最大长度。
- timeout：等待事件的最长时间，单位为毫秒。
- sigmask：指向信号集的指针，用于指定在等待期间应该被屏蔽的信号。

### epoll相关
#### epoll结构体
```cpp
typedef union epoll_data {
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;
struct epoll_event {
  uint32_t events;	/* Epoll events */
  epoll_data_t data;	/* User data variable */
} __EPOLL_PACKED;
```

#### 宏定义
- EPOLLIN 表示发生可读事件
- EPOLLET 表示ET触发即边沿触发

#### 边缘/水平触发

epoll ⽀持两种事件触发模式，分别是边缘触发（edge-triggered，ET）和⽔平触发（level-triggered，LT）。这两个术语还挺抽象的，其实它们的区别还是很好理解的。
- 使⽤边缘触发模式时，当被监控的 Socket 描述符上有可读事件发⽣时，服务器端只会从 epoll_wait
中苏醒⼀次，即使进程没有调⽤ read 函数从内核读取数据，也依然只苏醒⼀次，因此我们程序要保
证⼀次性将内核缓冲区的数据读取完；
- 使⽤⽔平触发模式时，当被监控的 Socket 上有可读事件发⽣时，服务器端不断地从 epoll_wait 中苏
醒，直到内核缓冲区数据被 read 函数读完才结束，⽬的是告诉我们有数据需要读取；

### 非阻塞I/O
ET边缘触发模式一般搭配非阻塞I/O使用,下面是设置服务器端sockfd使用非阻塞IO的函数
```cpp
void set_non_blocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}
```
- fcntl 是 Unix 和类 Unix 操作系统中的一个系统调用，用于操作文件描述符的控制选项。fcntl 可以用于多种目的，包括获取或设置文件描述符的各种属性，如文件锁定、文件描述符的非阻塞模式、文件描述符的异步 I/O 等。
- F_GETFL 获取文件状态标志
- F_SETFL 设置文件状态标志
- 使用`|`逻辑或运算符不改变其他文件标志位 

### 总结

- epfd 是通过调用 epoll_create1(0) 创建的 epoll 实例的文件描述符。
它代表了 epoll 的一个实例，用于注册要监听的文件描述符（FD），并查询发生的事件。

- ev 是一个 struct epoll_event 类型的变量，用于定义要注册到 epoll 实例中的事件类型和关联的数据。
在代码中，ev 被初始化并设置为监听 EPOLLIN（可读事件）和 EPOLLET（边缘触发模式）。
ev.data.fd 存储了要监听的文件描述符，而 ev.events 存储了事件类型。

- events 是一个 struct epoll_event 类型的数组，用于存储 epoll_wait 函数返回的事件。
当调用 epoll_wait 函数时，它会填充 events 数组，每个数组元素代表一个发生的事件，包括事件类型和触发事件的文件描述符。
在循环中，events 数组被用来遍历所有发生的事件，并根据事件类型执行相应的处理。

- epoll形象工作结构如下

![alt text](assets/info_day04_epoll_struct.png)

## day04 封装成类
### 基本思想
- 使用类重新封装函数，分为三个大类Epoll, Socket, InetAddress，在做设计时特意将类中变量都设置为private，不知道对后面功能扩充而言是否臃肿
- 要注意的是，ev用于将服务器fd/客户端sockfd添加到epoll实例中 events用于接收epoll_wait返回的事件集合，epoll_wait函数中有一个变量为struct epoll_event *__events，会将发生事件都保存在events中

### 可见性
- 在进行编写类的过程中，将变量设置为private来保证该变量在此类中为全局变量，可以任意调用，而对于其他不相关的类则无权进行访问。将变量设置为public则全局可以访问。对于private变量，现在的认知中，最好的处理办法是在类中重写set()与get()方法，通过public方法间接访问其他类中私有变量

### 注意事项
#### const
```cpp
const sockaddr_in* get_addr() const {return &addr_;}
```
- 函数后面的const表示这个成员函数不会修改任何成员变量，不改变对象状态，只读成员函数;
- 前置的const表示返回的指针指向的对象是一个常量，不能通过指针修改他指向对象



#### 左值右值
```cpp
int client_sockfd = ::accept(fd_, (sockaddr*)addr->get_addr(), &(addr->get_addrlen()));
socklen_t& get_addrlen() {return addrlen_;}
```
- 在后续调用get_addrlen(),需要对他的地址对应内存空间进行读写，如果这里不加`&`取地址符号，则为右值（比如临时变量或一个值），没有分配长久存在的内存空间，无法将相关信息保存在addrlen_中。所以要分配左值引用。
- 右值作为一个值可以被接收，比如函数返回值，当时不能对其取地址。在 C++ 中，对于基本数据类型的返回值（如 int），编译器通常会优化这个过程，使得返回值直接存储在寄存器中，而不是在内存中。

![alt text](assets/info_day04_system.png)

#### bug修复
- 在初次编写完此代码时，运行服务器时当有客户端连接一直有报错：`epoll wait error: Bad address` 原因是如图使用bzero时第一个变量前习惯性加`&`导致传入参数为二级指针

![alt text](assets/bug_day04_bzero.png)

## day05 epoll高级用法-channel
### 基本思想
- 将一个文件描述符添加到epoll红黑树，当该文件描述符上有事件发生时，拿到它、处理事件，这样我们每次只能拿到一个文件描述符，也就是一个int类型的整型值。试想，如果一个服务器同时提供不同的服务，如HTTP、FTP等，那么就算文件描述符上发生的事件都是可读事件，不同的连接类型也将决定不同的处理逻辑，仅仅通过一个文件描述符来区分显然会很麻烦，我们更加希望拿到关于这个文件描述符更多的信息
- epoll中的data其实是一个union类型，可以储存一个指针。而通过指针，理论上我们可以指向任何一个地址块的内容，可以是一个类的对象，这样就可以将一个文件描述符封装成一个Channel类，一个Channel类自始至终只负责一个文件描述符，对不同的服务、不同的事件类型，都可以在类中进行不同的处理，而不是仅仅拿到一个int类型的文件描述符。
- 每个文件描述符会被分发到一个Epoll类，用一个ep指针来指向。类中还有这个Channel负责的文件描述符。另外是两个事件变量，events表示希望监听这个文件描述符的哪些事件，因为不同事件的处理方式不一样。revents表示在epoll返回该Channel时文件描述符正在发生的事件。inEpoll表示当前Channel是否已经在epoll红黑树中，为了注册Channel的时候方便区分使用EPOLL_CTL_ADD还是EPOLL_CTL_MOD。
```cpp
typedef union epoll_data {
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;
struct epoll_event {
  uint32_t events;	/* Epoll events */
  epoll_data_t data;	/* User data variable */
} __EPOLL_PACKED;
```

### this
- cpp中this 关键字是一个指针，它指向当前对象的内存地址。它用于在类的成员函数中引用当前对象的成员变量和成员函数
```cpp
void Channel::enable_reading()
{
    events_ = EPOLLIN | EPOLLET;
    ep_->update_channel(this);
}
```
### channel
> 使用channel类替换单一fd文件描述符的原因
- 试想，如果一个服务器同时提供不同的服务，如HTTP、FTP等，那么就算文件描述符上发生的事件都是可读事件，不同的连接类型也将决定不同的处理逻辑，仅仅通过一个文件描述符来区分显然会很麻烦，我们更加希望拿到关于这个文件描述符更多的信息。
- 这个版本服务器内只有一个EventLoop，当其中有可读事件发生时，我们可以拿到该描述符对应的Channel。在新建Channel时，根据Channel描述符的不同分别绑定了两个回调函数，newConnection()函数被绑定到服务器socket上，handlrReadEvent()被绑定到新接受的客户端socket上。这样如果服务器socket有可读事件，Channel里的handleEvent()函数实际上会调用Server类的newConnection()新建连接。如果客户端socket有可读事件，Channel里的handleEvent()函数实际上会调用Server类的handlerReadEvent()响应客户端请求。

```cpp
std::vector<Channel*> Epoll::poll(int timeout)
{
    std::vector<Channel*> active_events;
    int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; i ++){
        Channel* ch = (Channel*)events_[i].data.ptr;
        ch->set_revents(events_[i].events);
        active_events.push_back(ch);
    }
    return active_events;
} 

typedef union epoll_data
{
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;

```
- 这里核心是`Channel* ch = (Channel*)events_[i].data.ptr;`这样我们就将一个文件描述符替换指向为一个Channel类
- 由于epoll_data是union联合体，其中内存分布如图所示，整个联合体在ubuntu20.04中是8字节，由最大的数据类型决定，这里是void*和u64决定
- 联合体对应的内存中存储的数据是确定的，只不过编译器会根据代码将其解释为不同的类型对其进行解析，比如u32和int这里都取了4位，注意小段对齐
- `printf("int: %d, void* : %d \n", sizeof(int), sizeof(void*));`, 这里结果是`int: 4, void* : 8` 记得加\n否则会留在缓冲区不显示

![alt text](assets/info_day05_epoll_data.png)

## day06 服务器与事件驱动核心
### 基本思想
- 我们为每一个添加到epoll的文件描述符都添加了一个Channel，用户可以自由注册各种事件、很方便地根据不同事件类型设置不同回调函数（在当前的源代码中只支持了目前所需的可读事件，将在之后逐渐进行完善）

类之间的关系图

![alt text](assets/info_day06_class_uml.png)

![alt text](assets/info_day06_class_plant_uml.png)

## day07 为我们的服务器添加一个Acceptor
### 基本思想
- 我们之前接受socket连接都是在Server类中实现的，可以分离接受连接这一模块，添加一个`Acceptor`类，使得代码更加抽象
- 这样一来，新建连接的逻辑就在Acceptor类中。但逻辑上新socket建立后就和之前监听的服务器socket没有任何关系了，TCP连接和Acceptor一样，拥有以上提到的三个特点，这两个类之间应该是平行关系。所以新的TCP连接应该由Server类来创建并管理生命周期，而不是Acceptor。并且将这一部分代码放在Server类里也并没有打破服务器的通用性，因为对于所有的服务，都要使用Acceptor来建立连接。

为实现上述内容，有以下两种方式：
- 使用传统的虚类、虚函数来设计一个接口
- C++11的特性：std::function、std::bind、右值引用、std::move等实现函数回调

虚函数使用后续补充，这里使用std:function来实现，在QT程序中也使用过同样的策略，绑定回调函数

## day08 一切皆是类，连TCP连接也不例外
### 基本思想
- 本章重写了`Socket`类与`InetAddress`类，将之前`public`成员改为`private`，新增set()与get()方法
- 今天的抽象过程中，将客户端连接都抽象为`Connection`类，客户端存储在`Server`类中的`map`，由`Server`管理，每一个类的实例都通过一个独有的`channel`负责分发到`epoll`，该`Channel`的事件处理函数`handleEvent()`会调用Connection中的事件处理函数来响应客户端请求。`Connection`处理事件的逻辑应该由`Connection`类本身来完成
- 同时，昨天将`Acceptor`类的处理事件函数（也就是新建连接功能）被放到了`Server`类中，今天将客户端建立连接这一过程放到了Acceptor的方法中，建立连接后执行回调函数还是在Server类中的`newConnection`，具体内容为设定取消连接的回调函数并将已建立连接存入map中
- 对于断开TCP连接操作，也就是销毁一个`Connection`类的实例。由于`Connection`的生命周期由`Server`进行管理，所以也应该由`Server`来删除连接。如果在`Connection`业务中需要断开连接操作，也应该和之前一样使用回调函数来实现，在`Server`新建每一个连接时绑定删除该连接的回调函数

## day09 缓冲区-大作用
### 基本思想
- 非阻塞式socket IO的读取，可以看到使用的读缓冲区大小为1024，每次从TCP缓冲区读取1024大小的数据到读缓冲区，然后发送给客户端。这是最底层C语言的编码，在逻辑上有很多不合适的地方。比如我们不知道客户端信息的真正大小是多少，只能以1024的读缓冲区去读TCP缓冲区（就算TCP缓冲区的数据没有1024，也会把后面的用空值补满）；也不能一次性读取所有客户端数据，再统一发给客户端。
- 以封装一个缓冲区是很有必要的，为每一个Connection类分配一个读缓冲区和写缓冲区，从客户端读取来的数据都存放在读缓冲区里，这样Connection类就不再直接使用char buf[]这种最笨的缓冲区来处理读写操作。
- 这个缓冲区类使用std::string来储存数据，也可以使用std::vector<char>。为每一个TCP连接分配一个读缓冲区后，就可以把客户端的信息读取到这个缓冲区内，缓冲区大小就是客户端发送的报文真实大小
- 在这里依然有一个char buf[]缓冲区，用于系统调用read()的读取，这个缓冲区大小无所谓，但太大或太小都可能对性能有影响（太小读取次数增多，太大资源浪费、单次读取速度慢），设置为1到设备TCP缓冲区的大小都可以。以上代码会把socket IO上的可读数据全部读取到缓冲区，缓冲区大小就等于客户端发送的数据大小。全部读取完成之后，可以构造一个写缓冲区、填好数据发送给客户端。由于是echo服务器，所以这里使用了相同的缓冲区。 

> 下面是之前使用channel操作epoll的优势：是channel相比于传统的单个fd

![alt text](assets/info_day09_channel-fd00.png)

![alt text](assets/info_day09_channel-fd01.png)

![alt text](assets/info_day09_channel-fd02.png)

![alt text](assets/info_day09_channel-fd03.png)

![alt text](assets/info_day09_port_reuse.png)

## day10 加入线程池到服务器
### 基本思想


#### 默认参数处理

![alt text](assets/info_day10_default_para00.png)

![alt text](assets/info_day10_default_para01.png)

![alt text](assets/info_day10_default_para02.png)


#### 大括号多种用法

![alt text](assets/info_day10_brace00.png)

![alt text](assets/info_day10_brace01.png)

![alt text](assets/info_day10_brace02.png)

![alt text](assets/info_day10_brace03.png)

![alt text](assets/info_day10_brace04.png)

![alt text](assets/info_day10_brace05.png)

![alt text](assets/info_day10_brace06.png)

#### emplace && emplace_back

- 使用push_back() 会调用一次拷贝构造函数
- 使用 emplace_back，直接在容器内构造对象，不调用拷贝构造函数
- 使用 queue 的 emplace，不调用拷贝或移动构造函数

![alt text](assets/info_day10_emplace_back.png)

![alt text](assets/info_day10_emplace.png)

#### mutex
- uniqie_lock 在使用上比 lock_quard 灵活，但代价就是效率会低一点，并且内存占用量也会相对高一些。
- 保护是通过对 tasks_ 的每一次访问（无论是读还是写）都加锁实现的。互斥保护的定义:保护任务队列的核心逻辑是 在对 tasks_ 进行操作时，必须持有 tasks_mtx_ 的锁，即加锁后才能访问 tasks_。这通过 RAII 风格的 std::unique_lock 实现.
- 在访问 tasks_ 之前，通过互斥锁 tasks_mtx_ 确保当前线程独占对任务队列的访问权限。
- 当锁被持有时，其他线程试图访问 tasks_ 会被阻塞。

![alt text](assets/info_day10_mutex.png)

#### thread

![alt text](assets/info_day10_thread_join.png)

![alt text](assets/info_day10_thread_join_detach.png)

![alt text](assets/info_day10_thread_joinable.png)

#### threadpool

![alt text](assets/info_day10_threadpool00.png)

![alt text](assets/info_day10_threadpool01.png)

![alt text](assets/info_day10_threadpool02.png)

## day11 完善线程池
### 基本思想

#### Reactor && Proactor

- 单Reactor模型既负责建立连接，又负责监听，面对瞬间的高并发会有性能瓶颈
- 多Reactor模型main-reactor只负责由acceptor建立连接，建立连接后将其分发给sub-reactor，由sub-reactor负责监听

![alt text](assets/info_day11_reactor00.png)

![alt text](assets/info_day11_reactor01.png)

![alt text](assets/info_day11_reactor02.png)

![alt text](assets/info_day11_proactor.png)

![alt text](assets/info_day11_async.png)

#### 左值引用与右值引用

![alt text](assets/info_day11_lvalue_rvalue_reference.png)

![alt text](assets/info_day11_universal_reference.png)

![alt text](assets/info_day11_reference_collapse.png)

#### C++模板

![alt text](assets/info_day11_inline_constexpr.png)

![alt text](assets/info_day11_template00.png)

![alt text](assets/info_day11_template01.png)

![alt text](assets/info_day11_std_future_resultof.png)

![alt text](assets/info_day11_packaged_task.png)

![alt text](assets/info_day11_get_future.png)

![alt text](assets/info_day11_command_line_argument_parsing.png)

![alt text](assets/info_day11_task_get_future.png)