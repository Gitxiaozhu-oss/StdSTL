# mystl::vector 项目的Makefile
# 编译选项
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall

# 目标文件
TARGET = test_unordered_map

# 默认目标
all: $(TARGET)

# 编译规则
$(TARGET): test_unordered_map.cpp my_unordered_map.h
	$(CXX) $(CXXFLAGS) test_unordered_map.cpp -o $(TARGET)

# 运行测试
run: $(TARGET)
	./$(TARGET)

# 清理规则
clean:
	rm -f $(TARGET)

.PHONY: all run clean 