#include <string.h>
#include <unistd.h>
#include <functional>
#include <iostream>
#include "../src/include/Buffer.h"
#include "../src/include/InetAddress.h"
#include "../src/include/Socket.h"
#include "../src/include/ThreadPool.h"
#include "../src/include/utils.h"

#define NET_IP "127.0.0.1"
#define NET_PORT 8888

using namespace std;

void oneClient(int msgs, int wait) {
  Socket *sock = new Socket();
  InetAddress *addr = new InetAddress(NET_IP, NET_PORT);
  sock->connect(addr);

  int sockfd = sock->getFd();

  Buffer *sendBuffer = new Buffer();
  Buffer *readBuffer = new Buffer();

  sleep(wait);
  int count = 0;
  while (count < msgs) {
    sendBuffer->setBuff("I'm client!");
    ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
    if (write_bytes == -1) {
      printf("socket already disconnected, can't write any more!\n");
      break;
    }
    int already_read = 0;
    char buf[1024];  //这个buf大小无所谓
    while (true) {
      bzero(&buf, sizeof(buf));
      ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
      if (read_bytes > 0) {
        readBuffer->append(buf, read_bytes);
        already_read += read_bytes;
      } else if (read_bytes == 0) {  // EOF
        printf("server disconnected!\n");
        exit(EXIT_SUCCESS);
      }
      if (already_read >= sendBuffer->size()) {
        printf("count: %d, message from server: %s\n", count++, readBuffer->c_str());
        break;
      }
    }
    readBuffer->clear();
  }

  delete addr;
  delete sock;
}

int main(int argc, char *argv[]) {
  int threads = 100;
  int msgs = 100;
  int wait = 0;
  int o;
  const char *optstring = "t:m:w:";
  while ((o = getopt(argc, argv, optstring)) != -1) {
    switch (o) {
      case 't':
        threads = stoi(optarg);
        break;
      case 'm':
        msgs = stoi(optarg);
        break;
      case 'w':
        wait = stoi(optarg);
        break;
      case '?':
        std::cout << "error optopt " << optopt << std::endl;
        std::cout << "error opterr " << opterr << std::endl;
        break;
    }
  }
  ThreadPool *pool = new ThreadPool(threads);
  std::function<void()> func = std::bind(oneClient, msgs, wait);
  for (int i = 0; i < threads; ++i) {
    pool->add(func);
  }
  delete pool;
  return 0;
}