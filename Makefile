# #可执行文件
TARGET = server client
CXXFLAGS :=  -w -I./inc
LDFLAGS := -lpthread 

# 定义编译器
CXX = g++ -std=c++11 -g

# 定义源文件和目标文件
SERVER_SRC = socket_server.cpp
SERVER_SRC += $(wildcard src/*.cpp )

CLIENT_SRC = socket_client.cpp
CLIENT_SRC += $(wildcard src/*.cpp )

# 默认目标
all: $(TARGET)

server: $(SERVER_SRC)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) 

client: $(CLIENT_SRC)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) 
	
# 清理生成的文件
clean:
	rm -f  $(TARGET) 

.PHONY: all clean

