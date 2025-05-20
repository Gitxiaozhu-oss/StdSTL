# mystl::hashtable 项目的Makefile
# 编译选项
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall

# 目标文件
TARGET = test_hashtable

# 默认目标
all: $(TARGET)

# 编译规则
$(TARGET): test_hashtable.cpp my_hashtable.h
	$(CXX) $(CXXFLAGS) test_hashtable.cpp -o $(TARGET)

# 运行测试
run: $(TARGET)
	./$(TARGET)

# 清理规则
clean:
	rm -f $(TARGET)

.PHONY: all run clean 