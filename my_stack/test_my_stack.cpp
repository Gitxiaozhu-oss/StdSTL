/**
 * @file test_my_stack.cpp
 * @brief 测试my_stack实现的正确性
 */
#include "my_stack.h"
#include <iostream>
#include <string>
#include <cassert>
#include <vector>

/**
 * @brief 测试基本功能
 */
void test_basic_operations() {
    std::cout << "测试基本操作..." << std::endl;
    
    // 创建空栈
    mystl::stack<int> s;
    assert(s.empty());
    assert(s.size() == 0);
    
    // 添加元素
    s.push(10);
    s.push(20);
    s.push(30);
    
    assert(!s.empty());
    assert(s.size() == 3);
    assert(s.top() == 30);
    
    // 移除元素
    s.pop();
    assert(s.size() == 2);
    assert(s.top() == 20);
    
    s.pop();
    assert(s.size() == 1);
    assert(s.top() == 10);
    
    s.pop();
    assert(s.empty());
    
    std::cout << "基本操作测试通过！" << std::endl;
}

/**
 * @brief 测试构造函数
 */
void test_constructors() {
    std::cout << "测试构造函数..." << std::endl;
    
    // 默认构造函数
    mystl::stack<int> s1;
    assert(s1.empty());
    
    // 指定大小构造函数
    mystl::stack<int> s2(3);
    assert(s2.size() == 3);
    
    // 指定大小和初值的构造函数
    mystl::stack<int> s3(3, 10);
    assert(s3.size() == 3);
    assert(s3.top() == 10);
    
    // 迭代器范围构造函数
    int arr[] = {1, 2, 3, 4, 5};
    mystl::stack<int> s4(arr, arr + 5);
    assert(s4.size() == 5);
    assert(s4.top() == 5);
    
    std::cout << "构造函数测试通过！" << std::endl;
}

/**
 * @brief 测试赋值操作
 */
void test_assignments() {
    std::cout << "测试赋值操作..." << std::endl;
    
    mystl::stack<int> s1;
    s1.push(1);
    s1.push(2);
    s1.push(3);
    
    mystl::stack<int> s2;
    
    // 拷贝赋值
    s2 = s1;
    assert(s2.size() == 3);
    assert(s2.top() == 3);
    
    std::cout << "赋值操作测试通过！" << std::endl;
}

/**
 * @brief 测试比较操作
 */
void test_comparisons() {
    std::cout << "测试比较操作..." << std::endl;
    
    mystl::stack<int> s1;
    s1.push(1);
    s1.push(2);
    s1.push(3);
    
    mystl::stack<int> s2;
    s2.push(1);
    s2.push(2);
    s2.push(3);
    
    mystl::stack<int> s3;
    s3.push(1);
    s3.push(2);
    s3.push(4);
    
    mystl::stack<int> s4;
    s4.push(1);
    s4.push(2);
    
    // 相等比较
    assert(s1 == s2);
    assert(!(s1 == s3));
    
    // 不等比较
    assert(s1 != s3);
    assert(!(s1 != s2));
    
    // 小于比较
    assert(s1 < s3);
    assert(!(s3 < s1));
    assert(s4 < s1); // 元素少的栈小于元素多的栈（因为底层比较是字典序）
    
    // 大于比较
    assert(s3 > s1);
    assert(!(s1 > s3));
    
    // 小于等于比较
    assert(s1 <= s2);
    assert(s1 <= s3);
    assert(!(s3 <= s1));
    
    // 大于等于比较
    assert(s1 >= s2);
    assert(s3 >= s1);
    assert(!(s1 >= s3));
    
    std::cout << "比较操作测试通过！" << std::endl;
}

/**
 * @brief 测试emplace操作
 */
void test_emplace() {
    std::cout << "测试emplace操作..." << std::endl;
    
    // 测试emplace
    mystl::stack<std::string> s1;
    s1.emplace("Hello");
    s1.emplace("World");
    assert(s1.size() == 2);
    assert(s1.top() == "World");
    
    std::cout << "emplace操作测试通过！" << std::endl;
}

/**
 * @brief 测试clear操作
 */
void test_clear() {
    std::cout << "测试clear操作..." << std::endl;
    
    // 测试clear
    mystl::stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    
    assert(s.size() == 3);
    s.clear();
    assert(s.empty());
    
    std::cout << "clear操作测试通过！" << std::endl;
}

/**
 * @brief 测试swap操作
 */
void test_swap() {
    std::cout << "测试swap操作..." << std::endl;
    
    // 测试swap
    mystl::stack<int> s1;
    s1.push(1);
    s1.push(2);
    s1.push(3);
    
    mystl::stack<int> s2;
    s2.push(4);
    s2.push(5);
    
    // 成员函数swap
    s1.swap(s2);
    assert(s1.size() == 2);
    assert(s1.top() == 5);
    assert(s2.size() == 3);
    assert(s2.top() == 3);
    
    std::cout << "swap操作测试通过！" << std::endl;
}

/**
 * @brief 测试全局swap函数
 */
void test_global_swap() {
    std::cout << "测试全局swap函数..." << std::endl;
    
    mystl::stack<int> s1;
    s1.push(1);
    s1.push(2);
    s1.push(3);
    
    mystl::stack<int> s2;
    s2.push(4);
    s2.push(5);
    
    // 全局函数swap
    mystl::swap(s1, s2);
    assert(s1.size() == 2);
    assert(s1.top() == 5);
    assert(s2.size() == 3);
    assert(s2.top() == 3);
    
    std::cout << "全局swap函数测试通过！" << std::endl;
}

/**
 * @brief 测试不同底层容器
 */
void test_different_container() {
    std::cout << "测试使用不同底层容器..." << std::endl;
    
    // 使用std::vector作为底层容器
    mystl::stack<int, std::vector<int>> s;
    s.push(10);
    s.push(20);
    s.push(30);
    
    assert(s.size() == 3);
    assert(s.top() == 30);
    
    s.pop();
    assert(s.size() == 2);
    assert(s.top() == 20);
    
    std::cout << "不同底层容器测试通过！" << std::endl;
}

int main() {
    std::cout << "开始测试my_stack..." << std::endl;
    
    test_basic_operations();
    test_constructors();
    test_assignments();
    test_comparisons();
    test_emplace();
    test_clear();
    test_swap();
    test_global_swap();
    test_different_container();
    
    std::cout << "所有测试通过！my_stack实现正确。" << std::endl;
    
    return 0;
} 