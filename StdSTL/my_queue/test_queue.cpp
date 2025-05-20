#include "my_queue.h"
#include <iostream>
#include <string>
#include <vector>
#include <functional>

/**
 * @brief 测试普通队列的基本功能
 */
void test_queue() {
    std::cout << "===== 测试队列(queue)的基本功能 =====" << std::endl;
    
    // 默认构造函数测试
    mystl::queue<int> q1;
    std::cout << "默认构造的队列是否为空: " << (q1.empty() ? "是" : "否") << std::endl;
    
    // push操作测试
    std::cout << "向队列添加元素: 10, 20, 30" << std::endl;
    q1.push(10);
    q1.push(20);
    q1.push(30);
    std::cout << "队列大小: " << q1.size() << std::endl;
    std::cout << "队首元素: " << q1.front() << std::endl;
    std::cout << "队尾元素: " << q1.back() << std::endl;
    
    // pop操作测试
    std::cout << "执行pop操作" << std::endl;
    q1.pop();
    std::cout << "pop后队列大小: " << q1.size() << std::endl;
    std::cout << "pop后队首元素: " << q1.front() << std::endl;
    
    // 使用初始化列表构造
    std::cout << "\n使用初始化列表构造队列" << std::endl;
    mystl::queue<int> q2({100, 200, 300, 400});
    std::cout << "队列大小: " << q2.size() << std::endl;
    std::cout << "队首元素: " << q2.front() << std::endl;
    std::cout << "队尾元素: " << q2.back() << std::endl;
    
    // 拷贝构造测试
    std::cout << "\n使用拷贝构造函数" << std::endl;
    mystl::queue<int> q3(q2);
    std::cout << "拷贝构造的队列大小: " << q3.size() << std::endl;
    std::cout << "拷贝构造的队首元素: " << q3.front() << std::endl;
    std::cout << "拷贝构造的队尾元素: " << q3.back() << std::endl;
    
    // 移动构造测试
    std::cout << "\n使用移动构造函数" << std::endl;
    mystl::queue<int> q4(std::move(q3));
    std::cout << "移动后源队列大小: " << q3.size() << std::endl;
    std::cout << "移动后目标队列大小: " << q4.size() << std::endl;
    std::cout << "移动后目标队列队首元素: " << q4.front() << std::endl;
    
    // 清空操作测试
    std::cout << "\n测试清空操作" << std::endl;
    std::cout << "清空前大小: " << q4.size() << std::endl;
    q4.clear();
    std::cout << "清空后大小: " << q4.size() << std::endl;
    
    // 使用其他容器作为底层容器
    std::cout << "\n使用std::vector作为底层容器" << std::endl;
    mystl::queue<int, std::vector<int>> q8({1, 2, 3, 4, 5});
    std::cout << "队列大小: " << q8.size() << std::endl;
    std::cout << "队列前端: " << q8.front() << std::endl;
    std::cout << "队列后端: " << q8.back() << std::endl;
}

/**
 * @brief 自定义类型结构体，用于测试
 */
class Person {
public:
    std::string name;
    int age;
    
    Person(const std::string& n, int a) : name(n), age(a) {}
    
    bool operator<(const Person& rhs) const {
        return age < rhs.age; // 按年龄比较
    }
    
    bool operator==(const Person& rhs) const {
        return age == rhs.age && name == rhs.name;
    }

    // 添加析构函数
    ~Person() {}
};

/**
 * @brief 测试优先队列的基本功能
 */
