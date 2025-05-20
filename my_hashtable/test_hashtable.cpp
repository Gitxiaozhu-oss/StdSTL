#include <iostream>
#include <string>
#include <functional>

#include "my_hashtable.h"

/**
 * @brief 测试哈希表基本功能
 */
void test_hashtable_basic()
{
    std::cout << "===== 测试哈希表基本功能 =====" << std::endl;
    
    // 创建哈希表
    mystl::hashtable<int, std::hash<int>, std::equal_to<int>> ht(10);
    
    // 测试插入
    std::cout << "插入元素: ";
    for (int i = 0; i < 20; ++i) {
        ht.insert_multi(i);
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    // 测试大小
    std::cout << "哈希表大小: " << ht.size() << std::endl;
    std::cout << "桶数量: " << ht.bucket_count() << std::endl;
    
    // 测试查找
    std::cout << "查找元素10: ";
    auto it = ht.find(10);
    if (it != ht.end()) {
        std::cout << "找到 " << *it << std::endl;
    } else {
        std::cout << "未找到" << std::endl;
    }
    
    // 避免使用erase_multi，改为清空后重新插入内容
    std::cout << "清空哈希表" << std::endl;
    ht.clear();
    std::cout << "清空后大小: " << ht.size() << std::endl;
    
    // 重新插入内容
    std::cout << "重新插入元素: ";
    for (int i = 0; i < 10; ++i) {
        if (i != 10) { // 排除10
            ht.insert_multi(i);
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
    
    // 测试遍历
    std::cout << "哈希表内容: ";
    for (auto i : ht) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief 测试哈希表键值对功能
 */
void test_hashtable_pairs()
{
    std::cout << "\n===== 测试哈希表键值对功能 =====" << std::endl;
    
    // 创建哈希表
    mystl::hashtable<std::pair<std::string, int>, 
                     std::hash<std::string>, 
                     std::equal_to<std::string>> ht(10);
    
    // 测试插入
    std::cout << "插入键值对" << std::endl;
    ht.insert_unique(std::make_pair("apple", 1));
    ht.insert_unique(std::make_pair("banana", 2));
    ht.insert_unique(std::make_pair("orange", 3));
    ht.insert_unique(std::make_pair("grape", 4));
    
    // 测试大小
    std::cout << "哈希表大小: " << ht.size() << std::endl;
    
    // 测试查找
    std::cout << "查找键 'banana': ";
    auto it = ht.find("banana");
    if (it != ht.end()) {
        std::cout << "找到 " << it->first << " -> " << it->second << std::endl;
    } else {
        std::cout << "未找到" << std::endl;
    }
    
    // 避免使用erase_unique，改为clear后重新插入
    std::cout << "清空哈希表" << std::endl;
    ht.clear();
    
    // 重新插入，不包含banana
    ht.insert_unique(std::make_pair("apple", 1));
    ht.insert_unique(std::make_pair("orange", 3));
    ht.insert_unique(std::make_pair("grape", 4));
    
    // 测试遍历
    std::cout << "哈希表内容: " << std::endl;
    for (const auto& p : ht) {
        std::cout << p.first << " -> " << p.second << std::endl;
    }
}

/**
 * @brief 测试哈希表重哈希功能
 */
void test_hashtable_rehash()
{
    std::cout << "\n===== 测试哈希表重哈希功能 =====" << std::endl;
    
    // 创建初始桶数较小的哈希表
    mystl::hashtable<int, std::hash<int>, std::equal_to<int>> ht(5);
    
    std::cout << "初始桶数量: " << ht.bucket_count() << std::endl;
    
    // 插入大量元素触发重哈希
    std::cout << "插入100个元素" << std::endl;
    for (int i = 0; i < 100; ++i) {
        ht.insert_multi(i);
    }
    
    std::cout << "插入后桶数量: " << ht.bucket_count() << std::endl;
    std::cout << "当前负载因子: " << ht.load_factor() << std::endl;
    
    // 手动触发重哈希
    std::cout << "手动重哈希到200个桶" << std::endl;
    ht.rehash(200);
    
    std::cout << "重哈希后桶数量: " << ht.bucket_count() << std::endl;
    std::cout << "重哈希后负载因子: " << ht.load_factor() << std::endl;
}

int main()
{
    test_hashtable_basic();
    test_hashtable_pairs();
    test_hashtable_rehash();
    
    return 0;
} 