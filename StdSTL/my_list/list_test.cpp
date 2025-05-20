#include <iostream>
#include <list>
#include <string>
#include <cassert>
#include <chrono>
#include "my_list.h"

/**
 * @brief 测试计时器类
 */
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool running;

public:
    Timer() : running(false) {}

    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        running = true;
    }

    void stop() {
        end_time = std::chrono::high_resolution_clock::now();
        running = false;
    }

    // 返回微秒
    double elapsed_us() {
        auto end = running ? std::chrono::high_resolution_clock::now() : end_time;
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start_time).count();
    }

    // 返回毫秒
    double elapsed_ms() {
        return elapsed_us() / 1000.0;
    }
};

// 使用宏简化测试代码
#define PRINT_TEST_TITLE(title) \
    std::cout << "\n=== " << title << " ===" << std::endl

#define RUN_TEST(test_func) \
    std::cout << "- 运行测试: " << #test_func << "..." << std::endl; \
    test_func(); \
    std::cout << "  测试通过!" << std::endl

/**
 * @brief 测试构造函数
 */
void test_constructors() {
    std::cout << "  测试默认构造函数" << std::endl;
    // 默认构造函数
    mystl::list<int> l1;
    std::cout << "  测试默认构造函数完成" << std::endl;
    std::cout << "  测试empty()和size()" << std::endl;
    assert(l1.empty());
    assert(l1.size() == 0);
    std::cout << "  测试empty()和size()完成" << std::endl;

    std::cout << "  测试填充构造函数" << std::endl;
    // 填充构造函数
    mystl::list<int> l2(5, 10);
    std::cout << "  测试填充构造函数完成" << std::endl;
    assert(l2.size() == 5);
    for (auto& elem : l2) {
        assert(elem == 10);
    }

    std::cout << "  测试范围构造函数" << std::endl;
    // 范围构造函数
    mystl::list<int> l3(l2.begin(), l2.end());
    std::cout << "  测试范围构造函数完成" << std::endl;
    assert(l3.size() == 5);
    assert(std::equal(l2.begin(), l2.end(), l3.begin()));

    std::cout << "  测试复制构造函数" << std::endl;
    // 复制构造函数
    mystl::list<int> l4(l3);
    std::cout << "  测试复制构造函数完成" << std::endl;
    assert(l4.size() == 5);
    assert(std::equal(l3.begin(), l3.end(), l4.begin()));

    std::cout << "  测试移动构造函数" << std::endl;
    // 移动构造函数
    mystl::list<int> l5(std::move(l4));
    std::cout << "  测试移动构造函数完成" << std::endl;
    assert(l5.size() == 5);
    std::cout << "  移动后测试l5.size()完成" << std::endl;
    assert(l4.empty());  // l4被移动后应该为空
    std::cout << "  移动后测试l4.empty()完成" << std::endl;

    std::cout << "  测试初始化列表构造函数" << std::endl;
    // 初始化列表构造函数
    mystl::list<int> l6 = {1, 2, 3, 4, 5};
    std::cout << "  测试初始化列表构造函数完成" << std::endl;
    assert(l6.size() == 5);
    auto it = l6.begin();
    for (int i = 1; i <= 5; ++i, ++it) {
        assert(*it == i);
    }
}

/**
 * @brief 测试赋值函数
 */
