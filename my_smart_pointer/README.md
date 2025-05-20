# C++11智能指针

## 1. 概述

`my_smart_pointer.h` 是一个符合C++11标准的智能指针库实现，提供了三种智能指针类：`unique_ptr`、`shared_ptr`和`weak_ptr`。这些智能指针用于自动化内存管理，防止内存泄漏，提高代码的安全性和健壮性。

该库具有以下特点：
- 完全符合C++11标准库接口规范
- 支持自定义删除器
- 提供完整的中文注释
- 实现了make_shared和make_unique等工厂函数

## 2. 智能指针基础

### 2.1 智能指针的作用

智能指针是对C++裸指针的封装，提供自动资源管理功能。当智能指针离开作用域时，它们会自动释放所管理的资源，避免资源泄漏。

### 2.2 三种智能指针类型

1. **`unique_ptr`**：独占所有权的智能指针，同一时刻只能有一个`unique_ptr`管理一个对象
2. **`shared_ptr`**：共享所有权的智能指针，多个`shared_ptr`可以共同管理同一个对象
3. **`weak_ptr`**：弱引用智能指针，不增加引用计数，用于解决循环引用问题

## 3. 实现细节

### 3.1 `unique_ptr` 实现

`unique_ptr`的核心设计特点：

1. **独占所有权**：禁止复制操作，只允许移动操作
2. **支持自定义删除器**：可以指定如何释放资源
3. **两种特化版本**：普通对象和数组对象

关键代码结构：
```cpp
template<typename T, typename Deleter = default_delete<T>>
class unique_ptr {
public:
    // 类型定义
    using element_type = typename std::remove_extent<T>::type;
    using deleter_type = Deleter;
    using pointer = element_type*;

private:
    pointer ptr_;           // 管理的指针
    deleter_type deleter_;  // 删除器
    
    // ... 具体实现 ...
};

// 数组特化版本
template<typename T, typename Deleter>
class unique_ptr<T[], Deleter> {
    // ... 数组版本的实现 ...
};
```

主要成员函数：
- 构造函数和析构函数
- 移动构造和移动赋值
- `reset()`：重置指针
- `release()`：释放所有权但不删除对象
- `get()`：获取原始指针
- `get_deleter()`：获取删除器

### 3.2 `shared_ptr` 实现

`shared_ptr`的核心设计特点：

1. **共享所有权**：通过引用计数管理对象生命周期
2. **线程安全的引用计数**：使用原子操作保证线程安全
3. **控制块设计**：将引用计数和资源管理分离

控制块的实现是`shared_ptr`的核心：

```cpp
// 控制块基类
class control_block_base {
private:
    std::atomic<long> shared_count_;  // 强引用计数
    std::atomic<long> weak_count_;    // 弱引用计数

public:
    // ... 计数管理方法 ...
    
    virtual void destroy_object() noexcept = 0;
    virtual const std::type_info& get_deleter_type() const noexcept = 0;
    virtual void* get_deleter() noexcept = 0;
};

// 具体控制块实现
template<typename T, typename Deleter>
class control_block : public control_block_base {
    // ... 具体实现 ...
};

// 内部类型Inplace控制块，直接在控制块内构造对象
template<typename T>
class inplace_control_block : public control_block_base {
    // ... 具体实现 ...
};
```

主要成员函数：
- 构造和析构
- 引用计数管理
- `use_count()`：获取当前引用计数
- `reset()`：重置指针
- `lock()`：从weak_ptr获取shared_ptr

### 3.3 `weak_ptr` 实现

`weak_ptr`的核心设计特点：

1. **不增加强引用计数**：不影响对象的生命周期
2. **可检查引用对象是否存在**：通过`expired()`方法
3. **可转换为shared_ptr**：通过`lock()`方法

关键代码结构：
```cpp
template<typename T>
class weak_ptr {
public:
    using element_type = typename std::remove_extent<T>::type;

private:
    element_type* ptr_ = nullptr;
    control_block_base* control_block_ = nullptr;
    
    // ... 具体实现 ...
};
```

主要成员函数：
- `expired()`：检查对象是否已被销毁
- `lock()`：尝试获取shared_ptr
- `use_count()`：获取引用计数

### 3.4 工厂函数

库提供了两个工厂函数来创建智能指针：

1. **`make_unique`**：创建unique_ptr对象
```cpp
template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, unique_ptr<T>>::type
make_unique(Args&&... args);
```

