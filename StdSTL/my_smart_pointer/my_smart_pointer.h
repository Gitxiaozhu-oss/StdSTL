#ifndef MYSTL_SMART_POINTER_H_
#define MYSTL_SMART_POINTER_H_

/**
 * @file my_smart_pointer.h
 * @brief 实现C++11标准的智能指针
 * 
 * 该文件实现了三个智能指针类：unique_ptr、shared_ptr和weak_ptr
 * 
 * - unique_ptr：独占所有权的智能指针，同一时刻只有一个unique_ptr可以拥有一个对象
 * - shared_ptr：共享所有权的智能指针，多个shared_ptr可以共同拥有同一个对象
 * - weak_ptr：弱引用智能指针，不增加对象的引用计数，用于解决循环引用问题
 * 
 * 主要特性：
 * 1. 符合C++11标准库规范的API
 * 2. 支持自定义删除器
 * 3. 提供make_shared和make_unique等工厂函数
 * 
 * 实现细节：
 * - shared_ptr使用引用计数机制，通过控制块管理对象的生命周期
 * - weak_ptr不会影响对象的生命周期，但可以检查对象是否存在并获取shared_ptr
 * - unique_ptr不允许复制，只允许移动，确保所有权的唯一性
 * 
 * 使用示例见 test_smart_pointer.cpp
 */

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <atomic>

namespace mystl {

// 前向声明
template<typename T> class shared_ptr;
template<typename T> class weak_ptr;

// ------------------------------------------------------------------------------------------
// 智能指针的辅助工具
// ------------------------------------------------------------------------------------------

/**
 * @brief 默认删除器
 * 
 * @tparam T 被删除对象的类型
 */
template<typename T>
struct default_delete {
    /**
     * @brief 类型定义
     */
    using pointer = T*;
    
    /**
     * @brief 操作符()，执行删除操作
     * 
     * @param ptr 要删除的指针
     */
    void operator()(T* ptr) const {
        static_assert(sizeof(T) > 0, "不能删除不完整类型的对象");
        delete ptr;
    }
};

/**
 * @brief 默认删除器的数组特化版本
 * 
 * @tparam T 被删除数组的元素类型
 */
template<typename T>
struct default_delete<T[]> {
    /**
     * @brief 类型定义
     */
    using pointer = T*;
    
    /**
     * @brief 操作符()，执行删除数组操作
     * 
     * @param ptr 要删除的数组指针
     */
    void operator()(T* ptr) const {
        static_assert(sizeof(T) > 0, "不能删除不完整类型的对象");
        delete[] ptr;
    }
};

// ------------------------------------------------------------------------------------------
// unique_ptr类 - 独占所有权的智能指针
// ------------------------------------------------------------------------------------------

/**
 * @brief unique_ptr类，提供独占所有权的智能指针功能
 * 
 * @tparam T 被管理对象的类型
 * @tparam Deleter 删除器类型
 */
template<typename T, typename Deleter = default_delete<T>>
class unique_ptr {
public:
    /**
     * @brief 类型定义
     */
    using element_type = typename std::remove_extent<T>::type;
    using deleter_type = Deleter;
    using pointer = element_type*;

private:
    /**
     * @brief 指向被管理对象的指针
     */
    pointer ptr_;
    
    /**
     * @brief 删除器
     */
    deleter_type deleter_;
    
public:
    /**
     * @brief 默认构造函数
     */
    constexpr unique_ptr() noexcept
        : ptr_(nullptr), deleter_() {}
    
    /**
     * @brief 空指针构造函数
     */
    constexpr unique_ptr(std::nullptr_t) noexcept
        : unique_ptr() {}
    
    /**
     * @brief 指针构造函数
     * 
     * @param p 指向被管理对象的指针
     */
    explicit unique_ptr(pointer p) noexcept
        : ptr_(p), deleter_() {}
    
    /**
     * @brief 指针和删除器构造函数
     * 
     * @param p 指向被管理对象的指针
     * @param d 删除器
     */
    unique_ptr(pointer p, const deleter_type& d) noexcept
        : ptr_(p), deleter_(d) {}
    
    /**
     * @brief 移动构造函数
     * 
     * @param u 另一个unique_ptr
     */
    unique_ptr(unique_ptr&& u) noexcept
        : ptr_(u.ptr_), deleter_(std::move(u.deleter_)) {
        u.ptr_ = nullptr;
    }
    
    /**
     * @brief 析构函数
     */
    ~unique_ptr() {
        if (ptr_) {
            deleter_(ptr_);
        }
    }
    
    /**
     * @brief 禁止复制构造
     */
    unique_ptr(const unique_ptr&) = delete;
    
    /**
     * @brief 禁止复制赋值
     */
    unique_ptr& operator=(const unique_ptr&) = delete;
    
    /**
     * @brief 移动赋值运算符
     * 
     * @param u 另一个unique_ptr
     * @return unique_ptr& 自身引用
     */
    unique_ptr& operator=(unique_ptr&& u) noexcept {
        if (this != &u) {
            if (ptr_) {
                deleter_(ptr_);
            }
            ptr_ = u.ptr_;
            deleter_ = std::move(u.deleter_);
            u.ptr_ = nullptr;
        }
        return *this;
    }
    
    /**
     * @brief 空指针赋值运算符
     * 
     * @param nullp nullptr
     * @return unique_ptr& 自身引用
     */
    unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }
    
    /**
     * @brief 获取被管理对象的指针
     * 
     * @return pointer 指向被管理对象的指针
     */
    pointer get() const noexcept {
        return ptr_;
    }
    
    /**
     * @brief 获取删除器
     * 
     * @return deleter_type& 删除器的引用
     */
    deleter_type& get_deleter() noexcept {
        return deleter_;
    }
    
    /**
     * @brief 获取删除器（常量版本）
     * 
     * @return const deleter_type& 删除器的常量引用
     */
    const deleter_type& get_deleter() const noexcept {
        return deleter_;
    }
    