void test_assignments() {
    // 复制赋值
    mystl::list<int> l1 = {1, 2, 3};
    mystl::list<int> l2;
    l2 = l1;
    assert(l2.size() == 3);
    assert(std::equal(l1.begin(), l1.end(), l2.begin()));

    // 移动赋值
    mystl::list<int> l3;
    l3 = std::move(l2);
    assert(l3.size() == 3);
    assert(l2.empty());  // l2被移动后应该为空

    // 初始化列表赋值
    l3 = {5, 6, 7, 8};
    assert(l3.size() == 4);
    auto it = l3.begin();
    for (int i = 5; i <= 8; ++i, ++it) {
        assert(*it == i);
    }

    // assign方法 - 填充
    l1.assign(4, 20);
    assert(l1.size() == 4);
    for (auto& elem : l1) {
        assert(elem == 20);
    }

    // assign方法 - 范围
    int arr[] = {100, 200, 300};
    l2.assign(std::begin(arr), std::end(arr));
    assert(l2.size() == 3);
    it = l2.begin();
    for (int i = 0; i < 3; ++i, ++it) {
        assert(*it == arr[i]);
    }

    // assign方法 - 初始化列表
    l3.assign({-1, -2, -3});
    assert(l3.size() == 3);
    it = l3.begin();
    int expected[] = {-1, -2, -3};
    for (int i = 0; i < 3; ++i, ++it) {
        assert(*it == expected[i]);
    }
}

/**
 * @brief 测试元素访问
 */
void test_element_access() {
    mystl::list<int> l = {10, 20, 30, 40, 50};
    
    // front
    assert(l.front() == 10);
    l.front() = 15;
    assert(l.front() == 15);
    
    // back
    assert(l.back() == 50);
    l.back() = 55;
    assert(l.back() == 55);
}

/**
 * @brief 测试容量相关函数
 */
void test_capacity() {
    mystl::list<int> l;
    
    // empty & size
    assert(l.empty());
    assert(l.size() == 0);
    
    l = {1, 2, 3};
    assert(!l.empty());
    assert(l.size() == 3);
    
    l.clear();
    assert(l.empty());
    assert(l.size() == 0);
}

/**
 * @brief 测试迭代器
 */
void test_iterators() {
    mystl::list<int> l = {10, 20, 30, 40, 50};
    
    // 正向迭代器
    auto it = l.begin();
    assert(*it == 10);
    ++it;
    assert(*it == 20);
    
    // 反向迭代器
    auto rit = l.rbegin();
    assert(*rit == 50);
    ++rit;
    assert(*rit == 40);
    
    // 迭代器范围
    int sum = 0;
    for (auto& n : l) {
        sum += n;
    }
    assert(sum == 150);
}

/**
 * @brief 测试修改器函数
 */
void test_modifiers() {
    // clear
    mystl::list<int> l1 = {1, 2, 3};
    l1.clear();
    assert(l1.empty());
    
    // insert
    l1.insert(l1.begin(), 10);
    assert(l1.size() == 1);
    assert(l1.front() == 10);
    
    l1.insert(l1.end(), 3, 20);
    assert(l1.size() == 4);
    assert(l1.back() == 20);
    
    mystl::list<int> l2;
    l2.insert(l2.begin(), l1.begin(), l1.end());
    assert(l2.size() == 4);
    assert(std::equal(l1.begin(), l1.end(), l2.begin()));
    
    l2.insert(l2.begin(), {-1, -2, -3});
    assert(l2.size() == 7);
    assert(l2.front() == -1);
    
    // emplace
    l1.emplace(l1.begin(), 5);
    assert(l1.front() == 5);
    
    // erase
    auto it = l1.begin();
    ++it;
    it = l1.erase(it);
    assert(l1.size() == 4);
    assert(*it == 20);  // 现在it指向被删除元素后的元素
    
    it = l1.begin();
    auto it_end = l1.end();
    --it_end;  // 指向最后一个元素
    l1.erase(it, it_end);
    assert(l1.size() == 1);
    assert(l1.front() == l1.back());
    
    // push & pop
    l1.push_back(100);
    l1.push_front(50);
    assert(l1.size() == 3);
    assert(l1.front() == 50);
    assert(l1.back() == 100);
    
    l1.pop_back();
    l1.pop_front();
    assert(l1.size() == 1);
    
    // resize
    l1.resize(5);
    assert(l1.size() == 5);
    
    l1.resize(2);
    assert(l1.size() == 2);
    
    l1.resize(4, 30);
    assert(l1.size() == 4);
    auto last = l1.end();
    --last;
    assert(*last == 30);
    
    // swap
    mystl::list<int> l3 = {1, 2, 3};
    mystl::list<int> l4 = {4, 5, 6, 7};
    l3.swap(l4);
    assert(l3.size() == 4);
    assert(l4.size() == 3);
    assert(l3.front() == 4);
    assert(l4.front() == 1);
}

