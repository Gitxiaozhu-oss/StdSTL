# `unordered_set.h` 

## 1. 概述

`unordered_set.h` 是一个基于哈希表实现的无序集合容器，提供了 `unordered_set` 和 `unordered_multiset` 两个模板类。这两个类与标准库中的同名容器功能类似，但这是一个优化过的自定义实现。本文档旨在帮助开发者快速理解这份代码的设计思路、核心实现和使用方法。

## 2. 设计思路

### 2.1 基本原理

- **哈希表基础**：采用开链法（链地址法）解决哈希冲突
- **底层实现**：依赖 `my_hashtable/my_hashtable.h` 实现核心功能
- **标准接口**：提供与 C++ 标准库一致的接口，确保兼容性

### 2.2 两种容器类型

1. **`unordered_set`**：不允许键值重复的无序集合
2. **`unordered_multiset`**：允许键值重复的无序集合

## 3. 类结构与成员

### 3.1 公共类型定义

两个容器类都基于 `hashtable` 实现，并继承了以下类型定义：

```cpp
typedef typename base_type::allocator_type       allocator_type;
typedef typename base_type::key_type             key_type;
typedef typename base_type::value_type           value_type;
typedef typename base_type::hasher               hasher;
typedef typename base_type::key_equal            key_equal;
typedef typename base_type::size_type            size_type;
typedef typename base_type::difference_type      difference_type;
typedef typename base_type::pointer              pointer;
typedef typename base_type::const_pointer        const_pointer;
typedef typename base_type::reference            reference;
typedef typename base_type::const_reference      const_reference;
typedef typename base_type::const_iterator       iterator;
typedef typename base_type::const_iterator       const_iterator;
typedef typename base_type::const_local_iterator local_iterator;
typedef typename base_type::const_local_iterator const_local_iterator;
```

### 3.2 主要成员变量

- `base_type ht_`：底层哈希表实例，所有操作都委托给此实例

## 4. 主要功能接口

### 4.1 构造函数

```cpp
// 默认构造函数
unordered_set() noexcept;

// 指定桶数量的构造函数
explicit unordered_set(size_type bucket_count,
                     const Hash& hash = Hash(),
                     const KeyEqual& equal = KeyEqual()) noexcept;

// 从迭代器范围构造
template <class InputIterator>
unordered_set(InputIterator first, InputIterator last,
            const size_type bucket_count = 100,
            const Hash& hash = Hash(),
            const KeyEqual& equal = KeyEqual());

// 从初始化列表构造
unordered_set(std::initializer_list<value_type> ilist,
            const size_type bucket_count = 100,
            const Hash& hash = Hash(),
            const KeyEqual& equal = KeyEqual());

// 拷贝构造函数
unordered_set(const unordered_set& rhs);

// 移动构造函数
unordered_set(unordered_set&& rhs) noexcept;
```

### 4.2 迭代器相关

```cpp
iterator       begin()        noexcept;
const_iterator begin()  const noexcept;
iterator       end()          noexcept;
const_iterator end()    const noexcept;
const_iterator cbegin() const noexcept;
const_iterator cend()   const noexcept;
```

### 4.3 容量相关

```cpp
bool      empty()    const noexcept;
size_type size()     const noexcept;
size_type max_size() const noexcept;
```

### 4.4 修改容器操作

```cpp
// 插入元素
template <class ...Args>
pair<iterator, bool> emplace(Args&& ...args);

template <class ...Args>
iterator emplace_hint(const_iterator hint, Args&& ...args);

pair<iterator, bool> insert(const value_type& value);
pair<iterator, bool> insert(value_type&& value);
iterator insert(const_iterator hint, const value_type& value);
iterator insert(const_iterator hint, value_type&& value);
template <class InputIterator>
void insert(InputIterator first, InputIterator last);

// 删除元素
void erase(iterator it);
void erase(iterator first, iterator last);
size_type erase(const key_type& key);
void clear();
void swap(unordered_set& other) noexcept;
```

### 4.5 查找相关

```cpp
size_type count(const key_type& key) const;
iterator find(const key_type& key);
const_iterator find(const key_type& key) const;
pair<iterator, iterator> equal_range(const key_type& key);
pair<const_iterator, const_iterator> equal_range(const key_type& key) const;
```

### 4.6 桶接口