    /**
     * @brief 转换为布尔类型，检查是否管理对象
     * 
     * @return true 如果管理对象
     * @return false 如果不管理对象
     */
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
    
    /**
     * @brief 解引用操作符
     * 
     * @return element_type& 被管理对象的引用
     */
    typename std::add_lvalue_reference<element_type>::type operator*() const {
        return *ptr_;
    }
    
    /**
     * @brief 箭头操作符
     * 
     * @return pointer 指向被管理对象的指针
     */
    pointer operator->() const noexcept {
        return ptr_;
    }
    
    /**
     * @brief 释放所有权，但不销毁对象
     * 
     * @return pointer 被释放的指针
     */
    pointer release() noexcept {
        pointer temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }
    
    /**
     * @brief 重置为新的指针，销毁之前的对象
     * 
     * @param p 新的指针
     */
    void reset(pointer p = pointer()) noexcept {
        pointer temp = ptr_;
        ptr_ = p;
        if (temp) {
            deleter_(temp);
        }
    }
    
    /**
     * @brief 交换两个unique_ptr
     * 
     * @param other 另一个unique_ptr
     */
    void swap(unique_ptr& other) noexcept {
        using std::swap;
        swap(ptr_, other.ptr_);
        swap(deleter_, other.deleter_);
    }
};

/**
 * @brief 交换两个unique_ptr
 * 
 * @tparam T 被管理对象的类型
 * @tparam Deleter 删除器类型
 * @param x 第一个unique_ptr
 * @param y 第二个unique_ptr
 */
template<typename T, typename Deleter>
void swap(unique_ptr<T, Deleter>& x, unique_ptr<T, Deleter>& y) noexcept {
    x.swap(y);
}

/**
 * @brief 比较两个unique_ptr是否相等
 * 
 * @tparam T1 第一个被管理对象的类型
 * @tparam D1 第一个删除器类型
 * @tparam T2 第二个被管理对象的类型
 * @tparam D2 第二个删除器类型
 * @param x 第一个unique_ptr
 * @param y 第二个unique_ptr
 * @return true 如果两个unique_ptr管理的对象相等
 * @return false 如果两个unique_ptr管理的对象不相等
 */
template<typename T1, typename D1, typename T2, typename D2>
bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return x.get() == y.get();
}

/**
 * @brief 比较两个unique_ptr是否不相等
 * 
 * @tparam T1 第一个被管理对象的类型
 * @tparam D1 第一个删除器类型
 * @tparam T2 第二个被管理对象的类型
 * @tparam D2 第二个删除器类型
 * @param x 第一个unique_ptr
 * @param y 第二个unique_ptr
 * @return true 如果两个unique_ptr管理的对象不相等
 * @return false 如果两个unique_ptr管理的对象相等
 */
template<typename T1, typename D1, typename T2, typename D2>
bool operator!=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return x.get() != y.get();
}

/**
 * @brief 比较unique_ptr与nullptr是否相等
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x unique_ptr
 * @return true 如果unique_ptr不管理任何对象
 * @return false 如果unique_ptr管理对象
 */
template<typename T, typename D>
bool operator==(const unique_ptr<T, D>& x, std::nullptr_t) noexcept {
    return !x;
}

/**
 * @brief 比较nullptr与unique_ptr是否相等
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x unique_ptr
 * @return true 如果unique_ptr不管理任何对象
 * @return false 如果unique_ptr管理对象
 */
template<typename T, typename D>
bool operator==(std::nullptr_t, const unique_ptr<T, D>& x) noexcept {
    return !x;
}

/**
 * @brief 比较unique_ptr与nullptr是否不相等
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x unique_ptr
 * @return true 如果unique_ptr管理对象
 * @return false 如果unique_ptr不管理任何对象
 */
template<typename T, typename D>
bool operator!=(const unique_ptr<T, D>& x, std::nullptr_t) noexcept {
    return static_cast<bool>(x);
}

/**
 * @brief 比较nullptr与unique_ptr是否不相等
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x unique_ptr
 * @return true 如果unique_ptr管理对象
 * @return false 如果unique_ptr不管理任何对象
 */
template<typename T, typename D>
bool operator!=(std::nullptr_t, const unique_ptr<T, D>& x) noexcept {
    return static_cast<bool>(x);
}

/**
 * @brief 比较两个unique_ptr的大小关系（小于）
 * 
 * @tparam T1 第一个被管理对象的类型
 * @tparam D1 第一个删除器类型
 * @tparam T2 第二个被管理对象的类型
 * @tparam D2 第二个删除器类型
 * @param x 第一个unique_ptr
 * @param y 第二个unique_ptr
 * @return true 如果第一个unique_ptr小于第二个
 * @return false 如果第一个unique_ptr不小于第二个
 */
template<typename T1, typename D1, typename T2, typename D2>
bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    using CT = typename std::common_type<typename unique_ptr<T1, D1>::pointer,
                                        typename unique_ptr<T2, D2>::pointer>::type;
    return std::less<CT>()(x.get(), y.get());
}

/**
 * @brief 比较两个unique_ptr的大小关系（小于等于）
 * 
 * @tparam T1 第一个被管理对象的类型
 * @tparam D1 第一个删除器类型
 * @tparam T2 第二个被管理对象的类型
 * @tparam D2 第二个删除器类型
 * @param x 第一个unique_ptr
 * @param y 第二个unique_ptr
 * @return true 如果第一个unique_ptr小于等于第二个
 * @return false 如果第一个unique_ptr大于第二个
 */
template<typename T1, typename D1, typename T2, typename D2>
bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return !(y < x);
}

/**
 * @brief 比较两个unique_ptr的大小关系（大于）
 * 
 * @tparam T1 第一个被管理对象的类型
 * @tparam D1 第一个删除器类型
 * @tparam T2 第二个被管理对象的类型
 * @tparam D2 第二个删除器类型
 * @param x 第一个unique_ptr
 * @param y 第二个unique_ptr
 * @return true 如果第一个unique_ptr大于第二个
 * @return false 如果第一个unique_ptr不大于第二个
 */