/**
 * @brief 测试链表特有操作
 */
void test_list_operations() {
    std::cout << "  测试splice - 整个链表" << std::endl;
    // splice - 整个链表
    mystl::list<int> l1 = {1, 2, 3};
    mystl::list<int> l2 = {4, 5, 6};
    l1.splice(l1.end(), l2);
    std::cout << "  l1.size = " << l1.size() << ", l2.size = " << l2.size() << std::endl;
    assert(l1.size() == 6);
    assert(l2.empty());
    
    std::cout << "  测试splice - 单个元素" << std::endl;
    // splice - 单个元素
    mystl::list<int> l3 = {7, 8, 9};
    auto it = l3.begin();
    ++it;  // 指向8
    l1.splice(l1.begin(), l3, it);
    std::cout << "  l1.size = " << l1.size() << ", l3.size = " << l3.size() << std::endl;
    assert(l1.size() == 7);
    assert(l1.front() == 8);
    assert(l3.size() == 2);
    
    std::cout << "  测试splice - 元素范围" << std::endl;
    // splice - 元素范围
    it = l3.begin();
    l1.splice(l1.end(), l3, it, l3.end());
    std::cout << "  l1.size = " << l1.size() << ", l3.size = " << l3.size() << std::endl;
    assert(l1.size() == 9);
    assert(l3.empty());
    
    std::cout << "  测试remove" << std::endl;
    // remove
    l1.remove(8);
    std::cout << "  l1.size = " << l1.size() << std::endl;
    assert(l1.size() == 8);
    for (auto& n : l1) {
        assert(n != 8);
    }
    
    std::cout << "  测试remove_if" << std::endl;
    // remove_if
    l1.remove_if([](int n) { return n % 2 == 0; });
    std::cout << "  l1.size = " << l1.size() << std::endl;
    std::cout << "  l1内容: ";
    for (auto& n : l1) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    assert(l1.size() == 5);  // 修正为5，因为l1现在包含 1 3 5 7 9
    for (auto& n : l1) {
        assert(n % 2 != 0);
    }
    
    std::cout << "  测试unique" << std::endl;
    // unique
    mystl::list<int> l4 = {1, 1, 2, 2, 2, 3, 3, 1};
    std::cout << "  调用unique前 l4.size = " << l4.size() << std::endl;
    l4.unique();
    std::cout << "  调用unique后 l4.size = " << l4.size() << std::endl;
    std::cout << "  l4内容: ";
    for (auto& n : l4) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    assert(l4.size() == 4);  // 1, 2, 3, 1 (四个元素，相邻重复的被移除)
    
    std::cout << "  测试unique - 使用谓词" << std::endl;
    mystl::list<int> l5 = {10, 11, 20, 21, 30};
    std::cout << "  调用unique前 l5.size = " << l5.size() << std::endl;
    l5.unique([](int a, int b) { return a / 10 == b / 10; });
    std::cout << "  调用unique后 l5.size = " << l5.size() << std::endl;
    std::cout << "  l5内容: ";
    for (auto& n : l5) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    assert(l5.size() == 3);
    
    std::cout << "  测试merge" << std::endl;
    // merge
    mystl::list<int> l6 = {1, 3, 5};
    mystl::list<int> l7 = {2, 4, 6};
    std::cout << "  调用merge前 l6.size = " << l6.size() << ", l7.size = " << l7.size() << std::endl;
    l6.merge(l7);
    std::cout << "  调用merge后 l6.size = " << l6.size() << ", l7.size = " << l7.size() << std::endl;
    assert(l6.size() == 6);
    assert(l7.empty());
    
    it = l6.begin();
    for (int i = 1; i <= 6; ++i, ++it) {
        assert(*it == i);
    }
    
    std::cout << "  测试sort" << std::endl;
    // sort
    mystl::list<int> l8 = {4, 1, 3, 5, 2};
    std::cout << "  调用sort前 l8内容: ";
    for (auto& n : l8) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    l8.sort();
    std::cout << "  调用sort后 l8内容: ";
    for (auto& n : l8) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    it = l8.begin();
    for (int i = 1; i <= 5; ++i, ++it) {
        assert(*it == i);
    }
    
    std::cout << "  测试sort - 使用谓词" << std::endl;
    // sort - 使用谓词
    l8.sort(std::greater<int>());
    std::cout << "  使用谓词sort后 l8内容: ";
    for (auto& n : l8) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    it = l8.begin();
    for (int i = 5; i >= 1; --i, ++it) {
        assert(*it == i);
    }
    
    std::cout << "  测试reverse" << std::endl;
    // reverse
    l8.reverse();
    std::cout << "  调用reverse后 l8内容: ";
    for (auto& n : l8) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    it = l8.begin();
    for (int i = 1; i <= 5; ++i, ++it) {
        assert(*it == i);
    }
}

