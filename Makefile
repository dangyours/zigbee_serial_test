# 编译器设置
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lserial

# 目标可执行文件
TARGET = single_communication serial_master serial_slave serial_master_all serial_slave_all serial_at

# 源文件（自动扫描当前目录的 .cpp 文件）
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# 默认构建规则
all: $(TARGET)

# $(TARGET): $(OBJS)
# 	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
serial_master: serial_master.o
	g++ -std=c++11 -Wall $^ -o $@ -L/usr/local/lib -lserial

single_communication: single_communication.o
	g++ -std=c++11 -Wall $^ -o $@ -L/usr/local/lib -lserial

serial_slave: serial_slave.o
	g++ -std=c++11 -Wall $^ -o $@ -L/usr/local/lib -lserial

serial_master_all: serial_master_all.o
	g++ -std=c++11 -Wall $^ -o $@ -L/usr/local/lib -lserial

serial_slave_all: serial_slave_all.o
	g++ -std=c++11 -Wall $^ -o $@ -L/usr/local/lib -lserial

serial_at: serial_at.o
	g++ -std=c++11 -Wall $^ -o $@ -L/usr/local/lib -lserial

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理规则
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