template<typename T1, typename D1, typename T2, typename D2>
bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return y < x;
}

/**
 * @brief 比较两个unique_ptr的大小关系（大于等于）
 * 
 * @tparam T1 第一个被管理对象的类型
 * @tparam D1 第一个删除器类型
 * @tparam T2 第二个被管理对象的类型
 * @tparam D2 第二个删除器类型
 * @param x 第一个unique_ptr
 * @param y 第二个unique_ptr
 * @return true 如果第一个unique_ptr大于等于第二个
 * @return false 如果第一个unique_ptr小于第二个
 */
template<typename T1, typename D1, typename T2, typename D2>
bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return !(x < y);
}

/**
 * @brief 比较unique_ptr与nullptr的大小关系（小于）
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x unique_ptr
 * @param y nullptr
 * @return true 如果unique_ptr小于nullptr
 * @return false 如果unique_ptr不小于nullptr
 */
template<typename T, typename D>
bool operator<(const unique_ptr<T, D>& x, std::nullptr_t) {
    return std::less<typename unique_ptr<T, D>::pointer>()(x.get(), nullptr);
}

/**
 * @brief 比较nullptr与unique_ptr的大小关系（小于）
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x nullptr
 * @param y unique_ptr
 * @return true 如果nullptr小于unique_ptr
 * @return false 如果nullptr不小于unique_ptr
 */
template<typename T, typename D>
bool operator<(std::nullptr_t, const unique_ptr<T, D>& y) {
    return std::less<typename unique_ptr<T, D>::pointer>()(nullptr, y.get());
}

/**
 * @brief 比较unique_ptr与nullptr的大小关系（小于等于）
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x unique_ptr
 * @param y nullptr
 * @return true 如果unique_ptr小于等于nullptr
 * @return false 如果unique_ptr大于nullptr
 */
template<typename T, typename D>
bool operator<=(const unique_ptr<T, D>& x, std::nullptr_t y) {
    return !(y < x);
}

/**
 * @brief 比较nullptr与unique_ptr的大小关系（小于等于）
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x nullptr
 * @param y unique_ptr
 * @return true 如果nullptr小于等于unique_ptr
 * @return false 如果nullptr大于unique_ptr
 */
template<typename T, typename D>
bool operator<=(std::nullptr_t x, const unique_ptr<T, D>& y) {
    return !(y < x);
}

/**
 * @brief 比较unique_ptr与nullptr的大小关系（大于）
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x unique_ptr
 * @param y nullptr
 * @return true 如果unique_ptr大于nullptr
 * @return false 如果unique_ptr不大于nullptr
 */
template<typename T, typename D>
bool operator>(const unique_ptr<T, D>& x, std::nullptr_t y) {
    return y < x;
}

/**
 * @brief 比较nullptr与unique_ptr的大小关系（大于）
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x nullptr
 * @param y unique_ptr
 * @return true 如果nullptr大于unique_ptr
 * @return false 如果nullptr不大于unique_ptr
 */
template<typename T, typename D>
bool operator>(std::nullptr_t x, const unique_ptr<T, D>& y) {
    return y < x;
}

/**
 * @brief 比较unique_ptr与nullptr的大小关系（大于等于）
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x unique_ptr
 * @param y nullptr
 * @return true 如果unique_ptr大于等于nullptr
 * @return false 如果unique_ptr小于nullptr
 */
template<typename T, typename D>
bool operator>=(const unique_ptr<T, D>& x, std::nullptr_t y) {
    return !(x < y);
}

/**
 * @brief 比较nullptr与unique_ptr的大小关系（大于等于）
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 * @param x nullptr
 * @param y unique_ptr
 * @return true 如果nullptr大于等于unique_ptr
 * @return false 如果nullptr小于unique_ptr
 */
template<typename T, typename D>
bool operator>=(std::nullptr_t x, const unique_ptr<T, D>& y) {
    return !(x < y);
}

/**
 * @brief 创建unique_ptr
 * 
 * @tparam T 被管理对象的类型
 * @tparam Args 构造参数类型
 * @param args 构造参数
 * @return unique_ptr<T> 创建的unique_ptr
 */
template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, unique_ptr<T>>::type
make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * @brief 创建数组类型的unique_ptr
 * 
 * @tparam T 数组元素类型
 * @param size 数组大小
 * @return unique_ptr<T[]> 创建的unique_ptr数组
 */
template<typename T>
typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, unique_ptr<T>>::type
make_unique(size_t size) {
    using U = typename std::remove_extent<T>::type;
    return unique_ptr<T>(new U[size]());
}

/**
 * @brief 禁用已知数组大小的make_unique版本
 * 
 * @tparam T 数组类型
 * @tparam Args 参数类型
 */
template<typename T, typename... Args>
typename std::enable_if<std::extent<T>::value != 0, void>::type
make_unique(Args&&...) = delete;

/**
 * @brief unique_ptr类的数组特化版本
 * 
 * @tparam T 数组的元素类型
 * @tparam Deleter 删除器类型
 */
template<typename T, typename Deleter>
class unique_ptr<T[], Deleter> {
public:
    /**
     * @brief 类型定义
     */
    using element_type = T;
    using deleter_type = Deleter;
    using pointer = element_type*;

private:
    /**
     * @brief 指向被管理数组的指针
     */
    pointer ptr_;
    
    /**
     * @brief 删除器
     */
    deleter_type deleter_;
    
public:
    /**
     * @brief 默认构造函数
     */
    constexpr unique_ptr() noexcept
        : ptr_(nullptr), deleter_() {}
    
    /**
     * @brief 空指针构造函数
     */
    constexpr unique_ptr(std::nullptr_t) noexcept
        : unique_ptr() {}
    