/**
 * @brief 测试与标准库的兼容性
 */
void test_std_compatibility() {
    // 复制到std::list
    mystl::list<int> l1 = {1, 2, 3, 4, 5};
    std::list<int> l2(l1.begin(), l1.end());
    assert(std::equal(l1.begin(), l1.end(), l2.begin()));
    
    // 从std::list复制
    std::list<int> l3 = {10, 20, 30};
    mystl::list<int> l4(l3.begin(), l3.end());
    assert(std::equal(l3.begin(), l3.end(), l4.begin()));
}

/**
 * @brief 测试比较操作符
 */
void test_relational_operators() {
    mystl::list<int> l1 = {1, 2, 3};
    mystl::list<int> l2 = {1, 2, 3};
    mystl::list<int> l3 = {1, 2, 4};
    mystl::list<int> l4 = {1, 2};
    
    // ==
    assert(l1 == l2);
    assert(!(l1 == l3));
    assert(!(l1 == l4));
    
    // !=
    assert(!(l1 != l2));
    assert(l1 != l3);
    assert(l1 != l4);
    
    // <
    assert(!(l1 < l2));
    assert(l1 < l3);
    assert(!(l1 < l4));
    assert(l4 < l1);
    
    // >
    assert(!(l1 > l2));
    assert(!(l1 > l3));
    assert(l1 > l4);
    assert(!(l4 > l1));
    
    // <=
    assert(l1 <= l2);
    assert(l1 <= l3);
    assert(!(l1 <= l4));
    assert(l4 <= l1);
    
    // >=
    assert(l1 >= l2);
    assert(!(l1 >= l3));
    assert(l1 >= l4);
    assert(!(l4 >= l1));
}

/**
 * @brief 性能测试
 */
void test_performance() {
    const int NUM_ELEMENTS = 100000;
    Timer timer;
    
    std::cout << "\n--- 性能测试 ---" << std::endl;
    
    // 插入性能测试
    {
        timer.start();
        mystl::list<int> l;
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            l.push_back(i);
        }
        timer.stop();
        std::cout << "mystl::list 插入 " << NUM_ELEMENTS << " 元素: " << timer.elapsed_ms() << " ms" << std::endl;
        
        timer.start();
        std::list<int> l_std;
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            l_std.push_back(i);
        }
        timer.stop();
        std::cout << "std::list 插入 " << NUM_ELEMENTS << " 元素: " << timer.elapsed_ms() << " ms" << std::endl;
    }
    
    // 访问性能测试
    {
        mystl::list<int> l;
        std::list<int> l_std;
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            l.push_back(i);
            l_std.push_back(i);
        }
        
        timer.start();
        int sum = 0;
        for (auto& n : l) {
            sum += n;
        }
        timer.stop();
        std::cout << "mystl::list 遍历 " << NUM_ELEMENTS << " 元素: " << timer.elapsed_ms() << " ms" << std::endl;
        
        timer.start();
        sum = 0;
        for (auto& n : l_std) {
            sum += n;
        }
        timer.stop();
        std::cout << "std::list 遍历 " << NUM_ELEMENTS << " 元素: " << timer.elapsed_ms() << " ms" << std::endl;
    }
    
    // 排序性能测试
    {
        mystl::list<int> l;
        std::list<int> l_std;
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            l.push_back(NUM_ELEMENTS - i);
            l_std.push_back(NUM_ELEMENTS - i);
        }
        
        timer.start();
        l.sort();
        timer.stop();
        std::cout << "mystl::list 排序 " << NUM_ELEMENTS << " 元素: " << timer.elapsed_ms() << " ms" << std::endl;
        
        timer.start();
        l_std.sort();
        timer.stop();
        std::cout << "std::list 排序 " << NUM_ELEMENTS << " 元素: " << timer.elapsed_ms() << " ms" << std::endl;
    }
}

