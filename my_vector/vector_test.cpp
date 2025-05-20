#include <iostream>
#include <string>
#include <utility>
#include <vector>   // 添加标准库vector头文件
#include <chrono>   // 添加计时相关头文件
#include <iomanip>  // 添加格式化输出头文件
#include <functional> // 添加functional头文件
#include "my_vector.h"

/**
 * @brief 测试vector的构造函数
 */
void test_constructor() {
    std::cout << "===== 测试构造函数 =====" << std::endl;
    
    // 默认构造函数
    mystl::vector<int> v1;
    std::cout << "v1.size() = " << v1.size() << ", v1.capacity() = " << v1.capacity() << std::endl;
    
    // 指定大小的构造函数
    mystl::vector<int> v2(5);
    std::cout << "v2.size() = " << v2.size() << ", v2.capacity() = " << v2.capacity() << std::endl;
    std::cout << "v2 内容: ";
    for (auto i : v2) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    // 指定大小和初始值的构造函数
    mystl::vector<int> v3(3, 10);
    std::cout << "v3.size() = " << v3.size() << ", v3.capacity() = " << v3.capacity() << std::endl;
    std::cout << "v3 内容: ";
    for (auto i : v3) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    // 迭代器范围构造函数
    int arr[] = {1, 2, 3, 4, 5};
    mystl::vector<int> v4(arr, arr + 5);
    std::cout << "v4.size() = " << v4.size() << ", v4.capacity() = " << v4.capacity() << std::endl;
    std::cout << "v4 内容: ";
    for (auto i : v4) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    // 拷贝构造函数
    mystl::vector<int> v5(v4);
    std::cout << "v5.size() = " << v5.size() << ", v5.capacity() = " << v5.capacity() << std::endl;
    std::cout << "v5 内容: ";
    for (auto i : v5) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    // 移动构造函数
    mystl::vector<int> v6(std::move(v5));
    std::cout << "v6.size() = " << v6.size() << ", v6.capacity() = " << v6.capacity() << std::endl;
    std::cout << "v6 内容: ";
    for (auto i : v6) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "v5.size() = " << v5.size() << ", v5.capacity() = " << v5.capacity() << std::endl;
    
    // 初始化列表构造函数
    mystl::vector<int> v7 = {10, 20, 30, 40};
    std::cout << "v7.size() = " << v7.size() << ", v7.capacity() = " << v7.capacity() << std::endl;
    std::cout << "v7 内容: ";
    for (auto i : v7) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief 测试vector的赋值函数
 */
void test_assignment() {
    std::cout << "\n===== 测试赋值函数 =====" << std::endl;
    
    mystl::vector<int> v1 = {1, 2, 3};
    mystl::vector<int> v2;
    
    // 拷贝赋值
    v2 = v1;
    std::cout << "v2 = v1 后，v2 内容: ";
    for (auto i : v2) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    // 移动赋值
    mystl::vector<int> v3;
    v3 = std::move(v2);
    std::cout << "v3 = std::move(v2) 后，v3 内容: ";
    for (auto i : v3) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "v2.size() = " << v2.size() << std::endl;
    
    // 初始化列表赋值
    mystl::vector<int> v4;
    v4 = {10, 20, 30, 40, 50};
    std::cout << "v4 = {10, 20, 30, 40, 50} 后，v4 内容: ";
    for (auto i : v4) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief 测试vector的容量相关函数
 */
void test_capacity() {
    std::cout << "\n===== 测试容量相关函数 =====" << std::endl;
    
    mystl::vector<int> v;
    std::cout << "初始状态：" << std::endl;
    std::cout << "size = " << v.size() << ", capacity = " << v.capacity() << ", empty = " << (v.empty() ? "true" : "false") << std::endl;
    
    // 添加元素
    v.push_back(10);
    v.push_back(20);
    std::cout << "添加两个元素后：" << std::endl;
    std::cout << "size = " << v.size() << ", capacity = " << v.capacity() << ", empty = " << (v.empty() ? "true" : "false") << std::endl;
    
    // 预留空间
    v.reserve(20);
    std::cout << "reserve(20)后：" << std::endl;
    std::cout << "size = " << v.size() << ", capacity = " << v.capacity() << ", empty = " << (v.empty() ? "true" : "false") << std::endl;
    
    // 收缩空间
    v.shrink_to_fit();
    std::cout << "shrink_to_fit()后：" << std::endl;
    std::cout << "size = " << v.size() << ", capacity = " << v.capacity() << ", empty = " << (v.empty() ? "true" : "false") << std::endl;
}

/**
 * @brief 测试vector的元素访问函数
 */
void test_access() {
    std::cout << "\n===== 测试元素访问函数 =====" << std::endl;
    
    mystl::vector<int> v = {10, 20, 30, 40, 50};
    
    std::cout << "v[2] = " << v[2] << std::endl;
    std::cout << "v.at(3) = " << v.at(3) << std::endl;
    std::cout << "v.front() = " << v.front() << std::endl;
    std::cout << "v.back() = " << v.back() << std::endl;
    
    // 越界访问测试
    try {
        std::cout << "尝试访问越界元素 v.at(10)..." << std::endl;
        v.at(10);
    } catch (const std::exception& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }
}

/**
 * @brief 测试vector的修改容器函数
 */
void test_modifiers() {
    std::cout << "\n===== 测试修改容器函数 =====" << std::endl;
    
    // 测试assign
    mystl::vector<int> v1;
    v1.assign(5, 10);
    std::cout << "v1.assign(5, 10) 后，v1 内容: ";
    for (auto i : v1) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    int arr[] = {100, 200, 300};
    v1.assign(arr, arr + 3);
    std::cout << "v1.assign(arr, arr+3) 后，v1 内容: ";
    for (auto i : v1) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    v1.assign({1, 2, 3, 4, 5});
    std::cout << "v1.assign({1,2,3,4,5}) 后，v1 内容: ";
    for (auto i : v1) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    // 测试push_back和pop_back
    mystl::vector<std::string> v2;
    v2.push_back("hello");
    v2.push_back("world");
    std::cout << "v2 内容: ";
    for (const auto& s : v2) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    
    v2.pop_back();
    std::cout << "v2.pop_back() 后，v2 内容: ";
    for (const auto& s : v2) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    
    // 测试insert
    mystl::vector<int> v3 = {10, 20, 30, 40};
    auto it = v3.begin() + 2;
    v3.insert(it, 25);
    std::cout << "v3.insert(it, 25) 后，v3 内容: ";
    for (auto i : v3) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    v3.insert(v3.begin(), 3, 5);
    std::cout << "v3.insert(v3.begin(), 3, 5) 后，v3 内容: ";
    for (auto i : v3) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    int arr2[] = {100, 200, 300};
    v3.insert(v3.end(), arr2, arr2 + 3);
    std::cout << "v3.insert(v3.end(), arr2, arr2 + 3) 后，v3 内容: ";
    for (auto i : v3) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    // 测试erase
    v3.erase(v3.begin());
    std::cout << "v3.erase(v3.begin()) 后，v3 内容: ";
    for (auto i : v3) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    v3.erase(v3.begin() + 2, v3.begin() + 5);
    std::cout << "v3.erase(v3.begin()+2, v3.begin()+5) 后，v3 内容: ";
    for (auto i : v3) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    // 测试clear
    v3.clear();
    std::cout << "v3.clear() 后，v3.size() = " << v3.size() << std::endl;
    
    // 测试resize
    mystl::vector<int> v4 = {1, 2, 3, 4, 5};
    v4.resize(3);
    std::cout << "v4.resize(3) 后，v4 内容: ";
    for (auto i : v4) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    v4.resize(6, 10);
    std::cout << "v4.resize(6, 10) 后，v4 内容: ";
    for (auto i : v4) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief 测试vector的比较操作符
 */
void test_comparison() {
    std::cout << "\n===== 测试比较操作符 =====" << std::endl;
    
    mystl::vector<int> v1 = {1, 2, 3};
    mystl::vector<int> v2 = {1, 2, 3};
    mystl::vector<int> v3 = {1, 2, 3, 4};
    mystl::vector<int> v4 = {1, 2, 4};
    
    std::cout << "v1 == v2: " << (v1 == v2 ? "true" : "false") << std::endl;
    std::cout << "v1 != v3: " << (v1 != v3 ? "true" : "false") << std::endl;
    std::cout << "v1 < v3: " << (v1 < v3 ? "true" : "false") << std::endl;
    std::cout << "v1 > v3: " << (v1 > v3 ? "true" : "false") << std::endl;
    std::cout << "v1 < v4: " << (v1 < v4 ? "true" : "false") << std::endl;
    std::cout << "v1 <= v2: " << (v1 <= v2 ? "true" : "false") << std::endl;
    std::cout << "v1 >= v2: " << (v1 >= v2 ? "true" : "false") << std::endl;
}

/**
 * @brief 测试vector的异常安全性
 */
void test_exception_safety() {
    std::cout << "\n===== 测试异常安全性 =====" << std::endl;
    
    // 这个类在复制时会抛出异常
    class ThrowOnCopy {
    public:
        ThrowOnCopy() = default;
        ThrowOnCopy(const ThrowOnCopy&) {
            static int count = 0;
            if (++count > 3) { // 第四次复制时抛出异常
                throw std::runtime_error("复制构造函数抛出异常");
            }
        }
        ThrowOnCopy& operator=(const ThrowOnCopy&) = default;
    };
    
    try {
        mystl::vector<ThrowOnCopy> v(3); // 创建3个元素
        std::cout << "成功创建含有3个ThrowOnCopy对象的vector" << std::endl;
        
        std::cout << "尝试push_back一个新元素（这将导致异常）..." << std::endl;
        for (int i = 0; i < 5; ++i) {
            v.push_back(ThrowOnCopy());  // 这将在某次复制时抛出异常
        }
    } catch (const std::exception& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }
}

/**
 * @brief 测试mystl::vector与std::vector的性能比较
 */
void test_performance() {
    std::cout << "\n===== 性能测试：mystl::vector vs std::vector =====" << std::endl;
    
    const int SMALL_TEST_SIZE = 10000;       // 小型测试数据量
    const int MEDIUM_TEST_SIZE = 100000;     // 中型测试数据量
    const int LARGE_TEST_SIZE = 1000000;     // 大型测试数据量
    
    std::cout << std::fixed << std::setprecision(6);  // 设置浮点数输出精度
    
    // 辅助函数：测量一个操作的执行时间（毫秒）
    auto time_operation = [](std::function<void()> operation) -> double {
        auto start = std::chrono::high_resolution_clock::now();
        operation();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    };
    
    // 辅助函数：显示性能测试结果
    auto show_results = [](const std::string& test_name, double std_time, double mystl_time) {
        std::cout << std::setw(30) << std::left << test_name 
                  << std::setw(15) << std::right << std_time << " ms"
                  << std::setw(15) << std::right << mystl_time << " ms"
                  << std::setw(15) << std::right 
                  << (mystl_time != 0 ? std_time / mystl_time : 0) << " x" 
                  << std::endl;
    };
    
    std::cout << "测试说明: 这个测试比较了mystl::vector和std::vector在相同操作下的性能差异\n";
    std::cout << "          比率 > 1 表示mystl::vector更快，比率 < 1 表示std::vector更快\n\n";
    
    std::cout << std::setw(30) << std::left << "测试项目" 
              << std::setw(15) << std::right << "std::vector" 
              << std::setw(15) << std::right << "mystl::vector" 
              << std::setw(15) << std::right << "性能比率" 
              << std::endl;
    std::cout << std::string(75, '-') << std::endl;
    
    // 测试1：构造函数性能（默认构造）
    {
        double std_time = time_operation([]() {
            std::vector<int> v;
        });
        
        double mystl_time = time_operation([]() {
            mystl::vector<int> v;
        });
        
        show_results("默认构造函数", std_time, mystl_time);
    }
    
    // 测试2：构造函数性能（带大小的构造）
    {
        double std_time = time_operation([&]() {
            std::vector<int> v(MEDIUM_TEST_SIZE);
        });
        
        double mystl_time = time_operation([&]() {
            mystl::vector<int> v(MEDIUM_TEST_SIZE);
        });
        
        show_results("带大小的构造函数", std_time, mystl_time);
    }
    
    // 测试3：push_back性能
    {
        double std_time = time_operation([&]() {
            std::vector<int> v;
            for (int i = 0; i < MEDIUM_TEST_SIZE; ++i) {
                v.push_back(i);
            }
        });
        
        double mystl_time = time_operation([&]() {
            mystl::vector<int> v;
            for (int i = 0; i < MEDIUM_TEST_SIZE; ++i) {
                v.push_back(i);
            }
        });
        
        show_results("push_back " + std::to_string(MEDIUM_TEST_SIZE) + " 个元素", std_time, mystl_time);
    }
    
    // 测试4：reserve + push_back性能
    {
        double std_time = time_operation([&]() {
            std::vector<int> v;
            v.reserve(MEDIUM_TEST_SIZE);
            for (int i = 0; i < MEDIUM_TEST_SIZE; ++i) {
                v.push_back(i);
            }
        });
        
        double mystl_time = time_operation([&]() {
            mystl::vector<int> v;
            v.reserve(MEDIUM_TEST_SIZE);
            for (int i = 0; i < MEDIUM_TEST_SIZE; ++i) {
                v.push_back(i);
            }
        });
        
        show_results("reserve + push_back", std_time, mystl_time);
    }
    
    // 测试5：随机访问性能
    {
        std::vector<int> std_v(LARGE_TEST_SIZE);
        mystl::vector<int> mystl_v(LARGE_TEST_SIZE);
        
        // 填充数据
        for (int i = 0; i < LARGE_TEST_SIZE; ++i) {
            std_v[i] = i;
            mystl_v[i] = i;
        }
        
        double std_time = time_operation([&]() {
            volatile int sum = 0;
            for (int i = 0; i < SMALL_TEST_SIZE; ++i) {
                int index = rand() % LARGE_TEST_SIZE;
                sum += std_v[index];
            }
        });
        
        double mystl_time = time_operation([&]() {
            volatile int sum = 0;
            for (int i = 0; i < SMALL_TEST_SIZE; ++i) {
                int index = rand() % LARGE_TEST_SIZE;
                sum += mystl_v[index];
            }
        });
        
        show_results("随机访问 " + std::to_string(SMALL_TEST_SIZE) + " 次", std_time, mystl_time);
    }
    
    // 测试6：遍历性能
    {
        std::vector<int> std_v(LARGE_TEST_SIZE);
        mystl::vector<int> mystl_v(LARGE_TEST_SIZE);
        
        // 填充数据
        for (int i = 0; i < LARGE_TEST_SIZE; ++i) {
            std_v[i] = i;
            mystl_v[i] = i;
        }
        
        double std_time = time_operation([&]() {
            volatile int sum = 0;
            for (const auto& val : std_v) {
                sum += val;
            }
        });
        
        double mystl_time = time_operation([&]() {
            volatile int sum = 0;
            for (const auto& val : mystl_v) {
                sum += val;
            }
        });
        
        show_results("遍历 " + std::to_string(LARGE_TEST_SIZE) + " 个元素", std_time, mystl_time);
    }
    
    // 测试7：插入性能（在中间插入）
    {
        std::vector<int> std_v(SMALL_TEST_SIZE);
        mystl::vector<int> mystl_v(SMALL_TEST_SIZE);
        
        double std_time = time_operation([&]() {
            for (int i = 0; i < 100; ++i) {
                std_v.insert(std_v.begin() + std_v.size()/2, i);
            }
        });
        
        double mystl_time = time_operation([&]() {
            for (int i = 0; i < 100; ++i) {
                mystl_v.insert(mystl_v.begin() + mystl_v.size()/2, i);
            }
        });
        
        show_results("中间插入 100 个元素", std_time, mystl_time);
    }
    
    // 测试8：删除性能（从中间删除）
    {
        std::vector<int> std_v(SMALL_TEST_SIZE);
        mystl::vector<int> mystl_v(SMALL_TEST_SIZE);
        
        double std_time = time_operation([&]() {
            for (int i = 0; i < 100 && !std_v.empty(); ++i) {
                std_v.erase(std_v.begin() + std_v.size()/2);
            }
        });
        
        double mystl_time = time_operation([&]() {
            for (int i = 0; i < 100 && !mystl_v.empty(); ++i) {
                mystl_v.erase(mystl_v.begin() + mystl_v.size()/2);
            }
        });
        
        show_results("中间删除 100 个元素", std_time, mystl_time);
    }
    
    // 测试9：resize性能
    {
        std::vector<int> std_v;
        mystl::vector<int> mystl_v;
        
        double std_time = time_operation([&]() {
            for (int i = 1; i <= 20; ++i) {
                std_v.resize(i * SMALL_TEST_SIZE / 20);
            }
        });
        
        double mystl_time = time_operation([&]() {
            for (int i = 1; i <= 20; ++i) {
                mystl_v.resize(i * SMALL_TEST_SIZE / 20);
            }
        });
        
        show_results("多次resize操作", std_time, mystl_time);
    }
    
    // 测试10：拷贝构造性能
    {
        std::vector<int> std_src(MEDIUM_TEST_SIZE);
        mystl::vector<int> mystl_src(MEDIUM_TEST_SIZE);
        
        // 填充数据
        for (int i = 0; i < MEDIUM_TEST_SIZE; ++i) {
            std_src[i] = i;
            mystl_src[i] = i;
        }
        
        double std_time = time_operation([&]() {
            std::vector<int> std_copy(std_src);
            volatile auto size = std_copy.size();  // 防止优化
        });
        
        double mystl_time = time_operation([&]() {
            mystl::vector<int> mystl_copy(mystl_src);
            volatile auto size = mystl_copy.size();  // 防止优化
        });
        
        show_results("拷贝构造", std_time, mystl_time);
    }
}

int main() {
    test_constructor();
    test_assignment();
    test_capacity();
    test_access();
    test_modifiers();
    test_comparison();
    test_exception_safety();
    test_performance();  // 添加性能测试
    
    return 0;
} 