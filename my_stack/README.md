# `my_stack.h` 技术文档

## 一、概述

`my_stack` 是一个基于C++11标准实现的栈容器适配器，采用LIFO（后进先出）原则。该实现封装了底层容器（默认为`mystl::deque<T>`），并提供一组精简的接口用于栈操作。本文档旨在帮助开发者快速理解`my_stack`的设计思路、实现细节和使用方法。

## 二、设计理念

### 1. 适配器模式

`my_stack`采用适配器设计模式，将底层容器（默认为`deque`）的接口转换为一组标准的栈操作接口。通过这种方式：

- 隐藏了底层容器的复杂性
- 限制了对数据的访问方式，确保只能从栈顶进行操作
- 提高了代码的可重用性，可以轻松替换底层容器

### 2. 类型安全

实现中使用静态断言确保模板参数与底层容器的值类型匹配，增强了类型安全性：

```cpp
static_assert(std::is_same<T, value_type>::value,
            "the value_type of Container should be same with T");
```

### 3. 异常安全

通过适当的`noexcept`规范和移动语义，提高了异常安全性和性能。

## 三、核心组件

### 1. 类型定义

```cpp
typedef Container                           container_type;
typedef typename Container::value_type      value_type;
typedef typename Container::size_type       size_type;
typedef typename Container::reference       reference;
typedef typename Container::const_reference const_reference;
```

这些类型定义从底层容器中继承，保持了类型的一致性。

### 2. 私有成员

```cpp
container_type c_;  // 用底层容器表现栈
```

单一私有成员变量，封装了底层容器实例。

## 四、接口功能详解

### 1. 构造函数

`my_stack`提供了多种构造方式：

- **默认构造函数**：创建空栈
- **指定大小构造**：创建包含n个默认值的栈
- **指定大小和初值构造**：创建包含n个指定值的栈
- **迭代器范围构造**：从迭代器范围创建栈
- **初始化列表构造**：从初始化列表创建栈
- **容器构造**：直接从底层容器构造栈（拷贝和移动版本）
- **拷贝构造**：从另一个栈拷贝
- **移动构造**：从另一个栈移动数据

### 2. 元素访问

```cpp
reference top() { return c_.back(); }
const_reference top() const { return c_.back(); }
```

仅提供对栈顶元素的访问，遵循栈的基本原则。

### 3. 容量操作

```cpp
[[nodiscard]] bool empty() const noexcept { return c_.empty(); }
size_type size() const noexcept { return c_.size(); }
```

提供基本的容量查询功能，使用`[[nodiscard]]`属性提示开发者不应忽略`empty()`的返回值。

### 4. 修改操作

- **emplace**：原地构造元素并添加到栈顶
- **push**：将元素添加到栈顶（拷贝和移动版本）
- **pop**：移除栈顶元素
- **clear**：清空栈
- **swap**：交换两个栈的内容

### 5. 比较操作

重载了六种比较运算符：`==`, `!=`, `<`, `>`, `<=`, `>=`，支持栈之间的完整比较操作。

## 五、实现技术

### 1. 现代C++11特性应用

- **右值引用与移动语义**：优化资源管理
  ```cpp
  void push(value_type&& value) { c_.push_back(std::move(value)); }
  ```

- **完美转发**：保留参数原有的值类别
  ```cpp
  template <class... Args>
  void emplace(Args&&... args) { c_.emplace_back(std::forward<Args>(args)...); }
  ```

- **noexcept规范**：提高编译器优化空间
  ```cpp
  void swap(stack& rhs) noexcept(noexcept(std::swap(c_, rhs.c_)))
  ```

- **统一初始化**：使用初始化列表构造和赋值
  ```cpp
  stack(std::initializer_list<T> ilist) :c_(ilist.begin(), ilist.end()) {}
  ```

### 2. 与底层容器的交互

栈的所有操作都转发到底层容器的对应方法：
- `top()` → `c_.back()`
- `push()` → `c_.push_back()`
- `pop()` → `c_.pop_back()`
- `empty()` → `c_.empty()`
- `size()` → `c_.size()`

### 3. swap实现

使用了`using std::swap`习惯用法，支持ADL（参数依赖查找）：

```cpp
void swap(stack& rhs) noexcept(noexcept(std::swap(c_, rhs.c_)))
{ 
    using std::swap;
    swap(c_, rhs.c_);
}
```

## 六、优化亮点

1. **[[nodiscard]]属性**：提醒调用者处理`empty()`返回值，避免误用
2. **委托底层容器**：避免重复实现，减少代码量
3. **友元函数实现比较**：直接访问底层容器，提高性能
4. **条件noexcept**：只在确保不抛出异常时标记为noexcept，增加编译器优化机会
5. **移动语义**：减少不必要的复制操作
6. **clear优化**：逐个弹出元素，避免一次性大量析构可能带来的问题

## 七、使用示例

```cpp
#include "my_stack.h"
#include <iostream>
#include <string>

int main() {
    // 创建整数栈
    mystl::stack<int> s;
    
    // 添加元素
    s.push(10);
    s.push(20);
    s.push(30);
    
    // 访问栈顶元素
    std::cout << "栈顶元素: " << s.top() << std::endl;  // 输出: 30
    
    // 移除栈顶元素
    s.pop();
    std::cout << "移除后栈顶元素: " << s.top() << std::endl;  // 输出: 20
    
    // 检查栈大小
    std::cout << "栈大小: " << s.size() << std::endl;  // 输出: 2
    
    // 使用emplace添加元素
    mystl::stack<std::string> strStack;
    strStack.emplace("Hello");
    strStack.emplace("World");
    
    // 清空栈
    s.clear();
    std::cout << "栈是否为空: " << (s.empty() ? "是" : "否") << std::endl;  // 输出: 是
    
    return 0;
}
```

## 八、总结

`my_stack`通过容器适配器模式，在底层容器之上提供了一个符合标准的栈接口。它充分利用了C++11特性，实现了高效、安全的栈操作。代码结构清晰，注释完善，是一个易于使用和理解的栈容器实现。

