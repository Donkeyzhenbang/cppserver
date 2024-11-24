server:
	g++ src/utils.cpp src/Buffer.cpp src/Socket.cpp src/InetAddress.cpp client.cpp -o client && \
	g++ server.cpp \
	-pthread \
	src/utils.cpp src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp src/Connection.cpp \
	src/Channel.cpp src/EventLoop.cpp src/Server.cpp src/Acceptor.cpp src/Buffer.cpp \
	src/ThreadPool.cpp \
	-o server && \
	g++ -pthread src/ThreadPool.cpp ThreadPoolTest.cpp -o threadpooltest

clean:
	rm -f server client threadpooltest