```cpp
local_iterator begin(size_type n) noexcept;
const_local_iterator begin(size_type n) const noexcept;
const_local_iterator cbegin(size_type n) const noexcept;
local_iterator end(size_type n) noexcept;
const_local_iterator end(size_type n) const noexcept;
const_local_iterator cend(size_type n) const noexcept;
size_type bucket_count() const noexcept;
size_type max_bucket_count() const noexcept;
size_type bucket_size(size_type n) const noexcept;
size_type bucket(const key_type& key) const;
```

### 4.7 哈希策略

```cpp
float load_factor() const noexcept;
float max_load_factor() const noexcept;
void max_load_factor(float ml);
void rehash(size_type count);
void reserve(size_type count);
hasher hash_fcn() const;
key_equal key_eq() const;
```

## 5. `unordered_set` 与 `unordered_multiset` 的区别

### 5.1 插入操作

- `unordered_set` 使用 `insert_unique_noresize` 和 `insert_unique`，确保键值不重复
- `unordered_multiset` 使用 `insert_multi_noresize` 和 `insert_multi`，允许键值重复

### 5.2 删除操作

- `unordered_set` 使用 `erase_unique`，最多删除一个元素
- `unordered_multiset` 使用 `erase_multi`，可能删除多个元素

### 5.3 范围查找

- `unordered_set` 使用 `equal_range_unique`，返回范围最多包含一个元素
- `unordered_multiset` 使用 `equal_range_multi`，返回可能包含多个元素的范围

## 6. 性能特点

### 6.1 时间复杂度

- 平均情况：
  - 查找、插入、删除：O(1)
- 最坏情况：
  - 查找、插入、删除：O(n)，当所有元素哈希到同一个桶时

### 6.2 空间复杂度

- O(n)，其中 n 是元素数量

### 6.3 负载因子

- 默认负载因子为 1.0
- 可通过 `max_load_factor()` 调整
- 负载因子越小，哈希冲突概率越低，但空间消耗越大

## 7. 特殊功能和优化

### 7.1 哈希表扩容

- 当容器大小超过 `bucket_count() * max_load_factor()` 时，会触发扩容
- 扩容策略采用预定义的质数序列
- 使用 `rehash()` 或 `reserve()` 可提前扩容，减少运行时开销

### 7.2 异常安全保证

- 提供基本异常安全保证
- 对 `emplace`、`emplace_hint` 和 `insert` 函数提供强异常安全保证

## 8. 使用示例

### 8.1 基本用法

```cpp
#include "unordered_set.h"
#include <string>

int main() {
    // 创建无序集合
    mystl::unordered_set<int> set1;
    
    // 插入元素
    set1.insert(10);
    set1.insert(20);
    set1.insert(30);
    
    // 使用emplace构造插入
    set1.emplace(40);
    
    // 检查元素是否存在
    if (set1.find(20) != set1.end()) {
        // 找到元素
    }
    
    // 遍历所有元素
    for (const auto& value : set1) {
        // 使用value
    }
    
    // 创建允许重复键的集合
    mystl::unordered_multiset<std::string> mset;
    
    // 插入重复元素
    mset.insert("apple");
    mset.insert("banana");
    mset.insert("apple");  // 允许重复
    
    // 计算特定键的元素数量
    size_t count = mset.count("apple");  // 返回2
    
    return 0;
}
```

### 8.2 自定义类型

对于自定义类型，需要提供哈希函数和相等比较函数：

```cpp
#include "unordered_set.h"
#include <string>

// 自定义类型
struct Person {
    std::string name;
    int age;
    
    Person(const std::string& n, int a) : name(n), age(a) {}
    
    bool operator==(const Person& other) const {
        return name == other.name && age == other.age;
    }
};

// 为Person提供自定义哈希函数
namespace std {
    template <>
    struct hash<Person> {
        size_t operator()(const Person& p) const {
            return hash<string>()(p.name) ^ hash<int>()(p.age);
        }
    };
}

int main() {
    mystl::unordered_set<Person> persons;
    
    persons.emplace("Alice", 25);
    persons.emplace("Bob", 30);
    
    return 0;
}
```

## 9. 优化建议

1. **桶数量优化**：如果预先知道元素数量，使用 `reserve()` 提前分配足够的桶
2. **负载因子调整**：根据内存和性能需求调整 `max_load_factor()`
3. **哈希函数优化**：为自定义类型提供高效的哈希函数，减少冲突
4. **避免频繁插入删除**：频繁操作可能导致性能下降

