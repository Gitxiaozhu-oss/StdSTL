# my_map.h 

## 1. 概述

`my_map.h`实现了两个关联容器：`map`和`multimap`，它们都是基于红黑树的键值对集合。这个实现遵循C++11标准，提供了与标准库类似的接口和功能，但使用自定义的命名空间和实现方式。

- `map`：键值对容器，键不允许重复，按键排序
- `multimap`：允许键重复的键值对容器，按键排序

## 2. 整体架构

两个类模板的底层实现都依赖于`mystl::rb_tree`红黑树，但对外提供标准的map/multimap接口。

## 3. 关键组件详解

### 3.1 辅助功能函数

实现了三个关键的辅助函数模板：

```cpp
// 比较函数
template <class T>
struct less : public std::binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x < y; }
};

// 移动语义
template <class T>
typename std::remove_reference<T>::type&& move(T&& t) noexcept;

// 完美转发
template <class T>
T&& forward(typename std::remove_reference<T>::type& t) noexcept;
```

这些辅助函数使得实现能够与标准库兼容，同时保持在自定义命名空间内的独立性。

### 3.2 map 类模板

#### 嵌套类型

map类内部定义了多种嵌套类型以保持与标准库接口的一致性：

```cpp
typedef Key                        key_type;
typedef T                          mapped_type;
typedef std::pair<const Key, T>    value_type;
typedef Compare                    key_compare;
// ...以及迭代器相关的类型定义
```

还包含一个`value_compare`嵌套类，用于比较两个元素的键值。

#### 底层实现

map使用红黑树作为底层实现机制：

```cpp
typedef mystl::rb_tree<value_type, key_compare>  base_type;
base_type tree_;
```

这种实现允许map继承红黑树的平衡特性，保证查找、插入和删除操作的O(log n)时间复杂度。

#### 关键函数实现

1. **构造函数**：支持默认构造、范围构造、初始化列表构造、拷贝构造和移动构造。

2. **元素访问**：
   - `at()`：安全访问元素，如果键不存在则抛出异常
   - `operator[]`：访问或创建元素

3. **修改器**：
   - `insert()`：多种插入方式（单元素、范围、提示位置）
   - `emplace()`：原地构造元素
   - `erase()`：删除元素（通过位置、键或范围）
   - `clear()`：清空容器

4. **查询操作**：
   - `find()`：查找键对应的元素
   - `count()`：计算键的出现次数
   - `lower_bound()`：返回不小于键的第一个元素
   - `upper_bound()`：返回大于键的第一个元素
   - `equal_range()`：返回与键匹配的元素范围

### 3.3 multimap 类模板

multimap的实现结构与map类似，但有以下区别：

1. 允许键重复，使用红黑树的`insert_multi`和`emplace_multi`而非`insert_unique`和`emplace_unique`
2. `count()`可能返回大于1的值
3. `equal_range()`可能返回包含多个元素的范围
4. 没有`operator[]`操作符，因为键不唯一无法确定返回哪个值

## 4. 异常安全保证

代码实现了不同等级的异常安全保证：

- 基本异常安全保证：大多数操作出现异常时容器保持有效状态
- 强异常安全保证：特定操作（emplace、emplace_hint、insert）提供强异常安全保证，即操作要么完全成功，要么容器状态不变

这通过以下代码体现：
```cpp
// 异常保证：
// mystl::map<Key, T> / mystl::multimap<Key, T> 满足基本异常保证，对以下等函数做强异常安全保证：
//   * emplace
//   * emplace_hint
//   * insert
```

## 5. 核心功能和红黑树关系

map和multimap的核心功能都委托给底层的红黑树实现：

- 有序性通过红黑树的自平衡特性保证
- 查找通过红黑树的二分搜索实现O(log n)时间复杂度
- 插入/删除通过红黑树的平衡调整保持O(log n)时间复杂度
- 迭代器通过红黑树的中序遍历提供有序遍历

以下代码展示了map如何将操作委托给红黑树：
```cpp
// 插入操作
std::pair<iterator, bool> insert(const value_type& value) {
    return tree_.insert_unique(value);
}

// 查找操作
iterator find(const key_type& key) {
    return tree_.find(key);
}
```

## 6. 使用方式和示例

### 基本使用

```cpp
// 创建map
my::map<int, std::string> m;

// 插入元素
m.insert({1, "一"});
m[2] = "二";  // 使用[]操作符

// 访问元素
std::string value = m.at(1);  // 安全访问，若键不存在会抛出异常
std::string value2 = m[3];    // 若键不存在，会创建一个默认值

// 迭代和遍历
for (auto& pair : m) {
    std::cout << pair.first << " -> " << pair.second << std::endl;
}

// 查找
auto it = m.find(2);
if (it != m.end()) {
    std::cout << "找到元素: " << it->second << std::endl;
}
```

### multimap特有操作

```cpp
// 创建multimap
my::multimap<int, std::string> mm;

// 插入多个相同键的元素
mm.insert({1, "一"});
mm.insert({1, "一一"});

// 查找所有匹配元素
auto range = mm.equal_range(1);
for (auto it = range.first; it != range.second; ++it) {
    std::cout << it->first << " -> " << it->second << std::endl;
}

// 计数
std::cout << "键1的元素数量: " << mm.count(1) << std::endl;
```

## 7. 性能特点和优化点

1. **时间复杂度**：
   - 查找、插入、删除操作：平均O(log n)
   - 遍历操作：O(n)

2. **优化点**：
   - 使用移动语义减少拷贝开销
   - 使用完美转发减少临时对象创建
   - 提供hint版本的插入操作提高性能
   - 使用默认参数减少代码冗余

3. **内存占用**：
   - 每个节点额外需要父节点和两个子节点指针
   - 每个节点需要一个颜色标志位