    /**
     * @brief 指针构造函数
     * 
     * @param p 指向被管理数组的指针
     */
    explicit unique_ptr(pointer p) noexcept
        : ptr_(p), deleter_() {}
    
    /**
     * @brief 指针和删除器构造函数
     * 
     * @param p 指向被管理数组的指针
     * @param d 删除器
     */
    unique_ptr(pointer p, const deleter_type& d) noexcept
        : ptr_(p), deleter_(d) {}
    
    /**
     * @brief 移动构造函数
     * 
     * @param u 另一个unique_ptr
     */
    unique_ptr(unique_ptr&& u) noexcept
        : ptr_(u.ptr_), deleter_(std::move(u.deleter_)) {
        u.ptr_ = nullptr;
    }
    
    /**
     * @brief 析构函数
     */
    ~unique_ptr() {
        if (ptr_) {
            deleter_(ptr_);
        }
    }
    
    /**
     * @brief 禁止复制构造
     */
    unique_ptr(const unique_ptr&) = delete;
    
    /**
     * @brief 禁止复制赋值
     */
    unique_ptr& operator=(const unique_ptr&) = delete;
    
    /**
     * @brief 移动赋值运算符
     * 
     * @param u 另一个unique_ptr
     * @return unique_ptr& 自身引用
     */
    unique_ptr& operator=(unique_ptr&& u) noexcept {
        if (this != &u) {
            if (ptr_) {
                deleter_(ptr_);
            }
            ptr_ = u.ptr_;
            deleter_ = std::move(u.deleter_);
            u.ptr_ = nullptr;
        }
        return *this;
    }
    
    /**
     * @brief 空指针赋值运算符
     * 
     * @param nullp nullptr
     * @return unique_ptr& 自身引用
     */
    unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }
    
    /**
     * @brief 获取被管理数组的指针
     * 
     * @return pointer 指向被管理数组的指针
     */
    pointer get() const noexcept {
        return ptr_;
    }
    
    /**
     * @brief 获取删除器
     * 
     * @return deleter_type& 删除器的引用
     */
    deleter_type& get_deleter() noexcept {
        return deleter_;
    }
    
    /**
     * @brief 获取删除器（常量版本）
     * 
     * @return const deleter_type& 删除器的常量引用
     */
    const deleter_type& get_deleter() const noexcept {
        return deleter_;
    }
    
    /**
     * @brief 转换为布尔类型，检查是否管理对象
     * 
     * @return true 如果管理对象
     * @return false 如果不管理对象
     */
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
    
    /**
     * @brief 数组下标运算符
     * 
     * @param i 下标
     * @return element_type& 数组元素引用
     */
    element_type& operator[](size_t i) const {
        return ptr_[i];
    }
    
    /**
     * @brief 释放所有权，但不销毁对象
     * 
     * @return pointer 被释放的指针
     */
    pointer release() noexcept {
        pointer temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }
    
    /**
     * @brief 重置为新的指针，销毁之前的对象
     * 
     * @param p 新的指针
     */
    void reset(pointer p = pointer()) noexcept {
        pointer temp = ptr_;
        ptr_ = p;
        if (temp) {
            deleter_(temp);
        }
    }
    
    /**
     * @brief 交换两个unique_ptr
     * 
     * @param other 另一个unique_ptr
     */
    void swap(unique_ptr& other) noexcept {
        using std::swap;
        swap(ptr_, other.ptr_);
        swap(deleter_, other.deleter_);
    }
};

// ------------------------------------------------------------------------------------------
// shared_ptr的控制块实现
// ------------------------------------------------------------------------------------------

/**
 * @brief 控制块基类，提供引用计数管理
 */
class control_block_base {
private:
    /**
     * @brief 强引用计数（shared_ptr引用的数量）
     */
    std::atomic<long> shared_count_;
    
    /**
     * @brief 弱引用计数（weak_ptr引用的数量）
     */
    std::atomic<long> weak_count_;

public:
    /**
     * @brief 构造函数
     */
    control_block_base() noexcept
        : shared_count_(1), weak_count_(0) {}
    
    /**
     * @brief 虚析构函数
     */
    virtual ~control_block_base() = default;
    
    /**
     * @brief 增加强引用计数
     * 
     * @return long 增加后的计数值
     */
    long add_shared_ref() noexcept {
        return ++shared_count_;
    }
    
    /**
     * @brief 减少强引用计数
     * 
     * @return long 减少后的计数值
     */
    long release_shared_ref() noexcept {
        return --shared_count_;
    }
    
    /**
     * @brief 增加弱引用计数
     * 
     * @return long 增加后的计数值
     */
    long add_weak_ref() noexcept {
        return ++weak_count_;
    }
    
    /**
     * @brief 减少弱引用计数
     * 
     * @return long 减少后的计数值
     */
    long release_weak_ref() noexcept {
        return --weak_count_;
    }
    
    /**
     * @brief 获取强引用计数
     * 
     * @return long 当前的强引用计数
     */
    long use_count() const noexcept {
        return shared_count_.load();
    }
    
    /**
     * @brief 释放对象（由派生类实现）
     */
    virtual void destroy_object() noexcept = 0;
    
    /**
     * @brief 获取删除器的类型ID（由派生类实现）
     * 
     * @return const std::type_info& 删除器的类型信息
     */
    virtual const std::type_info& get_deleter_type() const noexcept = 0;
    
    /**
     * @brief 获取删除器指针（由派生类实现）
     * 
     * @return void* 删除器的指针
     */
    virtual void* get_deleter() noexcept = 0;
};

/**
 * @brief 具体的控制块实现，包含具体类型的对象和删除器
 * 
 * @tparam T 被管理对象的类型
 * @tparam Deleter 删除器类型
 */
template<typename T, typename Deleter>
class control_block : public control_block_base {
private:
    /**
     * @brief 被管理的对象
     */
    T* ptr_;
    
