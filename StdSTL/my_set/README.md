# mystl::set/multiset

## 1. 整体架构概述

`mystl::set` 和 `mystl::multiset` 是基于红黑树实现的有序关联容器，分别对应标准库中的 `std::set` 和 `std::multiset`。这两个容器类提供了高效的插入、删除和查找操作，时间复杂度为 O(log n)。

```
┌─────────────────┐
│ mystl::set      │
│ mystl::multiset │
└────────┬────────┘
         │ 基于
         ▼
┌─────────────────┐
│  mystl::rb_tree │
└─────────────────┘
```

## 2. 主要特点

### mystl::set
- 键值即实值，不允许有重复键值
- 元素自动按键值排序
- 不允许修改已存在的元素值
- 插入、删除、查找操作的时间复杂度为 O(log n)

### mystl::multiset
- 键值即实值，允许有重复键值
- 元素自动按键值排序
- 不允许修改已存在的元素值
- 插入、删除、查找操作的时间复杂度为 O(log n)
- 提供更多针对重复元素的操作方法

## 3. 类的设计与层次结构

### 类模板定义
```cpp
template <class Key, class Compare = std::less<Key>>
class set { ... };

template <class Key, class Compare = std::less<Key>>
class multiset { ... };
```

### 内部成员概述
两个类内部都包含一个 `rb_tree` 成员：
```cpp
mystl::rb_tree<value_type, key_compare> tree_;
```

### 类型别名
```cpp
// 主要类型别名
typedef Key        key_type;       // 键值类型
typedef Key        value_type;     // 实值类型（键值即实值）
typedef Compare    key_compare;    // 键值比较函数
typedef Compare    value_compare;  // 实值比较函数

// 迭代器类型
typedef typename base_type::const_iterator         iterator;
typedef typename base_type::const_iterator         const_iterator;
typedef typename base_type::const_reverse_iterator reverse_iterator;
typedef typename base_type::const_reverse_iterator const_reverse_iterator;
```

## 4. 主要接口

### 构造函数和赋值运算符
```cpp
// 默认构造函数
set();
multiset();

// 迭代器范围构造函数
template <class InputIterator>
set(InputIterator first, InputIterator last);
template <class InputIterator>
multiset(InputIterator first, InputIterator last);

// 初始化列表构造函数
set(std::initializer_list<value_type> ilist);
multiset(std::initializer_list<value_type> ilist);

// 拷贝和移动构造函数
set(const set& rhs);
set(set&& rhs) noexcept;
multiset(const multiset& rhs);
multiset(multiset&& rhs) noexcept;

// 赋值运算符
set& operator=(const set& rhs);
set& operator=(set&& rhs) noexcept;
set& operator=(std::initializer_list<value_type> ilist);
multiset& operator=(const multiset& rhs);
multiset& operator=(multiset&& rhs) noexcept;
multiset& operator=(std::initializer_list<value_type> ilist);
```

### 迭代器相关
```cpp
iterator begin() noexcept;
const_iterator begin() const noexcept;
iterator end() noexcept;
const_iterator end() const noexcept;
reverse_iterator rbegin() noexcept;
const_reverse_iterator rbegin() const noexcept;
reverse_iterator rend() noexcept;
const_reverse_iterator rend() const noexcept;
const_iterator cbegin() const noexcept;
const_iterator cend() const noexcept;
const_reverse_iterator crbegin() const noexcept;
const_reverse_iterator crend() const noexcept;
```

### 容量相关
```cpp
bool empty() const noexcept;
size_type size() const noexcept;
size_type max_size() const noexcept;
```

### 插入操作
```cpp
// set 的插入操作
std::pair<iterator, bool> insert(const value_type& value);
std::pair<iterator, bool> insert(value_type&& value);
iterator insert(iterator hint, const value_type& value);
iterator insert(iterator hint, value_type&& value);
template <class InputIterator>
void insert(InputIterator first, InputIterator last);

// multiset 的插入操作
iterator insert(const value_type& value);
iterator insert(value_type&& value);
iterator insert(iterator hint, const value_type& value);
iterator insert(iterator hint, value_type&& value);
template <class InputIterator>
void insert(InputIterator first, InputIterator last);

// 原地构造元素
template <class... Args>
std::pair<iterator, bool> emplace(Args&&... args);  // set
template <class... Args>
iterator emplace(Args&&... args);  // multiset

template <class... Args>
iterator emplace_hint(iterator hint, Args&&... args);
```

