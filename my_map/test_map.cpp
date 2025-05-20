#include "my_map.h"
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

/**
 * @brief 测试map和multimap容器的功能
 * 
 * 本测试程序涵盖以下功能：
 * 1. 基本构造函数测试
 * 2. 元素插入和访问
 * 3. 元素查找和删除
 * 4. 迭代器相关操作
 * 5. 比较操作
 * 6. multimap特有功能
 */

// 打印map中的所有元素
template <typename Map>
void print_map(const Map& m, const std::string& name) {
    std::cout << name << " 内容 (" << m.size() << " 个元素):" << std::endl;
    for (auto& pair : m) {
        std::cout << "  " << pair.first << " -> " << pair.second << std::endl;
    }
    std::cout << std::endl;
}

// 测试map的基本功能
void test_map_basic() {
    std::cout << "===== 测试 map 基本功能 =====" << std::endl;

    // 默认构造函数
    my::map<int, std::string> m1;
    assert(m1.empty());
    assert(m1.size() == 0);

    // 插入元素
    m1.insert({1, "一"});
    m1.insert({3, "三"});
    m1.insert({5, "五"});
    
    assert(!m1.empty());
    assert(m1.size() == 3);
    print_map(m1, "m1");

    // 使用初始化列表构造
    my::map<int, std::string> m2 = {
        {2, "二"},
        {4, "四"},
        {6, "六"}
    };
    assert(m2.size() == 3);
    print_map(m2, "m2");

    // 测试拷贝构造
    my::map<int, std::string> m3(m1);
    assert(m3.size() == m1.size());
    print_map(m3, "m3 (m1的拷贝)");

    // 测试移动构造
    my::map<int, std::string> m4(std::move(m3));
    assert(m4.size() == 3);
    assert(m3.empty());  // m3应该被移动后为空
    print_map(m4, "m4 (m3的移动)");

    // 测试赋值操作
    my::map<int, std::string> m5;
    m5 = m2;
    assert(m5.size() == m2.size());
    print_map(m5, "m5 (m2的赋值)");

    // 测试[]操作符
    m5[7] = "七";
    assert(m5.size() == 4);
    assert(m5[7] == "七");

    // 测试at方法
    assert(m5.at(4) == "四");
    
    // 测试异常情况
    bool exception_caught = false;
    try {
        m5.at(9);  // 不存在的键
    } catch (const std::out_of_range&) {
        exception_caught = true;
    }
    assert(exception_caught);

    std::cout << "map基本功能测试通过!" << std::endl << std::endl;
}

// 测试map的查找和删除功能
void test_map_find_erase() {
    std::cout << "===== 测试 map 查找和删除功能 =====" << std::endl;
    
    my::map<int, std::string> m = {
        {1, "一"},
        {2, "二"},
        {3, "三"},
        {4, "四"},
        {5, "五"}
    };
    print_map(m, "原始map");

    // 测试find
    auto it = m.find(3);
    assert(it != m.end());
    assert(it->first == 3);
    assert(it->second == "三");

    auto it2 = m.find(6);
    assert(it2 == m.end());

    // 测试count
    assert(m.count(2) == 1);
    assert(m.count(6) == 0);

    // 测试lower_bound和upper_bound
    auto lower = m.lower_bound(3);
    assert(lower->first == 3);

    auto upper = m.upper_bound(3);
    assert(upper->first == 4);

    // 测试equal_range
    auto range = m.equal_range(3);
    assert(range.first->first == 3);
    assert(range.second->first == 4);

    // 测试删除单个元素
    m.erase(it);  // 删除键为3的元素
    assert(m.size() == 4);
    assert(m.find(3) == m.end());
    print_map(m, "删除键为3后的map");

    // 测试删除范围
    m.erase(m.find(2), m.find(5));
    assert(m.size() == 2);
    print_map(m, "删除键2到5之间的元素后的map");

    // 测试清空
    m.clear();
    assert(m.empty());
    print_map(m, "清空后的map");

    std::cout << "map查找和删除功能测试通过!" << std::endl << std::endl;
}

// 测试map的迭代器功能
void test_map_iterators() {
    std::cout << "===== 测试 map 迭代器功能 =====" << std::endl;
    
    my::map<int, std::string> m = {
        {1, "一"},
        {2, "二"},
        {3, "三"},
        {4, "四"},
        {5, "五"}
    };

    // 正向迭代器
    std::cout << "正向迭代:" << std::endl;
    for (auto it = m.begin(); it != m.end(); ++it) {
        std::cout << "  " << it->first << " -> " << it->second << std::endl;
    }

    // 反向迭代器
    std::cout << "反向迭代:" << std::endl;
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
        std::cout << "  " << it->first << " -> " << it->second << std::endl;
    }

    // const迭代器
    const my::map<int, std::string>& cm = m;
    std::cout << "const迭代器:" << std::endl;
    for (auto it = cm.cbegin(); it != cm.cend(); ++it) {
        std::cout << "  " << it->first << " -> " << it->second << std::endl;
    }

    std::cout << "map迭代器功能测试通过!" << std::endl << std::endl;
}

// 测试multimap的特有功能
void test_multimap() {
    std::cout << "===== 测试 multimap 功能 =====" << std::endl;
    
    my::multimap<int, std::string> mm;
    
    // 插入多个相同键的元素
    mm.insert({1, "一"});
    mm.insert({1, "一一"});
    mm.insert({1, "一二"});
    mm.insert({2, "二"});
    mm.insert({3, "三"});

    print_map(mm, "multimap");
    assert(mm.size() == 5);
    assert(mm.count(1) == 3);

    // 测试相同键的迭代
    std::cout << "键为1的所有元素:" << std::endl;
    auto range = mm.equal_range(1);
    for (auto it = range.first; it != range.second; ++it) {
        std::cout << "  " << it->first << " -> " << it->second << std::endl;
    }

    // 测试删除
    size_t removed = mm.erase(1);
    assert(removed == 3);  // 应当删除了3个元素
    assert(mm.count(1) == 0);
    print_map(mm, "删除键为1后的multimap");

    std::cout << "multimap功能测试通过!" << std::endl << std::endl;
}

// 测试比较操作
void test_comparison() {
    std::cout << "===== 测试比较操作 =====" << std::endl;
    
    my::map<int, std::string> m1 = {
        {1, "一"},
        {2, "二"},
        {3, "三"}
    };
    
    my::map<int, std::string> m2 = {
        {1, "一"},
        {2, "二"},
        {3, "三"},
        {4, "四"}
    };
    
    my::map<int, std::string> m3 = {
        {1, "一"},
        {2, "二"},
        {3, "三"}
    };
    
    my::map<int, std::string> m4 = {
        {1, "一"},
        {2, "贰"},  // 值不同
        {3, "三"}
    };

    assert(m1 == m3);
    assert(m1 != m2);
    assert(m1 != m4);
    assert(m1 < m2);  // m1比m2少元素
    assert(m2 > m1);
    assert(m1 <= m3);
    assert(m1 >= m3);

    std::cout << "比较操作测试通过!" << std::endl << std::endl;
}

int main() {
    std::cout << "开始测试my::map和my::multimap实现..." << std::endl << std::endl;
    
    test_map_basic();
    test_map_find_erase();
    test_map_iterators();
    test_multimap();
    test_comparison();
    
    std::cout << "所有测试通过！my::map和my::multimap实现符合预期！" << std::endl;
    return 0;
} 