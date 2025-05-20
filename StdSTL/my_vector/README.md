## 简介

`mystl::vector` 是一个基于C++11标准的STL vector容器的实现，它提供了一个动态数组，能够在运行时自动调整大小。本文档旨在帮助开发者快速理解该实现的设计思路、关键组件和实现细节。

## 设计目标

- 兼容C++11标准
- 优化内存使用和性能
- 保证异常安全性
- 提供与标准库vector相似的接口

## 代码结构

`mystl::vector` 的实现包含以下主要部分：

1. **类型定义** - 定义容器相关的类型
2. **构造函数和析构函数** - 管理vector的生命周期
3. **赋值操作** - 实现各种赋值方式
4. **迭代器接口** - 提供迭代器访问容器元素
5. **容量相关函数** - 管理容器的大小和容量
6. **元素访问函数** - 提供对元素的访问方式
7. **修改容器相关函数** - 添加、删除和修改元素
8. **辅助功能函数** - 内部使用的工具函数

## 核心数据成员

```cpp
iterator begin_;  // 使用空间的起始位置
iterator end_;    // 使用空间的结束位置（最后一个元素之后的位置）
iterator cap_;    // 存储空间的结束位置
```

这三个指针构成了vector的核心：
- `begin_` 指向数据存储区的开始
- `end_` 指向最后一个实际元素之后的位置
- `cap_` 指向分配的内存块结束位置

## 关键功能详解

### 1. 内存管理

#### 初始化策略

```cpp
void try_init() noexcept {
    try {
        // 默认分配16个元素的空间
        begin_ = data_allocator().allocate(16);
        end_ = begin_;
        cap_ = begin_ + 16;
    } catch (...) {
        // 如果分配失败，将指针设为nullptr
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
    }
}
```

- 默认分配16个元素的空间
- 增长策略是当前容量的1.5倍或根据需要的新元素数量决定

#### 容量扩展

```cpp
size_type get_new_cap(size_type add_size) {
    const auto old_size = capacity();
    
    // 检查是否超出最大容量
    if (old_size > max_size() - add_size) {
        throw std::length_error("vector::get_new_cap - 新容量超出了最大容量");
    }
    
    // 如果当前容量过大，增长较小以避免溢出
    if (old_size > max_size() - old_size / 2) {
        return old_size + add_size > max_size() - 16
               ? old_size + add_size 
               : old_size + add_size + 16;
    }
    
    // 正常情况下，新容量为旧容量的1.5倍或者旧容量+add_size的较大者
    // 如果是空容器，则最小分配16个元素的空间
    const size_type new_size = old_size == 0
                               ? std::max(add_size, static_cast<size_type>(16))
                               : std::max(old_size + old_size / 2, old_size + add_size);
    
    return new_size;
}
```

该函数决定了vector需要扩容时的新容量，采用的策略是：
- 空容器最小分配16个元素空间
- 非空容器扩展为当前容量的1.5倍
- 特殊情况进行边界检查和调整

### 2. 元素操作

#### 元素插入

```cpp
template <class... Args>
iterator emplace(const_iterator pos, Args&&... args) {
    // 计算pos位置相对于begin_的偏移量
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = xpos - begin_;
    
    if (end_ != cap_ && xpos == end_) {
        // 如果是在尾部插入且有足够空间，直接构造
        data_allocator().construct(end_, std::forward<Args>(args)...);
        ++end_;
    } else if (end_ != cap_) {
        // 如果有足够空间但不是在尾部插入
        // ...省略具体实现
    } else {
        // 空间不足，需要重新分配
        reallocate_emplace(xpos, std::forward<Args>(args)...);
    }
    
    return begin() + n;
}
```

插入操作需考虑三种情况：
1. 在尾部插入且有足够空间：直接在尾部构造
2. 不在尾部插入但有足够空间：移动现有元素，在空出位置构造
3. 空间不足：重新分配更大空间，并在新空间中重新排列元素

#### 元素删除

```cpp
iterator erase(const_iterator first, const_iterator last) {
    if (first == last) {
        return const_cast<iterator>(first);
    }
    
    const auto n = first - begin();
    iterator r = begin_ + (first - begin());
    // 将last后的元素移动到first位置
    iterator new_end = std::move(const_cast<iterator>(last), end_, r);
    // 析构多余的元素
    for (iterator p = new_end; p != end_; ++p) {
        p->~T();
    }
    end_ = new_end;
    
    return begin_ + n;
}
```

