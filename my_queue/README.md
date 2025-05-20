# mystl::queue 技术文档

## 概述

`my_queue.h` 实现了两个容器类：`queue`（队列）和 `priority_queue`（优先队列），这是标准模板库(STL)中重要容器的重新实现版本。本文档旨在帮助您快速理解和使用这些容器。

### queue类

`queue`是一个标准的先进先出(FIFO)容器，主要特点包括：

- 默认使用`mystl::deque`作为底层容器
- 元素总是从队尾添加，从队首移除
- 仅能访问队首和队尾元素
- 提供高效的元素添加和移除操作

### priority_queue类

`priority_queue`是一个优先队列实现，主要特点包括：

- 默认使用`mystl::vector`作为底层容器
- 自带高效堆算法实现（无需依赖外部堆操作）
- 支持自定义比较器，默认为`std::less`（最大堆）
- 始终能访问优先级最高的元素

## 使用指南

### 包含头文件

```cpp
#include "my_queue.h"
```

### 创建队列

```cpp
// 创建普通队列
mystl::queue<int> q1;                    // 默认构造
mystl::queue<int> q2({1, 2, 3, 4});      // 使用初始化列表构造
mystl::queue<int, std::vector<int>> q3;  // 使用vector作为底层容器

// 创建优先队列
mystl::priority_queue<int> pq1;          // 默认最大堆
// 最小堆优先队列
mystl::priority_queue<int, mystl::vector<int>, std::greater<int>> pq2;
```

### 队列操作示例

```cpp
// 添加元素
q1.push(10);
q1.push(20);

// 使用emplace原地构造（避免拷贝）
q1.emplace(30);

// 访问元素
int front_value = q1.front();  // 获取队首元素
int back_value = q1.back();    // 获取队尾元素

// 移除元素
q1.pop();  // 移除队首元素

// 检查状态
bool is_empty = q1.empty();
size_t size = q1.size();

// 清空队列
q1.clear();
```

### 优先队列操作示例

```cpp
// 添加元素
pq1.push(30);
pq1.push(10);
pq1.push(20);

// 使用emplace原地构造
pq1.emplace(40);

// 访问元素
int top_value = pq1.top();  // 获取顶部元素（最高优先级）

// 移除元素
pq1.pop();  // 移除顶部元素

// 检查状态
bool is_empty = pq1.empty();
size_t size = pq1.size();

// 清空优先队列
pq1.clear();
```

### 使用自定义类型

```cpp
// 自定义类型示例
struct Person {
    std::string name;
    int age;
    
    Person(const std::string& n, int a) : name(n), age(a) {}
    
    // 优先队列需要比较运算符
    bool operator<(const Person& rhs) const {
        return age < rhs.age;  // 按年龄比较
    }
};

// 创建存储自定义类型的优先队列
mystl::priority_queue<Person> pq;
pq.push(Person("张三", 25));
pq.push(Person("李四", 30));

// 使用emplace直接构造
pq.emplace("王五", 20);

// 获取优先级最高的人
const Person& top_person = pq.top();
```

## API参考

### queue

#### 构造函数

```cpp
queue();                                        // 默认构造函数
explicit queue(size_type n);                    // 构造指定大小的队列
queue(size_type n, const value_type& value);    // 构造n个value的队列
queue(IIter first, IIter last);                 // 使用迭代器范围构造
queue(std::initializer_list<T> ilist);          // 使用初始化列表构造
queue(const Container& c);                      // 使用容器构造
queue(Container&& c);                           // 使用右值容器构造
queue(const queue& rhs);                        // 拷贝构造
queue(queue&& rhs);                             // 移动构造
```

#### 元素访问

```cpp
reference front();                              // 访问队首元素
const_reference front() const;                  // 访问队首元素（常量版本）
reference back();                               // 访问队尾元素
const_reference back() const;                   // 访问队尾元素（常量版本）
```

#### 容量操作

```cpp
bool empty() const noexcept;                    // 检查队列是否为空
size_type size() const noexcept;                // 返回队列元素数量
```

#### 修改操作

```cpp
template <class ...Args>
void emplace(Args&& ...args);                   // 原地构造并添加元素到队尾
void push(const value_type& value);             // 添加元素到队尾
void push(value_type&& value);                  // 添加元素到队尾（移动版本）
void pop();                                     // 移除队首元素
void clear();                                   // 清空队列
void swap(queue& rhs);                          // 交换两个队列的内容
```

### priority_queue

#### 构造函数

```cpp
priority_queue();                               // 默认构造函数
explicit priority_queue(const Compare& c);      // 使用比较器构造
explicit priority_queue(size_type n);           // 构造指定大小的优先队列
priority_queue(size_type n, const value_type& value); // 构造n个value的优先队列
priority_queue(IIter first, IIter last);        // 使用迭代器范围构造
priority_queue(std::initializer_list<T> ilist); // 使用初始化列表构造
priority_queue(const Container& s);             // 使用容器构造
priority_queue(Container&& s);                  // 使用右值容器构造
priority_queue(const priority_queue& rhs);      // 拷贝构造
priority_queue(priority_queue&& rhs);           // 移动构造
```

#### 元素访问

```cpp
const_reference top() const;                    // 获取顶部元素（最高优先级）
```

#### 容量操作

```cpp
bool empty() const noexcept;                    // 检查优先队列是否为空
size_type size() const noexcept;                // 返回优先队列元素数量
```

#### 修改操作

```cpp
template <class... Args>
void emplace(Args&& ...args);                   // 原地构造并添加元素
void push(const value_type& value);             // 添加元素
void push(value_type&& value);                  // 添加元素（移动版本）
void pop();                                     // 移除顶部元素
void clear();                                   // 清空优先队列
void swap(priority_queue& rhs);                 // 交换内容
```

## 实现细节

### 堆算法

优先队列内置了自定义的堆算法实现，包括：

- `my___adjust_heap`：堆中的上滤操作
- `my_make_heap`：创建堆
- `my_push_heap`：添加元素到堆
- `my_pop_heap`：从堆中移除顶部元素

这些算法是优先队列高效运行的关键，对于大量数据的处理尤其重要。

### 注意事项

1. 优先队列默认为最大堆，如需最小堆请使用`std::greater`作为比较器
2. 优先队列中的自定义类型必须提供适当的比较运算符
3. `queue`和`priority_queue`都不支持随机访问元素
4. 使用`emplace`进行原地构造可以避免不必要的拷贝，提高性能