2. **`make_shared`**：创建shared_ptr对象
```cpp
template<typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args);
```

这些工厂函数的优势：
- 减少内存分配次数
- 异常安全
- 代码简洁

## 4. 关键机制

### 4.1 引用计数管理

`shared_ptr`和`weak_ptr`通过控制块实现引用计数管理：

1. **强引用计数**：由`shared_ptr`控制，当计数归零时删除对象
2. **弱引用计数**：由`weak_ptr`控制，当计数归零且强引用计数也为零时删除控制块

引用计数使用`std::atomic<long>`实现，保证线程安全。

### 4.2 内存布局优化

该库使用了两种内存布局：

1. **分离式布局**：对象和控制块分开分配
2. **紧凑式布局**：在`make_shared`中，对象直接构造在控制块内部

紧凑式布局减少了内存分配次数和内存碎片。

### 4.3 删除器管理

智能指针支持自定义删除器：

```cpp
// 默认删除器
template<typename T>
struct default_delete {
    void operator()(T* ptr) const {
        static_assert(sizeof(T) > 0, "不能删除不完整类型的对象");
        delete ptr;
    }
};

// 数组版本删除器
template<typename T>
struct default_delete<T[]> {
    void operator()(T* ptr) const {
        static_assert(sizeof(T) > 0, "不能删除不完整类型的对象");
        delete[] ptr;
    }
};
```

## 5. 使用示例

### 5.1 `unique_ptr` 使用

```cpp
// 基本使用
mystl::unique_ptr<TestClass> ptr(new TestClass(100));

// 移动所有权
mystl::unique_ptr<TestClass> ptr2 = std::move(ptr);

// 自定义删除器
mystl::unique_ptr<TestClass, CustomDeleter> ptr3(new TestClass(102), CustomDeleter());

// 数组版本
mystl::unique_ptr<int[]> arr(new int[5]);
for (int i = 0; i < 5; ++i) {
    arr[i] = i * 10;
}
```

### 5.2 `shared_ptr` 使用

```cpp
// 基本使用
mystl::shared_ptr<TestClass> ptr1(new TestClass(200));

// 复制共享所有权
mystl::shared_ptr<TestClass> ptr2 = ptr1;

// 使用工厂函数
auto ptr4 = mystl::make_shared<TestClass>(201);
```

### 5.3 `weak_ptr` 使用

```cpp
// 创建weak_ptr从shared_ptr
mystl::shared_ptr<TestClass> shared = mystl::make_shared<TestClass>(300);
mystl::weak_ptr<TestClass> weak = shared;

// 检查是否过期
bool isExpired = weak.expired();

// 获取shared_ptr
mystl::shared_ptr<TestClass> locked = weak.lock();
```

### 5.4 解决循环引用

```cpp
struct Node {
    mystl::shared_ptr<Node> next;     // 会导致循环引用
    mystl::weak_ptr<Node> weak_next;  // 避免循环引用
    int id;
};

// 使用weak_ptr避免循环引用
auto node3 = mystl::make_shared<Node>(3);
auto node4 = mystl::make_shared<Node>(4);

// 正确用法：一方使用weak_ptr
node3->weak_next = node4;
node4->weak_next = node3;
```

## 6. 注意事项与最佳实践

1. **不要将同一原始指针传递给多个独立的`shared_ptr`**
   ```cpp
   // 错误用法
   TestClass* raw = new TestClass();
   mystl::shared_ptr<TestClass> sp1(raw);
   mystl::shared_ptr<TestClass> sp2(raw);  // 危险：双重释放
   
   // 正确用法
   mystl::shared_ptr<TestClass> sp1(new TestClass());
   mystl::shared_ptr<TestClass> sp2 = sp1;  // 共享同一对象
   ```

2. **尽量使用工厂函数而非直接构造**
   ```cpp
   // 推荐
   auto ptr = mystl::make_shared<TestClass>(100);
   
   // 不推荐
   mystl::shared_ptr<TestClass> ptr(new TestClass(100));
   ```

3. **使用`weak_ptr`解决循环引用问题**
   
4. **对于数组对象，使用数组特化版本**
   ```cpp
   // 正确
   mystl::unique_ptr<int[]> arr(new int[10]);
   
   // 错误
   mystl::unique_ptr<int> arr(new int[10]);  // 会导致内存泄漏
   ```

5. **返回智能指针时注意类型转换**
   
6. **了解移动语义对智能指针的影响**