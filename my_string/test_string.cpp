#include <iostream>
#include <cassert>
#include <string>
#include "my_string.h"

/**
 * @brief 测试用例辅助函数，用于比较和报告测试结果
 * 
 * @param test_name 测试名称
 * @param result 测试结果
 * @param expected 期望结果
 * @return bool 测试是否通过
 */
template <typename T>
bool test_equal(const char* test_name, const T& result, const T& expected) {
    bool passed = (result == expected);
    std::cout << (passed ? "[通过] " : "[失败] ") << test_name;
    
    if (!passed) {
        std::cout << " - 期望: " << expected << ", 实际: " << result;
    }
    
    std::cout << std::endl;
    return passed;
}

/**
 * @brief 测试构造函数
 */
void test_constructors() {
    std::cout << "===== 测试构造函数 =====" << std::endl;
    
    // 测试默认构造函数
    mystl::string s1;
    test_equal("默认构造函数", s1.size(), size_t(0));
    test_equal("默认构造函数c_str", s1.c_str()[0], '\0');
    
    // 测试从C风格字符串构造
    mystl::string s2 = "Hello";
    test_equal("C风格字符串构造函数", s2.size(), size_t(5));
    test_equal("C风格字符串构造函数内容", std::string(s2.c_str()), std::string("Hello"));
    
    // 测试填充构造函数
    mystl::string s3(5, 'A');
    test_equal("填充构造函数", s3.size(), size_t(5));
    test_equal("填充构造函数内容", std::string(s3.c_str()), std::string("AAAAA"));
    
    // 测试复制构造函数
    mystl::string s4 = s2;
    test_equal("复制构造函数", s4.size(), s2.size());
    test_equal("复制构造函数内容", std::string(s4.c_str()), std::string(s2.c_str()));
    
    // 测试移动构造函数
    mystl::string s5 = std::move(mystl::string("World"));
    test_equal("移动构造函数", s5.size(), size_t(5));
    test_equal("移动构造函数内容", std::string(s5.c_str()), std::string("World"));
    
    // 测试从初始化列表构造
    mystl::string s6 = {'H', 'i', '!'};
    test_equal("初始化列表构造函数", s6.size(), size_t(3));
    test_equal("初始化列表构造函数内容", std::string(s6.c_str()), std::string("Hi!"));
}

/**
 * @brief 测试赋值操作
 */
void test_assignment() {
    std::cout << "\n===== 测试赋值操作 =====" << std::endl;
    
    // 测试复制赋值操作符
    mystl::string s1 = "Original";
    mystl::string s2;
    s2 = s1;
    test_equal("复制赋值操作符", s2.size(), s1.size());
    test_equal("复制赋值操作符内容", std::string(s2.c_str()), std::string(s1.c_str()));
    
    // 测试移动赋值操作符
    mystl::string s3;
    s3 = std::move(mystl::string("Moved"));
    test_equal("移动赋值操作符", s3.size(), size_t(5));
    test_equal("移动赋值操作符内容", std::string(s3.c_str()), std::string("Moved"));
    
    // 测试C风格字符串赋值
    mystl::string s4;
    s4 = "C-String";
    test_equal("C风格字符串赋值", s4.size(), size_t(8));
    test_equal("C风格字符串赋值内容", std::string(s4.c_str()), std::string("C-String"));
    
    // 测试字符赋值
    mystl::string s5;
    s5 = 'X';
    test_equal("字符赋值", s5.size(), size_t(1));
    test_equal("字符赋值内容", std::string(s5.c_str()), std::string("X"));
    
    // 测试assign方法
    mystl::string s6;
    s6.assign("Assigned", 8);
    test_equal("assign方法", s6.size(), size_t(8));
    test_equal("assign方法内容", std::string(s6.c_str()), std::string("Assigned"));
    
    // 测试从初始化列表赋值
    mystl::string s7;
    s7 = {'A', 'B', 'C'};
    test_equal("初始化列表赋值", s7.size(), size_t(3));
    test_equal("初始化列表赋值内容", std::string(s7.c_str()), std::string("ABC"));
}

/**
 * @brief 测试元素访问
 */
