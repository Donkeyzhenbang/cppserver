server:
	g++ src/utils.cpp client.cpp -o client && \
	g++ src/utils.cpp server.cpp src/Epoll.cpp src/InetAddress.cpp \
	src/Socket.cpp src/Channel.cpp src/EventLoop.cpp src/Server.cpp src/Acceptor.cpp src/Connection.cpp \
	-o server
clean:
	rm server && rm client