    /**
     * @brief 删除器
     */
    Deleter deleter_;

public:
    /**
     * @brief 构造函数
     * 
     * @param p 被管理对象的指针
     * @param d 删除器
     */
    control_block(T* p, Deleter d) noexcept
        : control_block_base(), ptr_(p), deleter_(std::move(d)) {}
    
    /**
     * @brief 实现销毁对象函数
     */
    void destroy_object() noexcept override {
        if (ptr_) {
            deleter_(ptr_);
            ptr_ = nullptr;
        }
    }
    
    /**
     * @brief 实现获取删除器类型函数
     * 
     * @return const std::type_info& 删除器的类型信息
     */
    const std::type_info& get_deleter_type() const noexcept override {
        return typeid(Deleter);
    }
    
    /**
     * @brief 实现获取删除器指针函数
     * 
     * @return void* 删除器的指针
     */
    void* get_deleter() noexcept override {
        return &deleter_;
    }
};

/**
 * @brief 内部类型Inplace控制块，直接在控制块内构造对象，避免两次内存分配
 * 
 * @tparam T 被管理对象的类型
 */
template<typename T>
class inplace_control_block : public control_block_base {
private:
    /**
     * @brief 存储对象的内存
     */
    alignas(T) unsigned char object_storage_[sizeof(T)];
    
    /**
     * @brief 获取对象指针
     * 
     * @return T* 对象指针
     */
    T* get_object() noexcept {
        return reinterpret_cast<T*>(object_storage_);
    }

public:
    /**
     * @brief 构造函数，原位构造对象
     * 
     * @tparam Args 构造参数类型
     * @param args 构造参数
     */
    template<typename... Args>
    inplace_control_block(Args&&... args) {
        new (object_storage_) T(std::forward<Args>(args)...);
    }
    
    /**
     * @brief 析构函数
     */
    ~inplace_control_block() {
        // 在destroy_object中处理对象的析构
    }
    
    /**
     * @brief 实现销毁对象函数
     */
    void destroy_object() noexcept override {
        get_object()->~T();
    }
    
    /**
     * @brief 实现获取删除器类型函数
     * 
     * @return const std::type_info& 删除器的类型信息
     */
    const std::type_info& get_deleter_type() const noexcept override {
        return typeid(void);
    }
    
    /**
     * @brief 实现获取删除器指针函数
     * 
     * @return void* 删除器的指针
     */
    void* get_deleter() noexcept override {
        return nullptr;
    }
    
    /**
     * @brief 获取对象指针
     * 
     * @return T* 对象指针
     */
    T* get_ptr() noexcept {
        return get_object();
    }
};

// ------------------------------------------------------------------------------------------
// shared_ptr类 - 共享所有权的智能指针
// ------------------------------------------------------------------------------------------

/**
 * @brief shared_ptr类，提供共享所有权的智能指针功能
 * 
 * @tparam T 被管理对象的类型
 */
template<typename T>
class shared_ptr {
public:
    /**
     * @brief 类型定义
     */
    using element_type = typename std::remove_extent<T>::type;
    using weak_type = weak_ptr<T>;

private:
    /**
     * @brief 指向被管理对象的指针
     */
    element_type* ptr_ = nullptr;
    
    /**
     * @brief 指向控制块的指针
     */
    control_block_base* control_block_ = nullptr;
    
    /**
     * @brief 增加引用计数
     */
    void increment_ref_count() noexcept {
        if (control_block_) {
            control_block_->add_shared_ref();
        }
    }
    
    /**
     * @brief 减少引用计数，必要时释放资源
     */
    void decrement_ref_count() noexcept {
        if (control_block_) {
            if (control_block_->release_shared_ref() == 0) {
                control_block_->destroy_object();
                if (control_block_->release_weak_ref() == 0) {
                    delete control_block_;
                }
            }
        }
    }
    
    /**
     * @brief 从weak_ptr构造shared_ptr的辅助函数
     * 
     * @param r weak_ptr
     */
    void construct_from_weak(const weak_ptr<T>& r) {
        if (r.expired()) {
            throw std::bad_weak_ptr();
        }
        ptr_ = r.ptr_;
        control_block_ = r.control_block_;
        increment_ref_count();
    }
    
    /**
     * @brief 友元类声明
     */
    template<typename U> friend class shared_ptr;
    template<typename U> friend class weak_ptr;
    
    /**
     * @brief make_shared函数需要访问private成员
     */
    template<typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&... args);

