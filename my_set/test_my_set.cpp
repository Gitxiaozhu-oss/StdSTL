#include "my_set.h"
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief 测试函数：打印set的内容
 * @tparam T 元素类型
 * @param s set容器
 * @param name 容器名称
 */
template <typename T>
void print_set(const mystl::set<T>& s, const std::string& name) {
    std::cout << name << "的内容: ";
    for (auto& x : s) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    std::cout << "大小: " << s.size() << std::endl;
    std::cout << "是否为空: " << (s.empty() ? "是" : "否") << std::endl;
    std::cout << std::endl;
}

/**
 * @brief 测试函数：打印multiset的内容
 * @tparam T 元素类型
 * @param ms multiset容器
 * @param name 容器名称
 */
template <typename T>
void print_multiset(const mystl::multiset<T>& ms, const std::string& name) {
    std::cout << name << "的内容: ";
    for (auto& x : ms) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    std::cout << "大小: " << ms.size() << std::endl;
    std::cout << "是否为空: " << (ms.empty() ? "是" : "否") << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "===== 测试set容器 =====" << std::endl;
    
    // 测试构造函数
    mystl::set<int> s1;
    print_set(s1, "s1 (默认构造)");
    
    // 测试插入操作
    s1.insert(10);
    s1.insert(20);
    s1.insert(30);
    s1.insert(40);
    print_set(s1, "s1 (插入元素后)");
    
    // 测试重复插入
    auto result = s1.insert(10);
    std::cout << "尝试插入重复元素10，成功: " << (result.second ? "是" : "否") << std::endl;
    print_set(s1, "s1 (尝试插入重复元素后)");
    
    // 测试initializer_list构造
    mystl::set<int> s2 = {5, 10, 15, 20, 25};
    print_set(s2, "s2 (初始化列表构造)");
    
    // 测试拷贝构造
    mystl::set<int> s3(s2);
    print_set(s3, "s3 (拷贝s2)");
    
    // 测试移动构造
    mystl::set<int> s4(std::move(s3));
    print_set(s4, "s4 (移动s3)");
    print_set(s3, "s3 (被移动后)");
    
    // 测试查找操作
    auto it = s4.find(15);
    if (it != s4.end()) {
        std::cout << "在s4中找到元素15" << std::endl;
    } else {
        std::cout << "在s4中未找到元素15" << std::endl;
    }
    
    it = s4.find(50);
    if (it != s4.end()) {
        std::cout << "在s4中找到元素50" << std::endl;
    } else {
        std::cout << "在s4中未找到元素50" << std::endl;
    }
    
    // 测试迭代器
    std::cout << "使用迭代器遍历s4: ";
    for (auto it = s4.begin(); it != s4.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试反向迭代器
    std::cout << "使用反向迭代器遍历s4: ";
    for (auto it = s4.rbegin(); it != s4.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试erase操作
    s4.erase(15);
    print_set(s4, "s4 (删除元素15后)");
    
    // 测试范围删除
    s4.erase(s4.begin(), std::next(s4.begin(), 2));
    print_set(s4, "s4 (删除前两个元素后)");
    
    // 测试lower_bound和upper_bound
    mystl::set<int> s5 = {10, 20, 30, 40, 50};
    auto lower = s5.lower_bound(25);
    auto upper = s5.upper_bound(25);
    
    std::cout << "s5中大于等于25的第一个元素: " << *lower << std::endl;
    std::cout << "s5中大于25的第一个元素: " << *upper << std::endl;
    
    // 测试equal_range
    auto range = s5.equal_range(30);
    std::cout << "s5中等于30的范围: ";
    for (auto it = range.first; it != range.second; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试count
    std::cout << "s5中元素30的数量: " << s5.count(30) << std::endl;
    std::cout << "s5中元素35的数量: " << s5.count(35) << std::endl;
    
    // 测试比较操作符
    mystl::set<int> s6 = {10, 20, 30};
    mystl::set<int> s7 = {10, 20, 30};
    mystl::set<int> s8 = {10, 20, 30, 40};
    
    std::cout << "s6 == s7: " << (s6 == s7 ? "是" : "否") << std::endl;
    std::cout << "s6 != s8: " << (s6 != s8 ? "是" : "否") << std::endl;
    std::cout << "s6 < s8: " << (s6 < s8 ? "是" : "否") << std::endl;
    
    // 测试交换操作
    std::cout << "交换前:" << std::endl;
    print_set(s6, "s6");
    print_set(s8, "s8");
    
    s6.swap(s8);
    
    std::cout << "交换后:" << std::endl;
    print_set(s6, "s6");
    print_set(s8, "s8");
    
    // 测试清空操作
    s6.clear();
    print_set(s6, "s6 (清空后)");
    
    std::cout << "\n===== 测试multiset容器 =====" << std::endl;
    
    // 测试构造函数
    mystl::multiset<int> ms1;
    print_multiset(ms1, "ms1 (默认构造)");
    
    // 测试插入操作
    ms1.insert(10);
    ms1.insert(20);
    ms1.insert(10); // 允许重复
    ms1.insert(30);
    print_multiset(ms1, "ms1 (插入元素后)");
    
    // 测试count_multi
    std::cout << "ms1中元素10的数量: " << ms1.count(10) << std::endl;
    std::cout << "ms1中元素20的数量: " << ms1.count(20) << std::endl;
    std::cout << "ms1中元素50的数量: " << ms1.count(50) << std::endl;
    
    // 测试initializer_list构造
    mystl::multiset<int> ms2 = {5, 10, 5, 20, 10, 15};
    print_multiset(ms2, "ms2 (初始化列表构造)");
    
    // 测试移动构造
    mystl::multiset<int> ms3(ms2);
    print_multiset(ms3, "ms3 (拷贝ms2)");
    
    mystl::multiset<int> ms4(std::move(ms3));
    print_multiset(ms4, "ms4 (移动ms3)");
    print_multiset(ms3, "ms3 (被移动后)");
    
    // 测试equal_range_multi
    auto multi_range = ms2.equal_range(10);
    std::cout << "ms2中等于10的范围: ";
    for (auto it = multi_range.first; it != multi_range.second; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试erase_multi
    size_t erased = ms2.erase(5);
    std::cout << "从ms2中删除元素5，共删除: " << erased << " 个元素" << std::endl;
    print_multiset(ms2, "ms2 (删除元素5后)");
    
    std::cout << "测试完成!" << std::endl;
    return 0;
} 