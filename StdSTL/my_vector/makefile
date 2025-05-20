# mystl::vector 项目的Makefile
# 编译选项
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall

# 目标文件
TARGET = vector_test

# 默认目标
all: $(TARGET)

# 编译规则
$(TARGET): vector_test.cpp my_vector.h
	$(CXX) $(CXXFLAGS) vector_test.cpp -o $(TARGET)

# 运行测试
run: $(TARGET)
	./$(TARGET)

# 清理规则
clean:
	rm -f $(TARGET)

.PHONY: all run clean 