public:
    /**
     * @brief 默认构造函数
     */
    constexpr shared_ptr() noexcept = default;
    
    /**
     * @brief 空指针构造函数
     */
    constexpr shared_ptr(std::nullptr_t) noexcept : shared_ptr() {}
    
    /**
     * @brief 指针构造函数
     * 
     * @param p 指向被管理对象的指针
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    explicit shared_ptr(U* p) {
        try {
            control_block_ = new control_block<U, default_delete<U>>(p, default_delete<U>());
            ptr_ = p;
        } catch (...) {
            delete p;
            throw;
        }
    }
    
    /**
     * @brief 指针和删除器构造函数
     * 
     * @tparam U 被管理对象的类型
     * @tparam D 删除器类型
     * @param p 指向被管理对象的指针
     * @param d 删除器
     */
    template<typename U, typename D,
             typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    shared_ptr(U* p, D d) {
        try {
            control_block_ = new control_block<U, D>(p, std::move(d));
            ptr_ = p;
        } catch (...) {
            d(p);
            throw;
        }
    }
    
    /**
     * @brief 复制构造函数
     * 
     * @param other 另一个shared_ptr
     */
    shared_ptr(const shared_ptr& other) noexcept
        : ptr_(other.ptr_), control_block_(other.control_block_) {
        increment_ref_count();
    }
    
    /**
     * @brief 移动构造函数
     * 
     * @param other 另一个shared_ptr
     */
    shared_ptr(shared_ptr&& other) noexcept
        : ptr_(other.ptr_), control_block_(other.control_block_) {
        other.ptr_ = nullptr;
        other.control_block_ = nullptr;
    }
    
    /**
     * @brief 转换复制构造函数
     * 
     * @tparam U 另一个类型，必须可转换为T
     * @param other 另一个shared_ptr
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    shared_ptr(const shared_ptr<U>& other) noexcept
        : ptr_(other.ptr_), control_block_(other.control_block_) {
        increment_ref_count();
    }
    
    /**
     * @brief 转换移动构造函数
     * 
     * @tparam U 另一个类型，必须可转换为T
     * @param other 另一个shared_ptr
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    shared_ptr(shared_ptr<U>&& other) noexcept
        : ptr_(other.ptr_), control_block_(other.control_block_) {
        other.ptr_ = nullptr;
        other.control_block_ = nullptr;
    }
    
    /**
     * @brief 从weak_ptr构造
     * 
     * @param r weak_ptr
     */
    explicit shared_ptr(const weak_ptr<T>& r) {
        construct_from_weak(r);
    }
    
    /**
     * @brief 析构函数
     */
    ~shared_ptr() {
        decrement_ref_count();
    }
    
    /**
     * @brief 复制赋值运算符
     * 
     * @param other 另一个shared_ptr
     * @return shared_ptr& 自身引用
     */
    shared_ptr& operator=(const shared_ptr& other) noexcept {
        shared_ptr(other).swap(*this);
        return *this;
    }
    
    /**
     * @brief 移动赋值运算符
     * 
     * @param other 另一个shared_ptr
     * @return shared_ptr& 自身引用
     */
    shared_ptr& operator=(shared_ptr&& other) noexcept {
        shared_ptr(std::move(other)).swap(*this);
        return *this;
    }
    
    /**
     * @brief 模板复制赋值运算符
     * 
     * @tparam U 另一个类型，必须可转换为T
     * @param other 另一个shared_ptr
     * @return shared_ptr& 自身引用
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    shared_ptr& operator=(const shared_ptr<U>& other) noexcept {
        shared_ptr(other).swap(*this);
        return *this;
    }
    
    /**
     * @brief 模板移动赋值运算符
     * 
     * @tparam U 另一个类型，必须可转换为T
     * @param other 另一个shared_ptr
     * @return shared_ptr& 自身引用
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    shared_ptr& operator=(shared_ptr<U>&& other) noexcept {
        shared_ptr(std::move(other)).swap(*this);
        return *this;
    }
    
    /**
     * @brief 释放被管理对象并重置为空
     */
    void reset() noexcept {
        shared_ptr().swap(*this);
    }
    
    /**
     * @brief 重置shared_ptr为新的对象
     * 
     * @tparam U 被管理对象的类型
     * @param p 指向被管理对象的指针
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    void reset(U* p) {
        shared_ptr(p).swap(*this);
    }
    
    /**
     * @brief 重置shared_ptr为新的对象，带自定义删除器
     * 
     * @tparam U 被管理对象的类型
     * @tparam D 删除器类型
     * @param p 指向被管理对象的指针
     * @param d 删除器
     */
    template<typename U, typename D,
             typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    void reset(U* p, D d) {
        shared_ptr(p, d).swap(*this);
    }
    
    /**
     * @brief 交换两个shared_ptr
     * 
     * @param other 另一个shared_ptr
     */
    void swap(shared_ptr& other) noexcept {
        using std::swap;
        swap(ptr_, other.ptr_);
        swap(control_block_, other.control_block_);
    }
    
    /**
     * @brief 获取被管理对象的指针
     * 
     * @return element_type* 被管理对象的指针
     */
    element_type* get() const noexcept {
        return ptr_;
    }
    
    /**
     * @brief 获取引用计数
     * 
     * @return long 引用计数
     */
    long use_count() const noexcept {
        return control_block_ ? control_block_->use_count() : 0;
    }
    
    /**
     * @brief 检查是否是唯一拥有者
     * 
     * @return true 如果是唯一拥有者
     * @return false 如果不是唯一拥有者或不拥有对象
     */
    bool unique() const noexcept {
        return use_count() == 1;
    }
    
    /**
     * @brief 检查是否拥有对象
     * 
     * @return true 如果拥有对象
     * @return false 如果不拥有对象
     */
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
    
    /**
     * @brief 获取删除器
     * 
     * @tparam D 删除器类型
     * @return D* 删除器指针
     */
    template<typename D>
    D* get_deleter() const noexcept {
        if (control_block_ && control_block_->get_deleter_type() == typeid(D)) {
            return static_cast<D*>(control_block_->get_deleter());
        }
        return nullptr;
    }
    
    /**
     * @brief 解引用运算符
     * 
     * @return element_type& 被管理对象的引用
     */
    typename std::add_lvalue_reference<element_type>::type operator*() const noexcept {
        return *ptr_;
    }
    
    /**
     * @brief 成员访问运算符
     * 
     * @return element_type* 被管理对象的指针
     */
    element_type* operator->() const noexcept {
        return ptr_;
    }
    
    /**
     * @brief 数组下标运算符（仅针对数组类型）
     * 
     * @param i 下标
     * @return element_type& 数组元素引用
     */
    template<typename U = T, typename = typename std::enable_if<std::is_array<U>::value>::type>
    typename std::add_lvalue_reference<element_type>::type operator[](std::ptrdiff_t i) const {
        return ptr_[i];
    }
};

/**
 * @brief 比较两个shared_ptr是否相等
 * 
 * @tparam T1 第一个shared_ptr的类型
 * @tparam T2 第二个shared_ptr的类型
 * @param lhs 左侧shared_ptr
 * @param rhs 右侧shared_ptr
 * @return true 如果两个shared_ptr管理同一个对象或都为空
 * @return false 如果两个shared_ptr管理不同的对象
 */
template<typename T1, typename T2>
bool operator==(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs) noexcept {
    return lhs.get() == rhs.get();
}

