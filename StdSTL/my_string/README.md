# mystl::string 

## 1. 概述

`my_string.h` 是一个实现了C++11标准string类的头文件，它提供了一个完整的`basic_string`类模板，用于处理文本字符串。该实现遵循C++11标准库中的string类接口，并提供了丰富的字符串操作功能。

## 2. 核心组件

### 2.1 char_traits 类模板

`char_traits` 模板类定义了字符类型的基本特性和操作，为字符串提供了统一的操作接口。

主要特性：
- 通用模板实现及对`char`和`wchar_t`类型的特化版本
- 提供字符串长度计算、比较、复制、移动、填充和查找等操作
- 利用标准库中的内存操作函数进行优化

```cpp
template <class CharType>
struct char_traits {
    typedef CharType char_type;
    static size_t length(const char_type* str) noexcept;
    static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept;
    static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept;
    static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept;
    static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept;
    static const char_type* find(const char_type* s, size_t n, const char_type& c) noexcept;
};
```

### 2.2 basic_string 模板类

`basic_string` 是字符串类的核心实现，是一个模板类，可以适用于不同类型的字符。

模板参数：
- `CharT`: 字符类型（如`char`、`wchar_t`等）
- `Traits`: 字符特性类（默认为`char_traits<CharT>`）
- `Alloc`: 分配器类型（默认为`std::allocator<CharT>`）

类型定义：
```cpp
using traits_type = Traits;
using value_type = CharT;
using allocator_type = Alloc;
using size_type = typename std::allocator_traits<Alloc>::size_type;
using difference_type = typename std::allocator_traits<Alloc>::difference_type;
using reference = value_type&;
using const_reference = const value_type&;
using pointer = value_type*;
using const_pointer = const value_type*;
using iterator = value_type*;
using const_iterator = const value_type*;
using reverse_iterator = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>;
```

## 3. 内存管理设计

### 3.1 Rep结构体

字符串的内部实现使用了一个名为`Rep`的表示结构体，用于管理实际的字符数据。

特点：
- 将元数据（容量和大小）与字符数据存储在连续的内存区域
- 使用一次内存分配来存储元数据和字符数据
- 提供创建和销毁Rep的静态方法

```cpp
struct Rep {
    size_type capacity;  // 容量
    size_type size;      // 大小
    
    // 获取字符数组起始位置
    CharT* data() noexcept {
        return reinterpret_cast<CharT*>(this + 1);
    }
    
    // 创建新的Rep
    static Rep* create(const Alloc& alloc, size_type capacity);
    
    // 释放Rep
    static void destroy(const Alloc& alloc, Rep* rep);
};
```

### 3.2 优化策略

- **内存布局优化**：将元数据和数据放在连续内存中，减少内存分配次数
- **小对象优化**：初始分配固定大小（STRING_INIT_SIZE）的内存
- **增长策略**：在需要增加容量时，新容量至少是当前容量的2倍
- **自定义分配器支持**：支持用户提供自定义的内存分配器

## 4. 功能和接口

### 4.1 构造函数

提供多种构造方式：
- 默认构造函数
- 从C风格字符串构造
- 填充构造（特定字符重复n次）
- 范围构造（从迭代器范围构造）
- 复制和移动构造
- 从初始化列表构造

### 4.2 赋值操作

- 复制赋值运算符
- 移动赋值运算符 
- C风格字符串赋值
- 字符赋值
- 初始化列表赋值
- 多种`assign`重载方法

### 4.3 元素访问

- `operator[]`: 无边界检查的访问
- `at()`: 带边界检查的访问，越界时抛出异常
- `front()`: 访问第一个元素
- `back()`: 访问最后一个元素
- `c_str()`: 获取C风格字符串
- `data()`: 获取内部字符数组

### 4.4 迭代器支持

- `begin()`, `end()`: 普通迭代器
- `cbegin()`, `cend()`: 常量迭代器
- `rbegin()`, `rend()`: 反向迭代器
- `crbegin()`, `crend()`: 常量反向迭代器

### 4.5 容量管理

- `size()`, `length()`: 获取字符串长度
- `max_size()`: 获取最大可能长度
- `resize()`: 调整字符串大小
- `capacity()`: 获取当前容量
- `reserve()`: 预留空间
- `shrink_to_fit()`: 收缩容量适应当前大小
- `empty()`: 检查字符串是否为空
- `clear()`: 清空字符串

## 5. C++11特性支持

- **移动语义**：提供移动构造和移动赋值，避免不必要的复制
- **noexcept说明**：适当位置标记noexcept，提高异常安全性
- **初始化列表**：支持使用初始化列表构造和赋值
- **类型特性**：使用std::allocator_traits等类型特性
- **SFINAE**：使用enable_if进行模板函数启用/禁用
- **常量表达式**：使用constexpr声明编译期常量

## 6. 最佳实践示例

### 6.1 基本使用

```cpp
// 创建字符串
mystl::string s1;                     // 默认构造
mystl::string s2 = "Hello";           // 从C字符串构造
mystl::string s3(5, 'A');             // 填充构造 "AAAAA"
mystl::string s4 = s2;                // 复制构造
mystl::string s5 = std::move(s4);     // 移动构造
mystl::string s6 = {'H', 'i', '!'};   // 初始化列表构造

// 访问元素
char first = s2[0];                   // 使用下标访问
char last = s2.back();                // 获取最后一个字符
s2[0] = 'h';                          // 修改字符

// 迭代器操作
for (auto c : s2) {                   // 范围for循环
    std::cout << c;
}

// 容量管理
s2.reserve(100);                      // 预留空间
s2.resize(10, 'X');                   // 调整大小并填充
s2.shrink_to_fit();                   // 释放多余内存
```

### 6.2 测试字符串功能

```cpp
void test_string() {
    mystl::string s = "Test";
    
    // 检查大小
    assert(s.size() == 4);
    assert(s.length() == 4);
    
    // 字符访问
    assert(s[0] == 'T');
    assert(s.at(1) == 'e');
    
    // 修改内容
    s[0] = 't';
    assert(s.c_str()[0] == 't');
    
    // 容量操作
    s.resize(8, 'X');
    assert(s.size() == 8);
    assert(std::string(s.c_str()) == "testXXXX");
    
    // 清空字符串
    s.clear();
    assert(s.empty());
}
```

## 7. 性能注意事项

- **内存分配策略**：当字符串增长需要重新分配内存时，会使用翻倍策略，这有助于减少内存分配次数，但也可能造成内存浪费
- **小字符串优化**：标准库多使用小字符串优化(SSO)，本实现使用固定初始大小，可能对小字符串不够优化
- **分配器传播**：实现考虑了分配器传播特性，使用std::allocator_traits来处理
- **移动语义**：尽可能使用移动语义减少不必要的复制操作