/**
 * @brief 异常安全测试
 */
class ExceptionTester {
private:
    static int throw_counter;
    static int throw_threshold;
    int value;

public:
    static void reset(int threshold = -1) {
        throw_counter = 0;
        throw_threshold = threshold;
    }

    ExceptionTester(int val = 0) : value(val) {
        if (++throw_counter == throw_threshold) {
            throw std::runtime_error("Exception tester");
        }
    }

    ExceptionTester(const ExceptionTester& other) : value(other.value) {
        if (++throw_counter == throw_threshold) {
            throw std::runtime_error("Exception tester");
        }
    }

    ExceptionTester& operator=(const ExceptionTester& other) {
        value = other.value;
        if (++throw_counter == throw_threshold) {
            throw std::runtime_error("Exception tester");
        }
        return *this;
    }

    bool operator==(const ExceptionTester& other) const {
        return value == other.value;
    }

    bool operator<(const ExceptionTester& other) const {
        return value < other.value;
    }

    friend std::ostream& operator<<(std::ostream& os, const ExceptionTester& obj) {
        return os << obj.value;
    }
};

int ExceptionTester::throw_counter = 0;
int ExceptionTester::throw_threshold = -1;

void test_exception_safety() {
    // 测试元素构造期间的异常
    {
        mystl::list<ExceptionTester> l;
        ExceptionTester::reset(3);  // 第3次分配时抛出异常
        
        try {
            mystl::list<ExceptionTester> l2(5, ExceptionTester(1));
            assert(false);  // 应该不会执行到这里
        } catch (const std::runtime_error&) {
            // 预期的异常
        }
        
        // l应该依然是有效的
        assert(l.empty());
    }
    
    // 测试插入操作期间的异常
    {
        mystl::list<ExceptionTester> l;
        l.push_back(ExceptionTester(1));
        l.push_back(ExceptionTester(2));
        
        ExceptionTester::reset(2);  // 第2次分配时抛出异常
        
        try {
            l.insert(l.begin(), 5, ExceptionTester(3));
            assert(false);  // 应该不会执行到这里
        } catch (const std::runtime_error&) {
            // 预期的异常
        }
        
        // l应该依然是有效的，且保持原有内容
        assert(l.size() == 2);
        auto it = l.begin();
        assert(*it == ExceptionTester(1));
        ++it;
        assert(*it == ExceptionTester(2));
    }
}

int main() {
    std::cout << "===== 测试 mystl::list 实现 =====" << std::endl;
    
    // 运行基本功能测试
    RUN_TEST(test_constructors);
    RUN_TEST(test_assignments);
    RUN_TEST(test_element_access);
    RUN_TEST(test_capacity);
    RUN_TEST(test_iterators);
    RUN_TEST(test_modifiers);
    RUN_TEST(test_list_operations);
    RUN_TEST(test_std_compatibility);
    RUN_TEST(test_relational_operators);
    
    // 运行性能测试
    test_performance();
    
    // 运行异常安全测试
    RUN_TEST(test_exception_safety);
    
    std::cout << "\n所有测试通过！" << std::endl;
    
    return 0;
} 