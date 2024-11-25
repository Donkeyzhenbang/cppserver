# server:
# 	g++ src/utils.cpp src/Buffer.cpp src/Socket.cpp src/InetAddress.cpp client.cpp -o client && \
# 	g++ server.cpp \
# 	-pthread \
# 	src/utils.cpp src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp src/Connection.cpp \
# 	src/Channel.cpp src/EventLoop.cpp src/Server.cpp src/Acceptor.cpp src/Buffer.cpp \
# 	src/ThreadPool.cpp \
# 	-o server && \
# 	g++ -pthread src/ThreadPool.cpp ThreadPoolTest.cpp -o threadpooltest

# clean:
# 	rm -f server client threadpooltest
# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -pthread

# Directories
SRC_DIR := src
BIN_DIR := bin

# Source files
SERVER_SRCS := $(SRC_DIR)/utils.cpp \
               $(SRC_DIR)/Epoll.cpp \
               $(SRC_DIR)/InetAddress.cpp \
               $(SRC_DIR)/Socket.cpp \
               $(SRC_DIR)/Connection.cpp \
               $(SRC_DIR)/Channel.cpp \
               $(SRC_DIR)/EventLoop.cpp \
               $(SRC_DIR)/Server.cpp \
               $(SRC_DIR)/Acceptor.cpp \
               $(SRC_DIR)/Buffer.cpp \
               $(SRC_DIR)/ThreadPool.cpp \
			   server.cpp

CLIENT_SRCS := $(SRC_DIR)/utils.cpp \
               $(SRC_DIR)/Buffer.cpp \
               $(SRC_DIR)/Socket.cpp \
               $(SRC_DIR)/InetAddress.cpp \
               client.cpp

THREADPOOL_TEST_SRCS := $(SRC_DIR)/ThreadPool.cpp \
                        ThreadPoolTest.cpp

# Output executables
SERVER_BIN := $(BIN_DIR)/server
CLIENT_BIN := $(BIN_DIR)/client
THREADPOOL_TEST_BIN := $(BIN_DIR)/threadpooltest

# Rules
all: $(SERVER_BIN) $(CLIENT_BIN) $(THREADPOOL_TEST_BIN)

# Server target
$(SERVER_BIN): $(SERVER_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Client target
$(CLIENT_BIN): $(CLIENT_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ThreadPool test target
$(THREADPOOL_TEST_BIN): $(THREADPOOL_TEST_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Clean up build artifacts
clean:
	rm -rf $(BIN_DIR)

# Phony targets
.PHONY: all clean
