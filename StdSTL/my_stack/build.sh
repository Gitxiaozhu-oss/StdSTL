#!/bin/bash

# 编译测试程序
g++ -std=c++11 -Wall -Wextra -pedantic -o test_my_stack test_my_stack.cpp

# 检查编译是否成功
if [ $? -ne 0 ]; then
    echo "编译失败！"
    exit 1
fi

# 运行测试程序
./test_my_stack

# 检查测试是否通过
if [ $? -ne 0 ]; then
    echo "测试失败！"
    exit 1
else
    echo "测试成功完成！"
fi 