/**
 * @brief 比较两个shared_ptr是否不相等
 * 
 * @tparam T1 第一个shared_ptr的类型
 * @tparam T2 第二个shared_ptr的类型
 * @param lhs 左侧shared_ptr
 * @param rhs 右侧shared_ptr
 * @return true 如果两个shared_ptr管理不同的对象
 * @return false 如果两个shared_ptr管理同一个对象或都为空
 */
template<typename T1, typename T2>
bool operator!=(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs) noexcept {
    return !(lhs == rhs);
}

/**
 * @brief 比较shared_ptr与nullptr
 * 
 * @tparam T shared_ptr的类型
 * @param lhs shared_ptr
 * @param nullp nullptr
 * @return true 如果shared_ptr为空
 * @return false 如果shared_ptr不为空
 */
template<typename T>
bool operator==(const shared_ptr<T>& lhs, std::nullptr_t) noexcept {
    return !lhs;
}

/**
 * @brief 比较nullptr与shared_ptr
 * 
 * @tparam T shared_ptr的类型
 * @param nullp nullptr
 * @param rhs shared_ptr
 * @return true 如果shared_ptr为空
 * @return false 如果shared_ptr不为空
 */
template<typename T>
bool operator==(std::nullptr_t, const shared_ptr<T>& rhs) noexcept {
    return !rhs;
}

/**
 * @brief 比较shared_ptr与nullptr是否不相等
 * 
 * @tparam T shared_ptr的类型
 * @param lhs shared_ptr
 * @param nullp nullptr
 * @return true 如果shared_ptr不为空
 * @return false 如果shared_ptr为空
 */
template<typename T>
bool operator!=(const shared_ptr<T>& lhs, std::nullptr_t) noexcept {
    return static_cast<bool>(lhs);
}

/**
 * @brief 比较nullptr与shared_ptr是否不相等
 * 
 * @tparam T shared_ptr的类型
 * @param nullp nullptr
 * @param rhs shared_ptr
 * @return true 如果shared_ptr不为空
 * @return false 如果shared_ptr为空
 */
template<typename T>
bool operator!=(std::nullptr_t, const shared_ptr<T>& rhs) noexcept {
    return static_cast<bool>(rhs);
}

/**
 * @brief 小于比较运算符
 * 
 * @tparam T1 第一个shared_ptr的类型
 * @tparam T2 第二个shared_ptr的类型
 * @param lhs 左侧shared_ptr
 * @param rhs 右侧shared_ptr
 * @return true 如果lhs.get() < rhs.get()
 * @return false 如果lhs.get() >= rhs.get()
 */
template<typename T1, typename T2>
bool operator<(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs) noexcept {
    using CT = typename std::common_type<typename shared_ptr<T1>::element_type*, 
                                         typename shared_ptr<T2>::element_type*>::type;
    return std::less<CT>()(lhs.get(), rhs.get());
}

/**
 * @brief 小于等于比较运算符
 * 
 * @tparam T1 第一个shared_ptr的类型
 * @tparam T2 第二个shared_ptr的类型
 * @param lhs 左侧shared_ptr
 * @param rhs 右侧shared_ptr
 * @return true 如果lhs.get() <= rhs.get()
 * @return false 如果lhs.get() > rhs.get()
 */
template<typename T1, typename T2>
bool operator<=(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs) noexcept {
    return !(rhs < lhs);
}

/**
 * @brief 大于比较运算符
 * 
 * @tparam T1 第一个shared_ptr的类型
 * @tparam T2 第二个shared_ptr的类型
 * @param lhs 左侧shared_ptr
 * @param rhs 右侧shared_ptr
 * @return true 如果lhs.get() > rhs.get()
 * @return false 如果lhs.get() <= rhs.get()
 */
template<typename T1, typename T2>
bool operator>(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs) noexcept {
    return rhs < lhs;
}

/**
 * @brief 大于等于比较运算符
 * 
 * @tparam T1 第一个shared_ptr的类型
 * @tparam T2 第二个shared_ptr的类型
 * @param lhs 左侧shared_ptr
 * @param rhs 右侧shared_ptr
 * @return true 如果lhs.get() >= rhs.get()
 * @return false 如果lhs.get() < rhs.get()
 */
template<typename T1, typename T2>
bool operator>=(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs) noexcept {
    return !(lhs < rhs);
}

/**
 * @brief 创建shared_ptr
 * 
 * @tparam T 被管理对象的类型
 * @tparam Args 构造参数类型
 * @param args 构造参数
 * @return shared_ptr<T> 创建的shared_ptr
 */
template<typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {
    auto control_block = new inplace_control_block<T>(std::forward<Args>(args)...);
    shared_ptr<T> result;
    result.ptr_ = control_block->get_ptr();
    result.control_block_ = control_block;
    return result;
}

// ------------------------------------------------------------------------------------------
// weak_ptr类 - 弱引用智能指针
// ------------------------------------------------------------------------------------------

/**
 * @brief weak_ptr类，提供弱引用智能指针功能
 * 
 * @tparam T 被引用对象的类型
 */
template<typename T>
class weak_ptr {
public:
    /**
     * @brief 类型定义
     */
    using element_type = typename std::remove_extent<T>::type;

private:
    /**
     * @brief 指向被引用对象的指针
     */
    element_type* ptr_ = nullptr;
    
    /**
     * @brief 指向控制块的指针
     */
    control_block_base* control_block_ = nullptr;
    
    /**
     * @brief 增加弱引用计数
     */
    void increment_weak_count() noexcept {
        if (control_block_) {
            control_block_->add_weak_ref();
        }
    }
    
    /**
     * @brief 减少弱引用计数，必要时释放资源
     */
    void decrement_weak_count() noexcept {
        if (control_block_) {
            if (control_block_->release_weak_ref() == 0 && control_block_->use_count() == 0) {
                delete control_block_;
            }
        }
    }
    
