#include <iostream>
#include <string>
#include "my_deque.h"

/**
 * @brief 一个简单的测试函数，测试deque容器的基本功能
 */
void test_deque() {
    // 测试默认构造函数
    mystl::deque<int> d1;
    
    // 测试填充构造函数
    mystl::deque<int> d2(5, 10);
    
    // 测试迭代器构造函数
    int arr[] = {1, 2, 3, 4, 5};
    mystl::deque<int> d3(arr, arr + 5);
    
    // 测试拷贝构造函数
    mystl::deque<int> d4(d3);
    
    // 测试移动构造函数
    mystl::deque<int> d5(std::move(d4));
    
    // 测试赋值操作
    d1 = d2;
    
    // 测试元素访问
    std::cout << "d1[0] = " << d1[0] << std::endl;
    std::cout << "d1.front() = " << d1.front() << std::endl;
    std::cout << "d1.back() = " << d1.back() << std::endl;
    
    // 测试容量相关操作
    std::cout << "d1.size() = " << d1.size() << std::endl;
    std::cout << "d1.empty() = " << d1.empty() << std::endl;
    
    // 测试修改容器操作
    d1.push_back(100);
    d1.push_front(200);
    std::cout << "After push: d1.size() = " << d1.size() << std::endl;
    std::cout << "d1.front() = " << d1.front() << std::endl;
    std::cout << "d1.back() = " << d1.back() << std::endl;
    
    // 测试删除操作
    d1.pop_back();
    d1.pop_front();
    std::cout << "After pop: d1.size() = " << d1.size() << std::endl;
    
    // 测试插入操作
    d1.insert(d1.begin() + 2, 300);
    std::cout << "After insert: ";
    for (auto it = d1.begin(); it != d1.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试删除操作
    d1.erase(d1.begin() + 1);
    std::cout << "After erase: ";
    for (auto it = d1.begin(); it != d1.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试清空操作
    d1.clear();
    std::cout << "After clear: d1.size() = " << d1.size() << std::endl;
    
    // 测试resize操作
    d3.resize(8, 100);
    std::cout << "After resize: ";
    for (auto it = d3.begin(); it != d3.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试shrink_to_fit操作
    d3.shrink_to_fit();
    
    // 测试复杂类型
    mystl::deque<std::string> d6;
    d6.push_back("Hello");
    d6.push_back("World");
    d6.emplace_back("!");
    
    std::cout << "String deque: ";
    for (const auto& s : d6) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "Testing my_deque implementation..." << std::endl;
    test_deque();
    std::cout << "All tests completed." << std::endl;
    return 0;
} 