void test_priority_queue() {
    std::cout << "\n===== 测试优先队列(priority_queue)的基本功能 =====" << std::endl;
    
    // 默认构造函数测试 (默认是最大堆)
    mystl::priority_queue<int> pq1;
    std::cout << "默认构造的优先队列是否为空: " << (pq1.empty() ? "是" : "否") << std::endl;
    
    // push操作测试
    std::cout << "向优先队列添加元素: 30, 10, 20, 5, 40" << std::endl;
    pq1.push(30);
    pq1.push(10);
    pq1.push(20);
    pq1.push(5);
    pq1.push(40);
    std::cout << "优先队列大小: " << pq1.size() << std::endl;
    std::cout << "优先队列顶部元素(最大值): " << pq1.top() << std::endl;
    
    // pop操作测试
    std::cout << "执行pop操作" << std::endl;
    pq1.pop();
    std::cout << "pop后优先队列大小: " << pq1.size() << std::endl;
    std::cout << "pop后优先队列顶部元素: " << pq1.top() << std::endl;
    
    // 使用自定义比较器(最小堆)
    std::cout << "\n使用自定义比较器(最小堆)" << std::endl;
    mystl::priority_queue<int, mystl::vector<int>, std::greater<int>> pq2;
    std::cout << "向最小堆优先队列添加元素: 30, 10, 20, 5, 40" << std::endl;
    pq2.push(30);
    pq2.push(10);
    pq2.push(20);
    pq2.push(5);
    pq2.push(40);
    std::cout << "优先队列大小: " << pq2.size() << std::endl;
    std::cout << "优先队列顶部元素(最小值): " << pq2.top() << std::endl;
    
    // 使用初始化列表构造
    std::cout << "\n使用初始化列表构造优先队列" << std::endl;
    mystl::priority_queue<int> pq3({100, 200, 50, 400, 350});
    std::cout << "优先队列大小: " << pq3.size() << std::endl;
    std::cout << "优先队列顶部元素: " << pq3.top() << std::endl;
    
    // 拷贝构造测试
    std::cout << "\n使用拷贝构造函数" << std::endl;
    mystl::priority_queue<int> pq4(pq3);
    std::cout << "拷贝构造的优先队列大小: " << pq4.size() << std::endl;
    std::cout << "拷贝构造的优先队列顶部元素: " << pq4.top() << std::endl;
    
    // 移动构造测试
    std::cout << "\n使用移动构造函数" << std::endl;
    mystl::priority_queue<int> pq5(std::move(pq4));
    std::cout << "移动后源优先队列大小: " << pq4.size() << std::endl;
    std::cout << "移动后目标优先队列大小: " << pq5.size() << std::endl;
    if (!pq5.empty()) {
        std::cout << "移动后目标优先队列顶部元素: " << pq5.top() << std::endl;
    }
}

/**
 * @brief 测试优先队列的自定义类型
 */
void test_custom_type() {
    std::cout << "\n===== 测试优先队列的自定义类型 =====" << std::endl;
    
    // 创建一个优先队列，存储整数
    mystl::priority_queue<int> pq_int;
    pq_int.push(10);
    pq_int.push(30);
    pq_int.push(20);
    
    std::cout << "整数优先队列大小: " << pq_int.size() << std::endl;
    std::cout << "整数优先队列顶部元素: " << pq_int.top() << std::endl;
    
    // 创建存储字符串的优先队列
    mystl::priority_queue<std::string> pq_str;
    pq_str.push(std::string("苹果"));
    pq_str.push(std::string("香蕉"));
    pq_str.push(std::string("橙子"));
    
    std::cout << "\n字符串优先队列大小: " << pq_str.size() << std::endl;
    std::cout << "字符串优先队列顶部元素: " << pq_str.top() << std::endl;
}

/**
 * @brief 测试emplace操作
 */
void test_emplace() {
    std::cout << "\n===== 测试emplace操作 =====" << std::endl;
    
    // 队列的emplace测试
    std::cout << "队列的emplace测试:" << std::endl;
    mystl::queue<std::pair<int, std::string>> q;
    q.emplace(1, std::string("一"));
    q.emplace(2, std::string("二"));
    q.emplace(3, std::string("三"));
    
    std::cout << "队列大小: " << q.size() << std::endl;
    
    // 确保队列不为空再访问元素
    if (!q.empty()) {
        const auto& front = q.front();
        const auto& back = q.back();
        std::cout << "队首元素: (" << front.first << ", " << front.second << ")" << std::endl;
        std::cout << "队尾元素: (" << back.first << ", " << back.second << ")" << std::endl;
    }
    
    // 整数队列的emplace测试
    std::cout << "\n整数队列的emplace测试:" << std::endl;
    mystl::queue<int> qi;
    qi.emplace(10);
    qi.emplace(20);
    qi.emplace(30);
    
    std::cout << "队列大小: " << qi.size() << std::endl;
    if (!qi.empty()) {
        std::cout << "队首元素: " << qi.front() << std::endl;
        std::cout << "队尾元素: " << qi.back() << std::endl;
    }
}

int main() {
    std::cout << "======= 开始测试my_queue.h =======" << std::endl;
    
    // 测试普通队列
    test_queue();
    
    // 测试优先队列
    test_priority_queue();
    
    // 测试自定义类型
    test_custom_type();
    
    // 测试emplace操作
    test_emplace();
    
    std::cout << "\n======= 测试完成 =======" << std::endl;
    return 0;
} 