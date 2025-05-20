#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <functional>
#include <string>
#include "my_rb_tree.h"

/**
 * 计时器类，用于测量函数执行时间
 */
class Timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::string operation_name;

public:
    Timer(const std::string& name) : operation_name(name) {
        start_time = std::chrono::high_resolution_clock::now();
    }

    ~Timer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << operation_name << " 耗时: " << duration << " ms" << std::endl;
    }
};

/**
 * 生成随机数据
 */
std::vector<int> generate_random_data(size_t size, int min_val = 0, int max_val = 1000000) {
    std::vector<int> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min_val, max_val);
    
    for (auto& val : data) {
        val = distrib(gen);
    }
    
    return data;
}

/**
 * 测试插入性能
 */
void test_insert_performance() {
    std::cout << "\n=== 测试插入性能 ===" << std::endl;
    
    // 测试不同大小的数据集
    const std::vector<size_t> sizes = {10000, 50000, 100000};
    
    for (auto size : sizes) {
        std::cout << "\n数据量: " << size << std::endl;
        
        // 随机数据
        auto random_data = generate_random_data(size);
        
        // 测试unique插入
        {
            mystl::rb_tree<int, std::less<int>> tree;
            Timer timer("唯一值插入(random)");
            for (const auto& val : random_data) {
                tree.insert_unique(val);
            }
            std::cout << "  树大小: " << tree.size() << std::endl;
        }
        
        // 测试multi插入
        {
            mystl::rb_tree<int, std::less<int>> tree;
            Timer timer("允许重复值插入(random)");
            for (const auto& val : random_data) {
                tree.insert_multi(val);
            }
            std::cout << "  树大小: " << tree.size() << std::endl;
        }
        
        // 有序数据
        std::vector<int> sorted_data(size);
        for (size_t i = 0; i < size; ++i) {
            sorted_data[i] = static_cast<int>(i);
        }
        
        // 测试有序数据的插入
        {
            mystl::rb_tree<int, std::less<int>> tree;
            Timer timer("唯一值插入(sorted)");
            for (const auto& val : sorted_data) {
                tree.insert_unique(val);
            }
            std::cout << "  树大小: " << tree.size() << std::endl;
        }
    }
}

/**
 * 测试查找性能
 */
void test_find_performance() {
    std::cout << "\n=== 测试查找性能 ===" << std::endl;
    
    const size_t size = 100000;
    auto data = generate_random_data(size);
    
    // 构建树
    mystl::rb_tree<int, std::less<int>> tree;
    for (const auto& val : data) {
        tree.insert_unique(val);
    }
    
    // 随机查找存在的元素
    {
        Timer timer("查找存在的元素(100000次)");
        size_t found_count = 0;
        for (const auto& val : data) {
            if (tree.find(val) != tree.end()) {
                ++found_count;
            }
        }
        std::cout << "  找到元素: " << found_count << std::endl;
    }
    
    // 随机查找不存在的元素
    {
        auto not_exist_data = generate_random_data(size, 1000001, 2000000);
        Timer timer("查找不存在的元素(100000次)");
        size_t found_count = 0;
        for (const auto& val : not_exist_data) {
            if (tree.find(val) != tree.end()) {
                ++found_count;
            }
        }
        std::cout << "  找到元素: " << found_count << std::endl;
    }
}

/**
 * 测试删除性能
 */
void test_erase_performance() {
    std::cout << "\n=== 测试删除性能 ===" << std::endl;
    
    const size_t size = 100000;
    auto data = generate_random_data(size);
    
    // 插入数据
    mystl::rb_tree<int, std::less<int>> tree;
    for (const auto& val : data) {
        tree.insert_unique(val);
    }
    
    // 随机顺序删除
    {
        auto erase_data = data;
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(erase_data.begin(), erase_data.end(), g);
        
        Timer timer("随机顺序删除");
        size_t erased_count = 0;
        for (const auto& val : erase_data) {
            erased_count += tree.erase_unique(val);
        }
        std::cout << "  删除元素: " << erased_count << std::endl;
        std::cout << "  剩余大小: " << tree.size() << std::endl;
    }
}

/**
 * 测试范围查询性能
 */
void test_range_query_performance() {
    std::cout << "\n=== 测试范围查询性能 ===" << std::endl;
    
    const size_t size = 100000;
    auto data = generate_random_data(size, 0, 1000);  // 使用较小的范围，增加重复概率
    
    // 构建允许重复键值的树
    mystl::rb_tree<int, std::less<int>> multi_tree;
    for (const auto& val : data) {
        multi_tree.insert_multi(val);
    }
    
    // 构建不允许重复键值的树
    mystl::rb_tree<int, std::less<int>> unique_tree;
    for (const auto& val : data) {
        unique_tree.insert_unique(val);
    }
    
    std::cout << "允许重复键值的树大小: " << multi_tree.size() << std::endl;
    std::cout << "不允许重复键值的树大小: " << unique_tree.size() << std::endl;
    
    // 测试equal_range_multi
    {
        Timer timer("equal_range_multi (1000次)");
        size_t total_range_size = 0;
        
        for (int i = 0; i < 1000; ++i) {
            int key = i;
            auto range = multi_tree.equal_range_multi(key);
            total_range_size += std::distance(range.first, range.second);
        }
        
        std::cout << "  平均范围大小: " << (total_range_size / 1000.0) << std::endl;
    }
    
    // 测试equal_range_unique
    {
        Timer timer("equal_range_unique (1000次)");
        size_t total_range_size = 0;
        
        for (int i = 0; i < 1000; ++i) {
            int key = i;
            auto range = unique_tree.equal_range_unique(key);
            total_range_size += std::distance(range.first, range.second);
        }
        
        std::cout << "  平均范围大小: " << (total_range_size / 1000.0) << std::endl;
    }
}

/**
 * 测试迭代器性能
 */
void test_iterator_performance() {
    std::cout << "\n=== 测试迭代器性能 ===" << std::endl;
    
    const size_t size = 100000;
    auto data = generate_random_data(size);
    
    // 构建树
    mystl::rb_tree<int, std::less<int>> tree;
    for (const auto& val : data) {
        tree.insert_unique(val);
    }
    
    // 测试正向遍历
    {
        Timer timer("正向遍历 (100000个元素)");
        size_t sum = 0;
        for (auto it = tree.begin(); it != tree.end(); ++it) {
            sum += *it;
        }
        std::cout << "  元素和: " << sum << std::endl;
    }
    
    // 测试反向遍历
    {
        Timer timer("反向遍历 (100000个元素)");
        size_t sum = 0;
        for (auto it = tree.rbegin(); it != tree.rend(); ++it) {
            sum += *it;
        }
        std::cout << "  元素和: " << sum << std::endl;
    }
}

int main() {
    std::cout << "===== 红黑树性能测试 =====" << std::endl;
    
    test_insert_performance();
    test_find_performance();
    test_erase_performance();
    test_range_query_performance();
    test_iterator_performance();
    
    std::cout << "\n性能测试完成！" << std::endl;
    return 0;
} 