#ifndef MY_STACK_H
#define MY_STACK_H

// 这个头文件包含了一个模板类 stack
// stack : 栈

/**
 * @file my_stack.h
 * @brief 实现了栈容器适配器
 * 
 * @details 栈是一种LIFO(后进先出)的数据结构，只能在一端(栈顶)进行元素的插入和删除操作。
 * 栈容器适配器封装了底层容器，提供了一组特定的成员函数来访问其元素。
 * 底层容器默认为deque，也可以指定为任何提供了特定接口的序列容器。
 */

#include "../my_deque/my_deque.h"    
#include <type_traits>  // 用于静态断言
#include <algorithm>    // 用于std::swap

namespace mystl
{

/**
 * @brief 栈容器适配器模板类
 * 
 * @tparam T 元素类型
 * @tparam Container 底层容器类型，默认为mystl::deque<T>
 */
template <class T, class Container = mystl::deque<T>>
class stack
{
public:
    // 容器类型定义
    typedef Container                           container_type;
    // 使用底层容器的型别
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;

    // 静态断言，确保T与底层容器value_type相同
    static_assert(std::is_same<T, value_type>::value,
                "the value_type of Container should be same with T");

private:
    container_type c_;  // 用底层容器表现 stack

public:
    // 构造、复制、移动函数
    
    /**
     * @brief 默认构造函数
     */
    stack() = default;

    /**
     * @brief 构造包含n个元素的栈
     * @param n 元素数量
     */
    explicit stack(size_type n) 
        :c_(n)
    {
    }
    
    /**
     * @brief 构造包含n个值为value的元素的栈
     * @param n 元素数量
     * @param value 元素值
     */
    stack(size_type n, const value_type& value) 
        :c_(n, value)
    {
    }

    /**
     * @brief 从迭代器范围构造栈
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    template <class IIter>
    stack(IIter first, IIter last)
        : c_(first, last)
    {
    }

    /**
     * @brief 从初始化列表构造栈
     * @param ilist 初始化列表
     */
    stack(std::initializer_list<T> ilist) 
        :c_(ilist.begin(), ilist.end())
    {
    }

    /**
     * @brief 从底层容器构造栈
     * @param c 底层容器
     */
    stack(const Container& c)
        :c_(c)
    {
    }
    
    /**
     * @brief 从底层容器移动构造栈
     * @param c 底层容器(右值引用)
     */
    stack(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
        :c_(std::move(c)) 
    {
    }

    /**
     * @brief 拷贝构造函数
     * @param rhs 源栈对象
     */
    stack(const stack& rhs) 
        :c_(rhs.c_) 
    {
    }
    
    /**
     * @brief 移动构造函数
     * @param rhs 源栈对象(右值引用)
     */
    stack(stack&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
        :c_(std::move(rhs.c_))
    {
    }

    /**
     * @brief 拷贝赋值运算符
     * @param rhs 源栈对象
     * @return 当前栈引用
     */
    stack& operator=(const stack& rhs)
    {
        c_ = rhs.c_;
        return *this;
    }
    
    /**
     * @brief 移动赋值运算符
     * @param rhs 源栈对象(右值引用) 
     * @return 当前栈引用
     */
    stack& operator=(stack&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value)
    { 
        c_ = std::move(rhs.c_); 
        return *this;
    }

    /**
     * @brief 从初始化列表赋值
     * @param ilist 初始化列表
     * @return 当前栈引用
     */
    stack& operator=(std::initializer_list<T> ilist) 
    {
        c_ = ilist; 
        return *this;
    }

    /**
     * @brief 析构函数
     */
    ~stack() = default;

    // 访问元素相关操作
    
    /**
     * @brief 获取栈顶元素
     * @return 栈顶元素引用
     */
    reference       top()       { return c_.back(); }
    
    /**
     * @brief 获取栈顶元素(常量版本)
     * @return 栈顶元素常量引用
     */
    const_reference top() const { return c_.back(); }

    // 容量相关操作
    
    /**
     * @brief 检查栈是否为空
     * @return 如果栈为空则返回true，否则返回false
     */
    [[nodiscard]] bool empty() const noexcept { return c_.empty(); }
    
    /**
     * @brief 返回栈中的元素个数
     * @return 元素个数
     */
    size_type size()  const noexcept { return c_.size(); }

    // 修改容器相关操作

    /**
     * @brief 原地构造元素并添加到栈顶
     * @param args 构造参数
     */
    template <class... Args>
    void emplace(Args&&... args)
    { c_.emplace_back(std::forward<Args>(args)...); }

    /**
     * @brief 将元素添加到栈顶
     * @param value 要添加的元素值
     */
    void push(const value_type& value)
    { c_.push_back(value); }
    
    /**
     * @brief 将元素添加到栈顶(移动版本)
     * @param value 要添加的元素值(右值引用)
     */
    void push(value_type&& value)      
    { c_.push_back(std::move(value)); }

    /**
     * @brief 移除栈顶元素
     */
    void pop() 
    { c_.pop_back(); }

    /**
     * @brief 清空栈
     */
    void clear() 
    {
        while (!empty())
            pop();
    }

    /**
     * @brief 交换两个栈的内容
     * @param rhs 要交换的栈
     */
    void swap(stack& rhs) noexcept(noexcept(std::swap(c_, rhs.c_)))
    { 
        using std::swap;
        swap(c_, rhs.c_);
    }

public:
    /**
     * @brief 相等比较运算符
     * @param lhs 左操作数
     * @param rhs 右操作数
     * @return 如果两个栈相等则返回true，否则返回false
     */
    friend bool operator==(const stack& lhs, const stack& rhs) { return lhs.c_ == rhs.c_; }
    
    /**
     * @brief 小于比较运算符
     * @param lhs 左操作数
     * @param rhs 右操作数
     * @return 如果左栈小于右栈则返回true，否则返回false
     */
    friend bool operator< (const stack& lhs, const stack& rhs) { return lhs.c_ <  rhs.c_; }
};

// 重载比较操作符

/**
 * @brief 相等比较运算符外部重载
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果两个栈相等则返回true，否则返回false
 */
template <class T, class Container>
bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
    return lhs == rhs;
}

/**
 * @brief 小于比较运算符外部重载
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果左栈小于右栈则返回true，否则返回false
 */
template <class T, class Container>
bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
    return lhs < rhs;
}

/**
 * @brief 不等比较运算符
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果两个栈不相等则返回true，否则返回false
 */
template <class T, class Container>
bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
    return !(lhs == rhs);
}

/**
 * @brief 大于比较运算符
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果左栈大于右栈则返回true，否则返回false
 */
template <class T, class Container>
bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
    return rhs < lhs;
}

/**
 * @brief 小于等于比较运算符
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果左栈小于等于右栈则返回true，否则返回false
 */
template <class T, class Container>
bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
    return !(rhs < lhs);
}

/**
 * @brief 大于等于比较运算符
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果左栈大于等于右栈则返回true，否则返回false
 */
template <class T, class Container>
bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
    return !(lhs < rhs);
}

/**
 * @brief 交换两个栈的内容(全局函数)
 * @param lhs 栈1
 * @param rhs 栈2
 */
template <class T, class Container>
void swap(stack<T, Container>& lhs, stack<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

} // namespace mystl

#endif // MY_STACK_H 