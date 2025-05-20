// test_unordered_map.cpp
// 测试 unordered_map 和 unordered_multimap 容器的功能

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <ctime>

// 包含我们自己实现的 unordered_map 头文件
#include "my_unordered_map.h"

/**
 * @brief 测试 unordered_map 的基本功能
 */
void test_unordered_map_basic() {
    std::cout << "===== 测试 unordered_map 基本功能 =====" << std::endl;
    
    // 默认构造函数
    mystl::unordered_map<int, std::string> map1;
    assert(map1.empty());
    assert(map1.size() == 0);
    
    // 插入元素 - insert
    auto ret1 = map1.insert(std::make_pair(1, "一"));
    assert(ret1.second);
    assert(ret1.first->first == 1);
    assert(ret1.first->second == "一");
    
    // 重复插入
    auto ret2 = map1.insert(std::make_pair(1, "一一"));
    assert(!ret2.second); // 不应该插入成功
    assert(map1.size() == 1);
    
    // 使用 operator[] 插入
    map1[2] = "二";
    map1[3] = "三";
    assert(map1.size() == 3);
    
    // 使用 at() 访问
    assert(map1.at(1) == "一");
    assert(map1.at(2) == "二");
    assert(map1.at(3) == "三");
    
    // 使用 operator[] 修改
    map1[2] = "二二";
    assert(map1[2] == "二二");
    
    // 查找功能
    auto it = map1.find(2);
    assert(it != map1.end());
    assert(it->second == "二二");
    
    it = map1.find(4);
    assert(it == map1.end());
    
    // count 方法
    assert(map1.count(1) == 1);
    assert(map1.count(4) == 0);
    
    // 删除元素
    size_t removed = map1.erase(1);
    assert(removed == 1);
    assert(map1.size() == 2);
    assert(map1.count(1) == 0);
    
    // 使用迭代器删除
    it = map1.find(2);
    map1.erase(it);
    assert(map1.size() == 1);
    
    // 清空容器
    map1.clear();
    assert(map1.empty());
    
    std::cout << "unordered_map 基本功能测试通过!" << std::endl;
}

/**
 * @brief 测试 unordered_map 的高级功能
 */
void test_unordered_map_advanced() {
    std::cout << "===== 测试 unordered_map 高级功能 =====" << std::endl;
    
    // 使用初始化列表构造
    mystl::unordered_map<std::string, int> map1 = {
        {"apple", 1},
        {"banana", 2},
        {"orange", 3}
    };
    assert(map1.size() == 3);
    
    // emplace 功能
    auto ret = map1.emplace("grape", 4);
    assert(ret.second);
    assert(ret.first->second == 4);
    
    // emplace_hint 功能
    auto it = map1.find("apple");
    auto it2 = map1.emplace_hint(it, "peach", 5);
    assert(it2->first == "peach");
    assert(it2->second == 5);
    
    // 移动构造
    mystl::unordered_map<std::string, int> map2(std::move(map1));
    assert(map2.size() == 5);
    assert(map1.size() == 0); // 被移动后应该为空
    
    // 复制构造
    mystl::unordered_map<std::string, int> map3(map2);
    assert(map3.size() == 5);
    
    // 遍历容器
    int sum = 0;
    for (const auto& pair : map3) {
        sum += pair.second;
    }
    assert(sum == 15); // 1+2+3+4+5=15
    
    // 桶接口
    std::cout << "桶数量: " << map3.bucket_count() << std::endl;
    std::cout << "负载因子: " << map3.load_factor() << std::endl;
    
    // rehash 测试
    size_t old_bucket_count = map3.bucket_count();
    map3.rehash(100);
    assert(map3.bucket_count() >= 100);
    
    // 保留空间
    map3.reserve(200);
    assert(map3.bucket_count() >= 200);
    
    // 交换容器
    mystl::unordered_map<std::string, int> map4 = {{"test", 100}};
    map3.swap(map4);
    assert(map3.size() == 1);
    assert(map4.size() == 5);
    
    std::cout << "unordered_map 高级功能测试通过!" << std::endl;
}

/**
 * @brief 测试 unordered_multimap 的功能
 */
void test_unordered_multimap() {
    std::cout << "===== 测试 unordered_multimap 功能 =====" << std::endl;
    
    // 默认构造函数
    mystl::unordered_multimap<int, std::string> mmap;
    
    // 插入元素
    auto it1 = mmap.insert(std::make_pair(1, "一"));
    auto it2 = mmap.insert(std::make_pair(2, "二"));
    auto it3 = mmap.insert(std::make_pair(3, "三"));
    
    // 插入重复键
    auto it4 = mmap.insert(std::make_pair(1, "一一"));
    auto it5 = mmap.insert(std::make_pair(1, "一二"));
    
    // 检查元素数量
    assert(mmap.size() == 5);
    
    // 查找元素
    auto range = mmap.equal_range(1);
    size_t count = 0;
    for (auto it = range.first; it != range.second; ++it) {
        count++;
    }
    assert(count == 3); // 键为1的元素应该有3个
    
    // count 方法
    assert(mmap.count(1) == 3);
    assert(mmap.count(2) == 1);
    
    // 使用迭代器删除
    auto range1 = mmap.equal_range(1);
    for (auto it = range1.first; it != range1.second;) {
        auto temp = it;
        ++it;
        mmap.erase(temp);
    }
    
    // 验证删除结果
    assert(mmap.count(1) == 0);
    assert(mmap.size() == 2);
    
    // 通过迭代器插入
    std::vector<std::pair<const int, std::string>> v = {
        {4, "四"}, {5, "五"}, {6, "六"}
    };
    mmap.insert(v.begin(), v.end());
    assert(mmap.size() == 5);
    
    // emplace 方法
    auto it6 = mmap.emplace(7, "七");
    auto it7 = mmap.emplace(7, "七七");
    assert(mmap.count(7) == 2);
    
    // 清空容器
    mmap.clear();
    assert(mmap.empty());
    
    std::cout << "unordered_multimap 功能测试通过!" << std::endl;
}

/**
 * @brief 测试异常安全性
 */
void test_exception_safety() {
    std::cout << "===== 测试异常安全性 =====" << std::endl;
    
    mystl::unordered_map<int, std::string> map;
    
    try {
        // 访问不存在的键
        map.at(1); // 应该抛出异常
        assert(false); // 不应该执行到这里
    } catch (const std::out_of_range& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }
    
    std::cout << "异常安全性测试通过!" << std::endl;
}

/**
 * @brief 性能测试
 */
void test_performance() {
    std::cout << "===== 性能测试 =====" << std::endl;
    
    const int COUNT = 10000;
    mystl::unordered_map<int, int> map;
    
    // 测试插入性能
    clock_t start = clock();
    for (int i = 0; i < COUNT; i++) {
        map[i] = i;
    }
    clock_t end = clock();
    double insert_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "插入 " << COUNT << " 个元素耗时: " << insert_time << "秒" << std::endl;
    
    // 测试查找性能
    start = clock();
    for (int i = 0; i < COUNT; i++) {
        auto it = map.find(i);
        assert(it != map.end());
    }
    end = clock();
    double find_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "查找 " << COUNT << " 个元素耗时: " << find_time << "秒" << std::endl;
    
    std::cout << "性能测试完成!" << std::endl;
}

int main() {
    std::cout << "开始测试 unordered_map 和 unordered_multimap..." << std::endl;
    
    test_unordered_map_basic();
    test_unordered_map_advanced();
    test_unordered_multimap();
    test_exception_safety();
    test_performance();
    
    std::cout << "所有测试完成，功能正常!" << std::endl;
    
    return 0;
}