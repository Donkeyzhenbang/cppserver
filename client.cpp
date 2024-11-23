#include <iostream>
#include <unistd.h>
#include <cstring>
#include "src/utils.h"
#include "src/Buffer.h"
#include "src/InetAddress.h"
#include "src/Socket.h"

#define NET_IP "127.0.0.1"
#define NET_PORT 8888

int main()
{
    Socket* sock = new Socket();
    InetAddress* addr = new InetAddress(NET_IP, NET_PORT);
    sock->connect(addr);

    int sockfd = sock->getFd();

    Buffer *sendBuffer = new Buffer();
    Buffer *readBuffer = new Buffer();


    while(true){
        sendBuffer->getline();
        ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
        if(write_bytes == -1){
            std::cout << "socket already disconnected, can't write any more \n";
            break;
        }
        int already_read = 0;
        char buf[1024];
        while(true){
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if(read_bytes > 0){
                readBuffer->append(buf, read_bytes);
                already_read += read_bytes;
            } else if(read_bytes == 0){         //EOF
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            }
            if(already_read >= sendBuffer->size()){
                printf("message from server: %s\n", readBuffer->c_str());
                break;
            } 
        }
        readBuffer->clear();
    }
    delete sock;
    delete addr;
    return 0;
}