    /**
     * @brief 友元类声明
     */
    template<typename U> friend class shared_ptr;
    template<typename U> friend class weak_ptr;

public:
    /**
     * @brief 默认构造函数
     */
    constexpr weak_ptr() noexcept = default;
    
    /**
     * @brief 复制构造函数
     * 
     * @param other 另一个weak_ptr
     */
    weak_ptr(const weak_ptr& other) noexcept
        : ptr_(other.ptr_), control_block_(other.control_block_) {
        increment_weak_count();
    }
    
    /**
     * @brief 移动构造函数
     * 
     * @param other 另一个weak_ptr
     */
    weak_ptr(weak_ptr&& other) noexcept
        : ptr_(other.ptr_), control_block_(other.control_block_) {
        other.ptr_ = nullptr;
        other.control_block_ = nullptr;
    }
    
    /**
     * @brief 从shared_ptr构造
     * 
     * @param other shared_ptr
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    weak_ptr(const shared_ptr<U>& other) noexcept
        : ptr_(other.ptr_), control_block_(other.control_block_) {
        increment_weak_count();
    }
    
    /**
     * @brief 转换复制构造函数
     * 
     * @tparam U 另一个类型，必须可转换为T
     * @param other 另一个weak_ptr
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    weak_ptr(const weak_ptr<U>& other) noexcept
        : ptr_(other.ptr_), control_block_(other.control_block_) {
        increment_weak_count();
    }
    
    /**
     * @brief 转换移动构造函数
     * 
     * @tparam U 另一个类型，必须可转换为T
     * @param other 另一个weak_ptr
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    weak_ptr(weak_ptr<U>&& other) noexcept
        : ptr_(other.ptr_), control_block_(other.control_block_) {
        other.ptr_ = nullptr;
        other.control_block_ = nullptr;
    }
    
    /**
     * @brief 析构函数
     */
    ~weak_ptr() {
        decrement_weak_count();
    }
    
    /**
     * @brief 复制赋值运算符
     * 
     * @param other 另一个weak_ptr
     * @return weak_ptr& 自身引用
     */
    weak_ptr& operator=(const weak_ptr& other) noexcept {
        weak_ptr(other).swap(*this);
        return *this;
    }
    
    /**
     * @brief 移动赋值运算符
     * 
     * @param other 另一个weak_ptr
     * @return weak_ptr& 自身引用
     */
    weak_ptr& operator=(weak_ptr&& other) noexcept {
        weak_ptr(std::move(other)).swap(*this);
        return *this;
    }
    
    /**
     * @brief 从shared_ptr赋值
     * 
     * @param other shared_ptr
     * @return weak_ptr& 自身引用
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    weak_ptr& operator=(const shared_ptr<U>& other) noexcept {
        weak_ptr(other).swap(*this);
        return *this;
    }
    
    /**
     * @brief 模板复制赋值运算符
     * 
     * @tparam U 另一个类型，必须可转换为T
     * @param other 另一个weak_ptr
     * @return weak_ptr& 自身引用
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    weak_ptr& operator=(const weak_ptr<U>& other) noexcept {
        weak_ptr(other).swap(*this);
        return *this;
    }
    
    /**
     * @brief 模板移动赋值运算符
     * 
     * @tparam U 另一个类型，必须可转换为T
     * @param other 另一个weak_ptr
     * @return weak_ptr& 自身引用
     */
    template<typename U, typename = typename std::enable_if<std::is_convertible<U*, element_type*>::value>::type>
    weak_ptr& operator=(weak_ptr<U>&& other) noexcept {
        weak_ptr(std::move(other)).swap(*this);
        return *this;
    }
    
    /**
     * @brief 重置weak_ptr为空
     */
    void reset() noexcept {
        weak_ptr().swap(*this);
    }
    
    /**
     * @brief 交换两个weak_ptr
     * 
     * @param other 另一个weak_ptr
     */
    void swap(weak_ptr& other) noexcept {
        using std::swap;
        swap(ptr_, other.ptr_);
        swap(control_block_, other.control_block_);
    }
    
    /**
     * @brief 获取引用计数
     * 
     * @return long 引用计数
     */
    long use_count() const noexcept {
        return control_block_ ? control_block_->use_count() : 0;
    }
    
    /**
     * @brief 检查被引用对象是否已被销毁
     * 
     * @return true 如果被引用对象已被销毁
     * @return false 如果被引用对象仍存在
     */
    bool expired() const noexcept {
        return use_count() == 0;
    }
    
    /**
     * @brief 尝试获取shared_ptr
     * 
     * @return shared_ptr<T> 获取的shared_ptr，如果对象已过期则为空
     */
    shared_ptr<T> lock() const noexcept {
        shared_ptr<T> result;
        if (!expired()) {
            try {
                result = shared_ptr<T>(*this);
            } catch (...) {
                // 如果构造失败，返回空shared_ptr
            }
        }
        return result;
    }
};

/**
 * @brief 交换两个weak_ptr
 * 
 * @tparam T weak_ptr的类型
 * @param lhs 第一个weak_ptr
 * @param rhs 第二个weak_ptr
 */
template<typename T>
void swap(weak_ptr<T>& lhs, weak_ptr<T>& rhs) noexcept {
    lhs.swap(rhs);
}

/**
 * @brief 别名模板，提供自定义删除器和分配器的功能
 * 
 * @tparam T 被管理对象的类型
 * @tparam D 删除器类型
 */
template<typename T, typename D = default_delete<T>>
using unique_ptr_t = unique_ptr<T, D>;

/**
 * @brief 获取删除器
 * 
 * @tparam D 删除器类型
 * @tparam T 智能指针管理的对象类型
 * @param sp 共享指针
 * @return D* 删除器指针，如果类型不匹配则返回nullptr
 */
template<typename D, typename T>
D* get_deleter(const shared_ptr<T>& sp) noexcept {
    return sp.template get_deleter<D>();
}

} // namespace mystl

#endif // MYSTL_SMART_POINTER_H_ 