### 删除操作
```cpp
void erase(iterator position);
size_type erase(const key_type& key);
void erase(iterator first, iterator last);
void clear();
```

### 查找操作
```cpp
iterator find(const key_type& key);
const_iterator find(const key_type& key) const;
size_type count(const key_type& key) const;
iterator lower_bound(const key_type& key);
const_iterator lower_bound(const key_type& key) const;
iterator upper_bound(const key_type& key);
const_iterator upper_bound(const key_type& key) const;
std::pair<iterator, iterator> equal_range(const key_type& key);
std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const;
```

### 其他操作
```cpp
void swap(set& rhs) noexcept;
void swap(multiset& rhs) noexcept;
key_compare key_comp() const;
value_compare value_comp() const;
allocator_type get_allocator() const;
```

## 5. 实现细节

### 红黑树作为底层实现
`set` 和 `multiset` 都是基于 `rb_tree` 红黑树实现的容器适配器，它们通过封装红黑树提供了更高级的接口。

### set 与 multiset 的区别
1. **插入行为**：
   - `set` 使用红黑树的 `insert_unique` 方法，确保元素不重复
   - `multiset` 使用红黑树的 `insert_multi` 方法，允许元素重复

2. **查询行为**：
   - `set` 使用 `count_unique` 和 `equal_range_unique` 方法
   - `multiset` 使用 `count_multi` 和 `equal_range_multi` 方法

### 迭代器特性
两个容器都使用红黑树的常量迭代器 `const_iterator`，这确保了容器中的元素在遍历过程中不会被修改，保持了红黑树的平衡性。

### 元素访问限制
由于使用常量迭代器，容器中的元素一旦插入后就不能被修改，这是通过迭代器类型的设计实现的：
```cpp
typedef typename base_type::const_iterator iterator;
typedef typename base_type::const_iterator const_iterator;
```

## 6. 移动语义支持

为了提高性能，`set` 和 `multiset` 都支持移动语义。移动语义可以避免不必要的深拷贝操作，特别是在容器元素较多时能显著提升性能。

最初的实现中，移动构造函数和移动赋值运算符在移动后没有将源对象置于有效状态，导致访问被移动对象时可能出现段错误。现在的实现通过调用 `rb_tree_init()` 确保源对象处于有效的空状态：

```cpp
// 修复后的移动构造函数处理流程
1. 移动资源（header_、node_count_、key_comp_）
2. 通过 reset() 函数将源对象重置为有效的空状态
```

## 7. 性能特性

### 时间复杂度
- 查找、插入、删除操作：O(log n)
- 遍历操作：O(n)

### 空间复杂度
- 存储 n 个元素需要 O(n) 的空间
- 每个节点包含数据和红黑树节点信息（颜色、指针等）

## 8. 使用示例

### 基本用法
```cpp
#include "my_set.h"
#include <iostream>

int main() {
    // 创建并初始化 set
    mystl::set<int> s = {5, 3, 7, 1, 9};
    
    // 插入元素
    s.insert(4);
    auto result = s.insert(3);  // 尝试插入重复元素
    std::cout << "插入成功: " << (result.second ? "是" : "否") << std::endl;
    
    // 遍历元素
    for (auto& x : s) {
        std::cout << x << " ";  // 输出有序结果：1 3 4 5 7 9
    }
    std::cout << std::endl;
    
    // 创建并初始化 multiset
    mystl::multiset<int> ms = {5, 3, 7, 3, 1, 5};
    
    // 统计元素
    std::cout << "3 的数量: " << ms.count(3) << std::endl;  // 输出：2
    
    // 遍历元素
    for (auto& x : ms) {
        std::cout << x << " ";  // 输出有序结果：1 3 3 5 5 7
    }
    std::cout << std::endl;
    
    return 0;
}
```

### 自定义比较器
```cpp
struct DescendingCompare {
    bool operator()(int a, int b) const {
        return a > b;  // 降序排列
    }
};

mystl::set<int, DescendingCompare> s = {1, 5, 3, 7, 9};
// 元素将按降序排列：9 7 5 3 1
```