删除操作主要涉及两步：
1. 移动后续元素到需删除元素的位置
2. 析构尾部多余的元素

### 3. 异常安全

为保证异常安全，代码中多处使用了try-catch块，例如在重新分配内存时：

```cpp
template <class T>
template <class... Args>
void vector<T>::reallocate_emplace(iterator pos, Args&&... args) {
    // 获取新的容量
    const auto new_size = get_new_cap(1);
    // 分配新内存
    auto new_begin = data_allocator().allocate(new_size);
    auto new_end = new_begin;
    
    try {
        // 将pos之前的元素移动到新内存
        new_end = mystl::uninitialized_move(begin_, pos, new_begin);
        // 在pos位置构造新元素
        data_allocator().construct(new_end, std::forward<Args>(args)...);
        ++new_end;
        // 将pos之后的元素移动到新内存
        new_end = mystl::uninitialized_move(pos, end_, new_end);
    } catch (...) {
        // 如果发生异常，销毁已构造的新元素并释放新内存
        for (auto p = new_begin; p != new_end; ++p) {
            p->~T();
        }
        data_allocator().deallocate(new_begin, new_size);
        throw;
    }
    
    // 销毁旧元素并释放旧内存
    destroy_and_recover(begin_, end_, cap_ - begin_);
    
    // 更新指针
    begin_ = new_begin;
    end_ = new_end;
    cap_ = new_begin + new_size;
}
```

这种设计确保了即使在内存分配或元素构造过程中发生异常，也不会导致内存泄漏，且容器保持在一个一致的状态。

### 4. 特殊实现细节

1. **C++11兼容性**：为了兼容C++11，实现了`uninitialized_move`函数

```cpp
template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_move(InputIterator first, InputIterator last, ForwardIterator result) {
    for (; first != last; ++first, ++result) {
        ::new (static_cast<void*>(std::addressof(*result))) 
            typename std::iterator_traits<ForwardIterator>::value_type(std::move(*first));
    }
    return result;
}
```

2. **禁用vector<bool>特化**：
```cpp
// 禁用vector<bool>的特殊实现
static_assert(!std::is_same<bool, T>::value, "vector<bool>在本实现中不被支持");
```

3. **初始容量策略**：选择合适的初始容量以减少重新分配的次数
```cpp
const size_type init_size = std::max(static_cast<size_type>(16), n);
```

## 性能特点

根据性能测试结果，`mystl::vector`在某些操作上表现出色：

1. **构造函数**：默认构造和带大小的构造比标准库快1.18倍和1.65倍
2. **拷贝构造**：比标准库快约12倍，表现非常优异
3. **中间删除**：与标准库性能相当

需要优化的部分：

1. **push_back**：比标准库慢约1.3倍
2. **resize操作**：比标准库慢约6.4倍
3. **中间插入**：比标准库慢约1.6倍

## 使用示例

```cpp
// 创建一个空vector
mystl::vector<int> v1;

// 创建指定大小的vector
mystl::vector<int> v2(5);

// 创建并初始化vector
mystl::vector<int> v3 = {1, 2, 3, 4, 5};

// 添加元素
v1.push_back(10);
v1.push_back(20);

// 使用迭代器
for (auto it = v1.begin(); it != v1.end(); ++it) {
    std::cout << *it << " ";
}

// 范围for循环
for (const auto& item : v3) {
    std::cout << item << " ";
}

// 容量管理
v1.reserve(100);  // 预留100个元素的空间
v1.resize(50);    // 调整大小为50个元素
v1.shrink_to_fit(); // 收缩容量以适应大小
```

## 总结

`mystl::vector`是一个功能完整、性能优秀的vector容器实现，它提供了与STL vector相似的API和语义。该实现特别注重内存管理、异常安全和性能优化，使其成为学习STL容器实现和实际应用的良好选择。

主要优点：
- 完整实现C++11标准中的vector功能
- 良好的内存管理策略
- 强大的异常安全保证
- 部分操作（如构造和拷贝）比标准库更快

如果您正在学习STL容器实现或需要一个自定义的vector容器，`mystl::vector`是一个值得参考的实现。