void test_element_access() {
    std::cout << "\n===== 测试元素访问 =====" << std::endl;
    
    mystl::string s = "Hello";
    
    // 测试operator[]
    test_equal("operator[]", s[0], 'H');
    test_equal("operator[]", s[4], 'o');
    
    // 测试at方法
    test_equal("at方法", s.at(1), 'e');
    test_equal("at方法", s.at(3), 'l');
    
    // 测试front和back
    test_equal("front方法", s.front(), 'H');
    test_equal("back方法", s.back(), 'o');
    
    // 测试c_str和data
    test_equal("c_str方法", std::string(s.c_str()), std::string("Hello"));
    test_equal("data方法", std::string(s.data()), std::string("Hello"));
    
    // 测试at方法异常
    bool exception_thrown = false;
    try {
        s.at(10); // 这应该抛出异常
    } catch (const std::out_of_range&) {
        exception_thrown = true;
    }
    test_equal("at方法越界异常", exception_thrown, true);
}

/**
 * @brief 测试迭代器
 */
void test_iterators() {
    std::cout << "\n===== 测试迭代器 =====" << std::endl;
    
    mystl::string s = "Iterator";
    
    // 测试普通迭代器
    std::string result;
    for (auto it = s.begin(); it != s.end(); ++it) {
        result += *it;
    }
    test_equal("begin/end迭代器", result, std::string("Iterator"));
    
    // 测试反向迭代器
    result.clear();
    for (auto it = s.rbegin(); it != s.rend(); ++it) {
        result += *it;
    }
    test_equal("rbegin/rend迭代器", result, std::string("rotaretI"));
    
    // 测试常量迭代器
    result.clear();
    for (auto it = s.cbegin(); it != s.cend(); ++it) {
        result += *it;
    }
    test_equal("cbegin/cend迭代器", result, std::string("Iterator"));
    
    // 测试常量反向迭代器
    result.clear();
    for (auto it = s.crbegin(); it != s.crend(); ++it) {
        result += *it;
    }
    test_equal("crbegin/crend迭代器", result, std::string("rotaretI"));
}

/**
 * @brief 测试容量相关方法
 */
void test_capacity() {
    std::cout << "\n===== 测试容量方法 =====" << std::endl;
    
    // 测试size和length
    mystl::string s = "Capacity";
    test_equal("size方法", s.size(), size_t(8));
    test_equal("length方法", s.length(), size_t(8));
    
    // 测试empty
    mystl::string s1;
    mystl::string s2 = "NotEmpty";
    test_equal("empty方法 - 空字符串", s1.empty(), true);
    test_equal("empty方法 - 非空字符串", s2.empty(), false);
    
    // 测试resize
    mystl::string s3 = "Resize";
    s3.resize(10, 'X');
    test_equal("resize方法 - 扩大", s3.size(), size_t(10));
    test_equal("resize方法 - 扩大内容", std::string(s3.c_str()), std::string("ResizeXXXX"));
    
    s3.resize(4);
    test_equal("resize方法 - 缩小", s3.size(), size_t(4));
    test_equal("resize方法 - 缩小内容", std::string(s3.c_str()), std::string("Resi"));
    
    // 测试reserve和capacity
    mystl::string s4 = "Reserve";
    size_t old_capacity = s4.capacity();
    s4.reserve(100);
    test_equal("reserve方法 - 容量增加", s4.capacity() > old_capacity, true);
    test_equal("reserve方法 - 大小不变", s4.size(), size_t(7));
    test_equal("reserve方法 - 内容不变", std::string(s4.c_str()), std::string("Reserve"));
    
    // 测试shrink_to_fit
    mystl::string s5 = "Shrink";
    s5.reserve(100);
    old_capacity = s5.capacity();
    s5.shrink_to_fit();
    test_equal("shrink_to_fit方法", s5.capacity() < old_capacity, true);
    test_equal("shrink_to_fit方法 - 内容不变", std::string(s5.c_str()), std::string("Shrink"));
    
    // 测试clear
    mystl::string s6 = "Clear";
    s6.clear();
    test_equal("clear方法", s6.size(), size_t(0));
    test_equal("clear方法 - 空字符串", s6.empty(), true);
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "开始测试 mystl::string 类...\n" << std::endl;
    
    test_constructors();
    test_assignment();
    test_element_access();
    test_iterators();
    test_capacity();
    
    std::cout << "\n所有测试完成！" << std::endl;
    
    return 0;
} 