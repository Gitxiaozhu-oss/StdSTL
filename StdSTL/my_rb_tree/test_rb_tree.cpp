#include <iostream>
#include <string>
#include <functional>
#include "my_rb_tree.h"

/**
 * @brief 测试红黑树基本功能
 */
void test_basic() {
    std::cout << "=== 测试红黑树基本功能 ===" << std::endl;
    
    // 创建一个int类型的红黑树
    mystl::rb_tree<int, std::less<int>> tree;
    
    // 测试插入和大小
    std::cout << "插入元素: 10, 7, 15, 5, 9, 13, 18" << std::endl;
    tree.insert_unique(10);
    tree.insert_unique(7);
    tree.insert_unique(15);
    tree.insert_unique(5);
    tree.insert_unique(9);
    tree.insert_unique(13);
    tree.insert_unique(18);
    
    std::cout << "树的大小: " << tree.size() << std::endl;
    
    std::cout << "遍历中序: ";
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试插入重复值，查看是否能成功插入
    auto result = tree.insert_unique(10);
    std::cout << "尝试插入重复值10: " << (result.second ? "成功" : "失败") << std::endl;
    
    // 测试插入并允许重复值
    std::cout << "允许重复值插入10: ";
    tree.insert_multi(10);
    std::cout << "现在树的大小: " << tree.size() << std::endl;
    
    std::cout << "遍历中序: ";
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief 测试插入重复键值
 */
void test_multi() {
    std::cout << "\n=== 测试插入重复键值 ===" << std::endl;
    
    // 创建一个允许重复键值的红黑树
    mystl::rb_tree<int, std::less<int>> tree;
    
    // 插入重复键值
    std::cout << "插入元素: 10, 7, 10, 5, 7, 10" << std::endl;
    tree.insert_multi(10);
    tree.insert_multi(7);
    tree.insert_multi(10);
    tree.insert_multi(5);
    tree.insert_multi(7);
    tree.insert_multi(10);
    
    std::cout << "树的大小: " << tree.size() << std::endl;
    
    // 测试遍历
    std::cout << "中序遍历: ";
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试统计
    std::cout << "键值为10的元素个数: " << tree.count_multi(10) << std::endl;
    
    // 测试范围查找
    std::cout << "查找键值为7的范围: ";
    auto range = tree.equal_range_multi(7);
    for (auto it = range.first; it != range.second; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试删除
    std::cout << "删除所有键值为10的元素" << std::endl;
    auto count = tree.erase_multi(10);
    std::cout << "删除了 " << count << " 个元素" << std::endl;
    std::cout << "树的大小: " << tree.size() << std::endl;
    
    std::cout << "中序遍历: ";
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief 测试复杂数据类型
 */
void test_complex_type() {
    std::cout << "\n=== 测试复杂数据类型 ===" << std::endl;
    
    // 创建一个存储pair的红黑树
    mystl::rb_tree<std::pair<int, std::string>, std::less<int>> tree;
    
    // 插入元素
    std::cout << "插入元素: (1,\"one\"), (2,\"two\"), (3,\"three\")" << std::endl;
    tree.insert_unique(std::make_pair(1, "one"));
    tree.insert_unique(std::make_pair(2, "two"));
    tree.insert_unique(std::make_pair(3, "three"));
    
    std::cout << "树的大小: " << tree.size() << std::endl;
    
    // 测试遍历
    std::cout << "中序遍历: ";
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << "(" << it->first << "," << it->second << ") ";
    }
    std::cout << std::endl;
    
    // 测试查找
    auto it = tree.find(2);
    if (it != tree.end()) {
        std::cout << "找到元素(2,\"" << it->second << "\")" << std::endl;
    } else {
        std::cout << "未找到元素2" << std::endl;
    }
}

/**
 * @brief 测试特性接口
 */
void test_interface() {
    std::cout << "\n=== 测试特性接口 ===" << std::endl;
    
    mystl::rb_tree<int, std::less<int>> tree;
    
    // 插入元素
    std::cout << "插入元素: 30, 15, 45, 10, 20, 40, 50" << std::endl;
    tree.insert_unique(30);
    tree.insert_unique(15);
    tree.insert_unique(45);
    tree.insert_unique(10);
    tree.insert_unique(20);
    tree.insert_unique(40);
    tree.insert_unique(50);
    
    // 测试边界查找
    auto lower = tree.lower_bound(25);
    auto upper = tree.upper_bound(25);
    
    std::cout << "25的lower_bound: " << *lower << std::endl;
    std::cout << "25的upper_bound: " << *upper << std::endl;
    
    // 测试范围
    std::cout << "范围[15, 45]: ";
    for (auto it = tree.lower_bound(15); it != tree.upper_bound(45); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试反向迭代器
    std::cout << "反向遍历: ";
    for (auto it = tree.rbegin(); it != tree.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试交换
    mystl::rb_tree<int, std::less<int>> tree2;
    tree2.insert_unique(100);
    tree2.insert_unique(200);
    
    std::cout << "交换前tree大小: " << tree.size() << ", tree2大小: " << tree2.size() << std::endl;
    swap(tree, tree2);
    std::cout << "交换后tree大小: " << tree.size() << ", tree2大小: " << tree2.size() << std::endl;
    
    std::cout << "交换后tree: ";
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

int main() {
    test_basic();
    test_multi();
    test_complex_type();
    test_interface();
    
    std::cout << "\n所有测试完成！" << std::endl;
    return 0;
} 