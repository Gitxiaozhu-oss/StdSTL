# my_hashtable

## 1. 概述

`my_hashtable.h` 是一个C++模板类实现的哈希表，采用开链法（链表法）解决哈希冲突。该实现提供了类似STL中`unordered_map`和`unordered_set`的核心功能，支持泛型类型，以及自定义哈希函数和键值比较方法。

## 2. 设计理念

该哈希表实现基于以下设计理念：

- **模板化设计**：通过模板参数支持任意类型的键值对存储
- **开链法处理冲突**：使用链表解决哈希冲突，每个哈希桶维护一个节点链表
- **自适应调整**：根据负载因子自动调整哈希表大小
- **迭代器支持**：提供符合STL标准的迭代器接口
- **键值分离设计**：通过值特性类(value traits)支持普通元素和键值对两种使用场景

## 3. 主要组件

### 3.1 哈希表节点 (hashtable_node)

```cpp
template <class T>
struct hashtable_node
{
    hashtable_node* next;   // 指向下一个节点
    T               value;  // 存储实值

    hashtable_node() = default;
    hashtable_node(const T& n) : next(nullptr), value(n) {}
    // ... 其他构造函数 ...
};
```

每个节点存储一个值和指向下一个节点的指针，形成链表结构，用于解决哈希冲突。

### 3.2 值特性类 (ht_value_traits)

```cpp
template <class T>
struct ht_value_traits
{
    static constexpr bool is_map = is_pair<T>::value;  // 是否为映射类型
    typedef ht_value_traits_imp<T, is_map> value_traits_type;  // 值特性类型
    typedef typename value_traits_type::key_type    key_type;     // 键类型
    typedef typename value_traits_type::mapped_type mapped_type;  // 映射类型
    typedef typename value_traits_type::value_type  value_type;   // 值类型
    
    // ... 方法 ...
};
```

值特性类处理两种情况：
- 普通元素：键和值是同一个对象 (如`std::unordered_set`)
- 键值对：键是`pair`的第一个元素，值是整个`pair` (如`std::unordered_map`)

### 3.3 迭代器类

哈希表提供了四种迭代器：

- **ht_iterator**: 可变迭代器，遍历整个哈希表
- **ht_const_iterator**: 常量迭代器，遍历整个哈希表
- **ht_local_iterator**: 局部可变迭代器，仅遍历一个哈希桶
- **ht_const_local_iterator**: 局部常量迭代器，仅遍历一个哈希桶

迭代器均继承自`mystl::iterator`，符合STL迭代器接口标准。

### 3.4 哈希表主类 (hashtable)

```cpp
template <class T, class Hash, class KeyEqual>
class hashtable
{
    // ... 类型定义 ...
private:
    bucket_type buckets_;     // 桶数组，每个桶是一个链表头指针
    size_type   bucket_size_; // 桶数量
    size_type   size_;        // 元素数量
    float       mlf_;         // 最大负载因子
    hasher      hash_;        // 哈希函数
    key_equal   equal_;       // 判断键值相等的函数
    
    // ... 私有方法 ...
    
public:
    // ... 构造函数、操作方法等 ...
};
```

哈希表主类管理所有操作，包括插入、删除、查找等功能。

## 4. 关键算法与实现

### 4.1 哈希与桶管理

- **哈希函数**：通过模板参数`Hash`提供自定义哈希函数
- **素数表**：使用预定义的素数表作为桶大小，以减少哈希冲突
- **哈希计算**: 使用`hash_(key) % bucket_size_`计算桶索引

```cpp
size_type hash(const key_type& key) const
{
    return hash_(key) % bucket_size_;
}
```

### 4.2 插入操作

支持两种插入模式：

- **multi**：允许重复键值，如`unordered_multimap`
- **unique**：不允许重复键值，如`unordered_map`

关键实现：

```cpp
// 在不需要重建表格的情况下插入新节点，键值不允许重复
std::pair<iterator, bool> insert_unique_noresize(const value_type& value)
{
    const auto n = hash(value_traits::get_key(value));
    auto first = buckets_[n];
    // 检查键是否已存在
    for (auto cur = first; cur; cur = cur->next)
    {
        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(value)))
            return std::make_pair(iterator(cur, this), false);
    }
    // 让新节点成为链表的第一个节点
    auto tmp = create_node(value);  
    tmp->next = first;
    buckets_[n] = tmp;
    ++size_;
    return std::make_pair(iterator(tmp, this), true);
}
```

### 4.3 重哈希操作

当元素数量超过桶数量乘以负载因子时，进行重哈希：

```cpp
void rehash_if_need(size_type n)
{
    if (static_cast<float>(size_ + n) > (float)bucket_size_ * max_load_factor())
        rehash(size_ + n);
}
```

### 4.4 查找操作

```cpp
iterator find(const key_type& key)
{
    const auto n = hash(key);
    node_ptr first = buckets_[n];
    for (; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {}
    return iterator(first, this);
}
```

### 4.5 删除操作

支持三种删除方式：
- 删除单个位置的元素
- 删除范围内的元素
- 删除指定键的所有元素

## 5. 哈希表特性

### 5.1 桶管理

- 使用素数表作为可能的桶大小集合
- 根据元素数量智能选择适当的桶大小

```cpp
static constexpr size_t ht_prime_list[] = {
  101ull, 173ull, 263ull, 397ull, 599ull, /* ... */ 18446744073709551557ull
};
```

### 5.2 负载因子管理

- 默认最大负载因子为1.0
- 支持自定义最大负载因子
- 当负载因子超过阈值时自动触发重哈希

```cpp
float load_factor() const noexcept
{ return bucket_size_ != 0 ? (float)size_ / bucket_size_ : 0.0f; }

float max_load_factor() const noexcept
{ return mlf_; }

void max_load_factor(float ml)
{
    if (ml != ml || ml < 0)
        throw std::out_of_range("invalid hash load factor");
    mlf_ = ml;
}
```

## 6. 使用示例

### 6.1 基本用法

```cpp
// 创建哈希表
mystl::hashtable<int, std::hash<int>, std::equal_to<int>> ht(10);

// 插入元素
ht.insert_multi(42);
ht.insert_unique(100);

// 查找元素
auto it = ht.find(42);
if (it != ht.end()) {
    // 处理找到的元素
}

// 删除元素
ht.erase_multi(42);
```

### 6.2 作为map/set基础

此哈希表可以作为实现unordered_map和unordered_set的基础：

```cpp
// 使用哈希表实现键值对存储
mystl::hashtable<std::pair<std::string, int>, 
                 std::hash<std::string>, 
                 std::equal_to<std::string>> map_ht(10);
                 
// 插入键值对
map_ht.insert_unique(std::make_pair("apple", 1));
```

## 7. 性能考量

- **哈希函数**：哈希函数的质量直接影响哈希表性能
- **初始桶大小**：适当的初始桶大小可以减少重哈希次数
- **负载因子**：较低的负载因子提高查找性能，但增加内存消耗
- **链表长度**：链表过长会降低性能，应尽量避免严重哈希冲突