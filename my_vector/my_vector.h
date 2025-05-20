#ifndef MY_VECTOR_H
#define MY_VECTOR_H

/**
 * @file my_vector.h
 * @brief 实现一个STL的vector容器
 * 
 * 该文件包含一个模板类vector，是对连续内存线性容器的实现
 */

#include <initializer_list>
#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include <memory>
#include <utility>
#include <limits>
#include <iterator>

namespace mystl {

// 自定义实现uninitialized_move函数，用于C++11环境
template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_move(InputIterator first, InputIterator last, ForwardIterator result) {
    for (; first != last; ++first, ++result) {
        ::new (static_cast<void*>(std::addressof(*result))) 
            typename std::iterator_traits<ForwardIterator>::value_type(std::move(*first));
    }
    return result;
}

/**
 * @brief vector容器类的实现
 * 
 * @tparam T 存储元素的类型
 */
template <class T>
class vector {
    // 禁用vector<bool>的特殊实现
    static_assert(!std::is_same<bool, T>::value, "vector<bool>在本实现中不被支持");

public:
    // vector的类型定义
    typedef T                                   value_type;
    typedef T*                                  pointer;
    typedef const T*                            const_pointer;
    typedef T&                                  reference;
    typedef const T&                            const_reference;
    typedef T*                                  iterator;
    typedef const T*                            const_iterator;
    typedef std::reverse_iterator<iterator>     reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef size_t                              size_type;
    typedef ptrdiff_t                           difference_type;
    typedef std::allocator<T>                   allocator_type;
    typedef std::allocator<T>                   data_allocator;

private:
    iterator begin_;  // 使用空间的起始位置
    iterator end_;    // 使用空间的结束位置（最后一个元素之后的位置）
    iterator cap_;    // 存储空间的结束位置

public:
    /**
     * @brief 获取容器使用的分配器
     * @return 返回分配器
     */
    allocator_type get_allocator() const noexcept { 
        return data_allocator(); 
    }

    // 构造函数、析构函数和赋值运算符

    /**
     * @brief 默认构造函数
     * 
     * 创建一个空的vector
     */
    vector() noexcept {
        try_init();
    }

    /**
     * @brief 指定大小的构造函数
     * 
     * @param n 元素个数
     */
    explicit vector(size_type n) {
        fill_init(n, value_type());
    }

    /**
     * @brief 指定大小和初始值的构造函数
     * 
     * @param n 元素个数
     * @param value 初始值
     */
    vector(size_type n, const value_type& value) {
        fill_init(n, value);
    }

    /**
     * @brief 迭代器范围构造函数
     * 
     * @tparam Iter 迭代器类型
     * @param first 起始迭代器
     * @param last 终止迭代器
     */
    template <class Iter, typename std::enable_if<
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, 
        std::input_iterator_tag>::value, int>::type = 0>
    vector(Iter first, Iter last) {
        range_init(first, last);
    }

    /**
     * @brief 拷贝构造函数
     * 
     * @param rhs 源vector
     */
    vector(const vector& rhs) {
        range_init(rhs.begin_, rhs.end_);
    }

    /**
     * @brief 移动构造函数
     * 
     * @param rhs 源vector
     */
    vector(vector&& rhs) noexcept
        :begin_(rhs.begin_), end_(rhs.end_), cap_(rhs.cap_) {
        rhs.begin_ = nullptr;
        rhs.end_ = nullptr;
        rhs.cap_ = nullptr;
    }

    /**
     * @brief 初始化列表构造函数
     * 
     * @param ilist 初始化列表
     */
    vector(std::initializer_list<value_type> ilist) {
        range_init(ilist.begin(), ilist.end());
    }

    /**
     * @brief 析构函数
     */
    ~vector() {
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = end_ = cap_ = nullptr;
    }

    /**
     * @brief 拷贝赋值运算符
     * 
     * @param rhs 源vector
     * @return 返回自身引用
     */
    vector& operator=(const vector& rhs);

    /**
     * @brief 移动赋值运算符
     * 
     * @param rhs 源vector
     * @return 返回自身引用
     */
    vector& operator=(vector&& rhs) noexcept;

    /**
     * @brief 初始化列表赋值运算符
     * 
     * @param ilist 初始化列表
     * @return 返回自身引用
     */
    vector& operator=(std::initializer_list<value_type> ilist) {
        vector tmp(ilist.begin(), ilist.end());
        swap(tmp);
        return *this;
    }

    // 迭代器相关操作

    /**
     * @brief 返回容器第一个元素的迭代器
     * @return 第一个元素的迭代器
     */
    iterator begin() noexcept { 
        return begin_; 
    }

    /**
     * @brief 返回容器第一个元素的常量迭代器
     * @return 第一个元素的常量迭代器
     */
    const_iterator begin() const noexcept { 
        return begin_; 
    }

    /**
     * @brief 返回容器最后一个元素之后位置的迭代器
     * @return 最后元素之后位置的迭代器
     */
    iterator end() noexcept { 
        return end_; 
    }

    /**
     * @brief 返回容器最后一个元素之后位置的常量迭代器
     * @return 最后元素之后位置的常量迭代器
     */
    const_iterator end() const noexcept { 
        return end_; 
    }

    /**
     * @brief 返回容器反向迭代器的起始位置（对应容器的最后一个元素）
     * @return 反向迭代器
     */
    reverse_iterator rbegin() noexcept { 
        return reverse_iterator(end()); 
    }

    /**
     * @brief 返回容器反向迭代器的起始位置的常量版本
     * @return 常量反向迭代器
     */
    const_reverse_iterator rbegin() const noexcept { 
        return const_reverse_iterator(end()); 
    }

    /**
     * @brief 返回容器反向迭代器的结束位置（对应容器的第一个元素之前的位置）
     * @return 反向迭代器
     */
    reverse_iterator rend() noexcept { 
        return reverse_iterator(begin()); 
    }

    /**
     * @brief 返回容器反向迭代器的结束位置的常量版本
     * @return 常量反向迭代器
     */
    const_reverse_iterator rend() const noexcept { 
        return const_reverse_iterator(begin()); 
    }

    /**
     * @brief 返回常量迭代器，指向容器第一个元素
     * @return 常量迭代器
     */
    const_iterator cbegin() const noexcept { 
        return begin(); 
    }

    /**
     * @brief 返回常量迭代器，指向容器最后一个元素之后的位置
     * @return 常量迭代器
     */
    const_iterator cend() const noexcept { 
        return end(); 
    }

    /**
     * @brief 返回常量反向迭代器，指向容器最后一个元素
     * @return 常量反向迭代器
     */
    const_reverse_iterator crbegin() const noexcept { 
        return rbegin(); 
    }

    /**
     * @brief 返回常量反向迭代器，指向容器第一个元素之前的位置
     * @return 常量反向迭代器
     */
    const_reverse_iterator crend() const noexcept { 
        return rend(); 
    }

    // 容量相关操作

    /**
     * @brief 判断容器是否为空
     * @return 容器为空返回true，否则返回false
     */
    bool empty() const noexcept { 
        return begin_ == end_; 
    }

    /**
     * @brief 获取容器中元素的数量
     * @return 元素数量
     */
    size_type size() const noexcept { 
        return static_cast<size_type>(end_ - begin_); 
    }

    /**
     * @brief 获取容器可以容纳的最大元素数量
     * @return 最大元素数量
     */
    size_type max_size() const noexcept { 
        return static_cast<size_type>(-1) / sizeof(T); 
    }

    /**
     * @brief 获取容器当前分配的存储空间的容量
     * @return 当前容量
     */
    size_type capacity() const noexcept { 
        return static_cast<size_type>(cap_ - begin_); 
    }

    /**
     * @brief 预留存储空间
     * @param n 预留的元素数量
     */
    void reserve(size_type n);

    /**
     * @brief 收缩容器存储空间以适应实际元素数量
     */
    void shrink_to_fit();

    // 元素访问操作

    /**
     * @brief 访问指定位置的元素
     * 
     * @param n 元素位置
     * @return 元素的引用
     */
    reference operator[](size_type n) {
        return *(begin_ + n);
    }

    /**
     * @brief 访问指定位置的元素（常量版本）
     * 
     * @param n 元素位置
     * @return 元素的常量引用
     */
    const_reference operator[](size_type n) const {
        return *(begin_ + n);
    }

    /**
     * @brief 访问指定位置的元素，带边界检查
     * 
     * @param n 元素位置
     * @return 元素的引用
     * @throw std::out_of_range 如果n >= size()
     */
    reference at(size_type n) {
        if (n >= size()) {
            throw std::out_of_range("vector<T>::at() 下标越界");
        }
        return (*this)[n];
    }

    /**
     * @brief 访问指定位置的元素，带边界检查（常量版本）
     * 
     * @param n 元素位置
     * @return 元素的常量引用
     * @throw std::out_of_range 如果n >= size()
     */
    const_reference at(size_type n) const {
        if (n >= size()) {
            throw std::out_of_range("vector<T>::at() 下标越界");
        }
        return (*this)[n];
    }

    /**
     * @brief 访问容器中第一个元素
     * 
     * @return 第一个元素的引用
     */
    reference front() {
        return *begin_;
    }

    /**
     * @brief 访问容器中第一个元素（常量版本）
     * 
     * @return 第一个元素的常量引用
     */
    const_reference front() const {
        return *begin_;
    }

    /**
     * @brief 访问容器中最后一个元素
     * 
     * @return 最后一个元素的引用
     */
    reference back() {
        return *(end_ - 1);
    }

    /**
     * @brief 访问容器中最后一个元素（常量版本）
     * 
     * @return 最后一个元素的常量引用
     */
    const_reference back() const {
        return *(end_ - 1);
    }

    /**
     * @brief 获取指向内存中存储数组的指针
     * 
     * @return 指向数组的指针
     */
    pointer data() noexcept {
        return begin_;
    }

    /**
     * @brief 获取指向内存中存储数组的常量指针
     * 
     * @return 指向数组的常量指针
     */
    const_pointer data() const noexcept {
        return begin_;
    }

    // 修改容器相关操作

    /**
     * @brief 用指定元素填充容器
     * 
     * @param n 元素个数
     * @param value 填充值
     */
    void assign(size_type n, const value_type& value) {
        fill_assign(n, value);
    }

    /**
     * @brief 用迭代器范围内的元素替换容器内容
     * 
     * @tparam Iter 迭代器类型
     * @param first 起始迭代器
     * @param last 终止迭代器
     */
    template <class Iter, typename std::enable_if<
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, 
        std::input_iterator_tag>::value, int>::type = 0>
    void assign(Iter first, Iter last) {
        copy_assign(first, last, typename std::iterator_traits<Iter>::iterator_category());
    }

    /**
     * @brief 用初始化列表替换容器内容
     * 
     * @param ilist 初始化列表
     */
    void assign(std::initializer_list<value_type> ilist) {
        copy_assign(ilist.begin(), ilist.end(), std::forward_iterator_tag());
    }

    /**
     * @brief 在指定位置构造元素
     * 
     * @tparam Args 构造参数类型
     * @param pos 指定位置
     * @param args 构造参数
     * @return 指向新元素的迭代器
     */
    template <class... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    /**
     * @brief 在容器尾部构造元素
     * 
     * @tparam Args 构造参数类型
     * @param args 构造参数
     */
    template <class... Args>
    void emplace_back(Args&&... args);

    /**
     * @brief 在容器尾部添加元素
     * 
     * @param value 要添加的元素值
     */
    void push_back(const value_type& value);

    /**
     * @brief 在容器尾部添加元素（移动版本）
     * 
     * @param value 要添加的元素值
     */
    void push_back(value_type&& value) {
        emplace_back(std::move(value));
    }

    /**
     * @brief 移除容器尾部元素
     */
    void pop_back();

    /**
     * @brief 在指定位置插入元素
     * 
     * @param pos 指定位置
     * @param value 要插入的元素值
     * @return 指向新元素的迭代器
     */
    iterator insert(const_iterator pos, const value_type& value);

    /**
     * @brief 在指定位置插入元素（移动版本）
     * 
     * @param pos 指定位置
     * @param value 要插入的元素值
     * @return 指向新元素的迭代器
     */
    iterator insert(const_iterator pos, value_type&& value) {
        return emplace(pos, std::move(value));
    }

    /**
     * @brief 在指定位置插入多个相同的元素
     * 
     * @param pos 指定位置
     * @param n 插入元素的数量
     * @param value 要插入的元素值
     * @return 指向第一个新元素的迭代器
     */
    iterator insert(const_iterator pos, size_type n, const value_type& value);

    /**
     * @brief 在指定位置插入一个范围内的元素
     * 
     * @tparam Iter 迭代器类型
     * @param pos 指定位置
     * @param first 起始迭代器
     * @param last 终止迭代器
     */
    template <class Iter, typename std::enable_if<
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, 
        std::input_iterator_tag>::value, int>::type = 0>
    void insert(const_iterator pos, Iter first, Iter last);

    /**
     * @brief 在指定位置插入初始化列表中的元素
     * 
     * @param pos 指定位置
     * @param ilist 初始化列表
     * @return 指向第一个新元素的迭代器
     */
    iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

    /**
     * @brief 移除指定位置的元素
     * 
     * @param pos 指定位置
     * @return 指向被移除元素的下一个元素的迭代器
     */
    iterator erase(const_iterator pos);

    /**
     * @brief 移除指定范围的元素
     * 
     * @param first 起始迭代器
     * @param last 终止迭代器
     * @return 指向最后一个被移除元素的下一个元素的迭代器
     */
    iterator erase(const_iterator first, const_iterator last);

    /**
     * @brief 清空容器
     */
    void clear() {
        erase(begin(), end());
    }

    /**
     * @brief 调整容器大小
     * 
     * @param new_size 新的大小
     */
    void resize(size_type new_size) {
        resize(new_size, value_type());
    }

    /**
     * @brief 调整容器大小，并指定填充值
     * 
     * @param new_size 新的大小
     * @param value 填充值
     */
    void resize(size_type new_size, const value_type& value);

    /**
     * @brief 反转容器中的元素顺序
     */
    void reverse() {
        std::reverse(begin(), end());
    }

    /**
     * @brief 与另一个vector交换内容
     * 
     * @param rhs 要交换的vector
     */
    void swap(vector& rhs) noexcept;

private:
    // 辅助函数

    /**
     * @brief 尝试初始化vector
     */
    void try_init() noexcept;

    /**
     * @brief 初始化指定大小的空间
     * 
     * @param size 元素数量
     * @param cap 容量
     */
    void init_space(size_type size, size_type cap);

    /**
     * @brief 用指定值填充初始化
     * 
     * @param n 元素数量
     * @param value 填充值
     */
    void fill_init(size_type n, const value_type& value);

    /**
     * @brief 使用迭代器范围初始化
     * 
     * @tparam Iter 迭代器类型
     * @param first 起始迭代器
     * @param last 终止迭代器
     */
    template <class Iter>
    void range_init(Iter first, Iter last);

    /**
     * @brief 销毁元素并回收内存
     * 
     * @param first 起始位置
     * @param last 终止位置
     * @param n 元素数量
     */
    void destroy_and_recover(iterator first, iterator last, size_type n);

    /**
     * @brief 计算新的容量
     * 
     * @param add_size 要添加的元素数量
     * @return 新的容量
     */
    size_type get_new_cap(size_type add_size);

    /**
     * @brief 填充赋值
     * 
     * @param n 元素数量
     * @param value 填充值
     */
    void fill_assign(size_type n, const value_type& value);

    /**
     * @brief 使用迭代器范围赋值（输入迭代器版本）
     * 
     * @tparam IIter 输入迭代器类型
     * @param first 起始迭代器
     * @param last 终止迭代器
     */
    template <class IIter>
    void copy_assign(IIter first, IIter last, std::input_iterator_tag);

    /**
     * @brief 使用迭代器范围赋值（前向迭代器版本）
     * 
     * @tparam FIter 前向迭代器类型
     * @param first 起始迭代器
     * @param last 终止迭代器
     */
    template <class FIter>
    void copy_assign(FIter first, FIter last, std::forward_iterator_tag);

    /**
     * @brief 重新分配空间并在指定位置就地构造元素
     * 
     * @tparam Args 构造参数类型
     * @param pos 指定位置
     * @param args 构造参数
     */
    template <class... Args>
    void reallocate_emplace(iterator pos, Args&&... args);

    /**
     * @brief 重新分配空间并在指定位置插入元素
     * 
     * @param pos 指定位置
     * @param value 要插入的元素值
     */
    void reallocate_insert(iterator pos, const value_type& value);

    /**
     * @brief 在指定位置填充插入元素
     * 
     * @param pos 指定位置
     * @param n 插入元素的数量
     * @param value 要插入的元素值
     * @return 指向第一个新元素的迭代器
     */
    iterator fill_insert(iterator pos, size_type n, const value_type& value);

    /**
     * @brief 在指定位置复制插入元素
     * 
     * @tparam IIter 输入迭代器类型
     * @param pos 指定位置
     * @param first 起始迭代器
     * @param last 终止迭代器
     */
    template <class IIter>
    void copy_insert(iterator pos, IIter first, IIter last);

    /**
     * @brief 重新插入元素（用于shrink_to_fit）
     * 
     * @param size 元素数量
     */
    void reinsert(size_type size);
};

// 重载比较操作符
template <class T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs);

template <class T>
bool operator!=(const vector<T>& lhs, const vector<T>& rhs);

template <class T>
bool operator<(const vector<T>& lhs, const vector<T>& rhs);

template <class T>
bool operator>(const vector<T>& lhs, const vector<T>& rhs);

template <class T>
bool operator<=(const vector<T>& lhs, const vector<T>& rhs);

template <class T>
bool operator>=(const vector<T>& lhs, const vector<T>& rhs);

// 重载swap
template <class T>
void swap(vector<T>& lhs, vector<T>& rhs);

/******************************************************************************************
 * vector 成员函数实现
 ******************************************************************************************/

// 拷贝赋值运算符
template <class T>
vector<T>& vector<T>::operator=(const vector& rhs) {
    if (this != &rhs) {
        const auto len = rhs.size();
        if (len > capacity()) {
            // 如果容量不足，创建新的vector并交换
            vector tmp(rhs.begin(), rhs.end());
            swap(tmp);
        } else if (size() >= len) {
            // 如果当前元素数量够用，只需拷贝并析构多余元素
            auto i = std::copy(rhs.begin(), rhs.end(), begin());
            // 析构多余的元素
            for (auto p = i; p != end_; ++p) {
                p->~T();
            }
            end_ = begin_ + len;
        } else {
            // 先拷贝已有空间内的元素
            std::copy(rhs.begin(), rhs.begin() + size(), begin_);
            // 再构造剩余元素
            std::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
            end_ = begin_ + len;
        }
    }
    return *this;
}

// 移动赋值运算符
template <class T>
vector<T>& vector<T>::operator=(vector&& rhs) noexcept {
    // 销毁当前元素并释放内存
    destroy_and_recover(begin_, end_, cap_ - begin_);
    // 窃取rhs资源
    begin_ = rhs.begin_;
    end_ = rhs.end_;
    cap_ = rhs.cap_;
    // 将rhs置于可析构状态
    rhs.begin_ = nullptr;
    rhs.end_ = nullptr;
    rhs.cap_ = nullptr;
    return *this;
}

// 预留存储空间
template <class T>
void vector<T>::reserve(size_type n) {
    // 只有当要求的容量大于当前容量时才重新分配
    if (capacity() < n) {
        // 检查是否超出最大容量
        if (n > max_size()) {
            throw std::length_error("vector::reserve - n超出了最大容量");
        }
        // 保存旧大小，用于之后的元素拷贝
        const auto old_size = size();
        // 分配新内存
        auto tmp = data_allocator().allocate(n);
        
        // 移动现有元素到新内存
        try {
            mystl::uninitialized_move(begin_, end_, tmp);
        } catch (...) {
            // 分配失败时释放新分配的内存
            data_allocator().deallocate(tmp, n);
            throw;
        }
        
        // 销毁旧元素并释放旧内存
        destroy_and_recover(begin_, end_, cap_ - begin_);
        
        // 更新指针
        begin_ = tmp;
        end_ = tmp + old_size;
        cap_ = begin_ + n;
    }
}

// 收缩容器存储空间
template <class T>
void vector<T>::shrink_to_fit() {
    // 只有当有多余空间时才进行收缩
    if (end_ < cap_) {
        reinsert(size());
    }
}

// helper functions

// try_init函数：尝试初始化一个默认大小的vector
template <class T>
void vector<T>::try_init() noexcept {
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

// init_space函数：初始化指定大小的空间
template <class T>
void vector<T>::init_space(size_type size, size_type cap) {
    try {
        begin_ = data_allocator().allocate(cap);
        end_ = begin_ + size;
        cap_ = begin_ + cap;
    } catch (...) {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
        throw;
    }
}

// fill_init函数：用指定值填充初始化
template <class T>
void vector<T>::fill_init(size_type n, const value_type& value) {
    // 选择初始容量，至少为16或n的较大者
    const size_type init_size = std::max(static_cast<size_type>(16), n);
    init_space(n, init_size);
    // 使用value填充n个元素
    std::uninitialized_fill_n(begin_, n, value);
}

// range_init函数：使用迭代器范围初始化
template <class T>
template <class Iter>
void vector<T>::range_init(Iter first, Iter last) {
    // 计算元素数量
    const size_type len = std::distance(first, last);
    // 选择初始容量，至少为16或len的较大者
    const size_type init_size = std::max(len, static_cast<size_type>(16));
    init_space(len, init_size);
    // 复制元素
    std::uninitialized_copy(first, last, begin_);
}

// destroy_and_recover函数：销毁元素并回收内存
template <class T>
void vector<T>::destroy_and_recover(iterator first, iterator last, size_type n) {
    // 析构元素
    for (auto p = first; p != last; ++p) {
        p->~T();
    }
    // 释放内存
    if (first) {
        data_allocator().deallocate(first, n);
    }
}

// get_new_cap函数：计算新的容量
template <class T>
typename vector<T>::size_type vector<T>::get_new_cap(size_type add_size) {
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

// fill_assign函数：填充赋值
template <class T>
void vector<T>::fill_assign(size_type n, const value_type& value) {
    if (n > capacity()) {
        // 如果需要更大的容量，创建一个新的vector并交换
        vector tmp(n, value);
        swap(tmp);
    } else if (n > size()) {
        // 如果需要更多元素，先填充已有元素
        std::fill(begin(), end(), value);
        // 再在末尾添加新元素
        end_ = std::uninitialized_fill_n(end_, n - size(), value);
    } else {
        // 如果需要更少元素，填充n个元素后删除多余的
        erase(std::fill_n(begin_, n, value), end_);
    }
}

// copy_assign函数（输入迭代器版本）
template <class T>
template <class IIter>
void vector<T>::copy_assign(IIter first, IIter last, std::input_iterator_tag) {
    auto cur = begin_;
    // 先复制到现有空间
    for (; first != last && cur != end_; ++first, ++cur) {
        *cur = *first;
    }
    
    if (first == last) {
        // 如果输入迭代器耗尽，删除多余元素
        erase(cur, end_);
    } else {
        // 如果还有更多输入元素，插入到末尾
        insert(end_, first, last);
    }
}

// copy_assign函数（前向迭代器版本）
template <class T>
template <class FIter>
void vector<T>::copy_assign(FIter first, FIter last, std::forward_iterator_tag) {
    const size_type len = std::distance(first, last);
    
    if (len > capacity()) {
        // 如果需要更大的容量，创建一个新的vector并交换
        vector tmp(first, last);
        swap(tmp);
    } else if (size() >= len) {
        // 如果当前大小足够，直接复制并销毁多余元素
        auto new_end = std::copy(first, last, begin_);
        for (auto p = new_end; p != end_; ++p) {
            p->~T();
        }
        end_ = new_end;
    } else {
        // 先复制当前大小的元素
        auto mid = first;
        std::advance(mid, size());
        std::copy(first, mid, begin_);
        // 再构造剩余元素
        end_ = std::uninitialized_copy(mid, last, end_);
    }
}

// reallocate_emplace函数：重新分配空间并在指定位置就地构造元素
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

// reallocate_insert函数：重新分配空间并在指定位置插入元素
template <class T>
void vector<T>::reallocate_insert(iterator pos, const value_type& value) {
    // 获取新的容量
    const auto new_size = get_new_cap(1);
    // 分配新内存
    auto new_begin = data_allocator().allocate(new_size);
    auto new_end = new_begin;
    
    // 创建value的拷贝，避免原值因为移动操作而被修改
    const value_type value_copy = value;
    
    try {
        // 将pos之前的元素移动到新内存
        new_end = mystl::uninitialized_move(begin_, pos, new_begin);
        // 在pos位置构造新元素
        data_allocator().construct(new_end, value_copy);
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

// emplace函数：在指定位置就地构造元素
template <class T>
template <class... Args>
typename vector<T>::iterator vector<T>::emplace(const_iterator pos, Args&&... args) {
    // 计算pos位置相对于begin_的偏移量
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = xpos - begin_;
    
    if (end_ != cap_ && xpos == end_) {
        // 如果是在尾部插入且有足够空间，直接构造
        data_allocator().construct(end_, std::forward<Args>(args)...);
        ++end_;
    } else if (end_ != cap_) {
        // 如果有足够空间但不是在尾部插入
        // 先保存end_的值
        auto old_end = end_;
        // 将最后一个元素复制到未初始化内存
        data_allocator().construct(end_, *(end_ - 1));
        ++end_;
        // 将[xpos, old_end-1)范围内的元素向后移动一个位置
        std::move_backward(xpos, old_end - 1, old_end);
        // 在空出的位置构造新元素
        *xpos = value_type(std::forward<Args>(args)...);
    } else {
        // 空间不足，需要重新分配
        reallocate_emplace(xpos, std::forward<Args>(args)...);
    }
    
    return begin() + n;
}

// emplace_back函数：在容器尾部就地构造元素
template <class T>
template <class... Args>
void vector<T>::emplace_back(Args&&... args) {
    if (end_ != cap_) {
        // 如果有足够空间，直接在尾部构造
        data_allocator().construct(end_, std::forward<Args>(args)...);
        ++end_;
    } else {
        // 空间不足，需要重新分配
        reallocate_emplace(end_, std::forward<Args>(args)...);
    }
}

// push_back函数：在容器尾部添加元素
template <class T>
void vector<T>::push_back(const value_type& value) {
    if (end_ != cap_) {
        // 如果有足够空间，直接在尾部构造
        data_allocator().construct(end_, value);
        ++end_;
    } else {
        // 空间不足，需要重新分配
        reallocate_insert(end_, value);
    }
}

// pop_back函数：移除容器尾部元素
template <class T>
void vector<T>::pop_back() {
    if (empty()) {
        return;
    }
    // 析构最后一个元素
    (--end_)->~T();
}

// insert函数：在指定位置插入元素
template <class T>
typename vector<T>::iterator vector<T>::insert(const_iterator pos, const value_type& value) {
    // 计算pos位置相对于begin_的偏移量
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = pos - begin();
    
    if (end_ != cap_ && xpos == end_) {
        // 如果是在尾部插入且有足够空间，直接构造
        data_allocator().construct(end_, value);
        ++end_;
    } else if (end_ != cap_) {
        // 如果有足够空间但不是在尾部插入
        // 先保存end_的值
        auto new_end = end_;
        // 将最后一个元素复制到未初始化内存
        data_allocator().construct(end_, *(end_ - 1));
        ++new_end;
        // 创建value的副本，避免因为移动操作修改原值
        auto value_copy = value;
        // 将[xpos, end_-1)范围内的元素向后移动一个位置
        std::move_backward(xpos, end_ - 1, end_);
        // 在空出的位置赋值
        *xpos = std::move(value_copy);
        end_ = new_end;
    } else {
        // 空间不足，需要重新分配
        reallocate_insert(xpos, value);
    }
    
    return begin_ + n;
}

// insert函数：在指定位置插入多个相同的元素
template <class T>
typename vector<T>::iterator vector<T>::insert(const_iterator pos, size_type n, const value_type& value) {
    // 如果插入0个元素，直接返回
    if (n == 0) {
        return const_cast<iterator>(pos);
    }
    
    iterator xpos = const_cast<iterator>(pos);
    const size_type pos_n = xpos - begin_;
    
    // 保存value的副本，避免因为移动操作修改原值
    const value_type value_copy = value;
    
    if (static_cast<size_type>(cap_ - end_) >= n) {
        // 如果有足够空间
        const size_type after_elems = end_ - xpos;
        auto old_end = end_;
        
        if (after_elems > n) {
            // 如果待插入位置后的元素数量大于n
            // 将末尾n个元素移动到未初始化空间
            std::uninitialized_copy(end_ - n, end_, end_);
            end_ += n;
            // 将[xpos, old_end-n)范围内的元素向后移动n个位置
            std::move_backward(xpos, old_end - n, old_end);
            // 在[xpos, xpos+n)范围内填充value_copy
            std::fill(xpos, xpos + n, value_copy);
        } else {
            // 如果待插入位置后的元素数量不大于n
            // 在末尾填充value_copy，数量为n-after_elems
            end_ = std::uninitialized_fill_n(end_, n - after_elems, value_copy);
            // 将[xpos, old_end)范围内的元素移动到新位置
            end_ = mystl::uninitialized_move(xpos, old_end, end_);
            // 在[xpos, xpos+after_elems)范围内填充value_copy
            std::fill(xpos, xpos + after_elems, value_copy);
        }
    } else {
        // 空间不足，需要重新分配
        const auto new_size = get_new_cap(n);
        auto new_begin = data_allocator().allocate(new_size);
        auto new_end = new_begin;
        
        try {
            // 将xpos之前的元素移动到新内存
            new_end = mystl::uninitialized_move(begin_, xpos, new_begin);
            // 在新位置填充n个value_copy
            new_end = std::uninitialized_fill_n(new_end, n, value_copy);
            // 将xpos之后的元素移动到新内存
            new_end = mystl::uninitialized_move(xpos, end_, new_end);
        } catch (...) {
            // 如果发生异常，销毁已构造的新元素并释放新内存
            destroy_and_recover(new_begin, new_end, new_size);
            throw;
        }
        
        // 销毁旧元素并释放旧内存
        destroy_and_recover(begin_, end_, cap_ - begin_);
        
        // 更新指针
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }
    
    return begin_ + pos_n;
}

// 在指定位置插入一个范围内的元素
template <class T>
template <class Iter, typename std::enable_if<
    std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, 
    std::input_iterator_tag>::value, int>::type>
void vector<T>::insert(const_iterator pos, Iter first, Iter last) {
    // 如果要插入的范围为空，直接返回
    if (first == last) {
        return;
    }
    
    iterator xpos = const_cast<iterator>(pos);
    
    // 计算要插入的元素个数
    const size_type n = std::distance(first, last);
    
    if (static_cast<size_type>(cap_ - end_) >= n) {
        // 如果有足够空间
        const size_type after_elems = end_ - xpos;
        auto old_end = end_;
        
        if (after_elems > n) {
            // 如果待插入位置后的元素数量大于n
            // 将末尾n个元素移动到未初始化空间
            std::uninitialized_copy(end_ - n, end_, end_);
            end_ += n;
            // 将[xpos, old_end-n)范围内的元素向后移动n个位置
            std::move_backward(xpos, old_end - n, old_end);
            // 复制[first, last)范围内的元素到xpos位置
            std::copy(first, last, xpos);
        } else {
            // 如果待插入位置后的元素数量不大于n
            // 找到从first开始，需要放到uninitialized区域的迭代器
            auto mid = first;
            std::advance(mid, after_elems);
            // 将[mid, last)范围内的元素复制到end_
            end_ = std::uninitialized_copy(mid, last, end_);
            // 将[xpos, old_end)范围内的元素移动到新位置
            end_ = mystl::uninitialized_move(xpos, old_end, end_);
            // 复制[first, mid)范围内的元素到xpos位置
            std::copy(first, mid, xpos);
        }
    } else {
        // 空间不足，需要重新分配
        const auto new_size = get_new_cap(n);
        auto new_begin = data_allocator().allocate(new_size);
        auto new_end = new_begin;
        
        try {
            // 将xpos之前的元素移动到新内存
            new_end = mystl::uninitialized_move(begin_, xpos, new_begin);
            // 复制[first, last)范围内的元素到新位置
            new_end = std::uninitialized_copy(first, last, new_end);
            // 将xpos之后的元素移动到新内存
            new_end = mystl::uninitialized_move(xpos, end_, new_end);
        } catch (...) {
            // 如果发生异常，销毁已构造的新元素并释放新内存
            destroy_and_recover(new_begin, new_end, new_size);
            throw;
        }
        
        // 销毁旧元素并释放旧内存
        destroy_and_recover(begin_, end_, cap_ - begin_);
        
        // 更新指针
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }
}

// erase函数：移除指定位置的元素
template <class T>
typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
    if (pos == end()) {
        return const_cast<iterator>(pos);
    }
    
    iterator xpos = const_cast<iterator>(pos);
    // 将pos后面的元素向前移动一个位置
    std::move(xpos + 1, end_, xpos);
    // 析构最后一个元素
    (--end_)->~T();
    
    return xpos;
}

// erase函数：移除指定范围的元素
template <class T>
typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) {
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

// resize函数：调整容器大小
template <class T>
void vector<T>::resize(size_type new_size, const value_type& value) {
    if (new_size < size()) {
        // 如果新大小小于当前大小，删除多余元素
        erase(begin() + new_size, end());
    } else {
        // 否则在尾部插入元素
        insert(end(), new_size - size(), value);
    }
}

// swap函数：与另一个vector交换内容
template <class T>
void vector<T>::swap(vector& rhs) noexcept {
    if (this != &rhs) {
        std::swap(begin_, rhs.begin_);
        std::swap(end_, rhs.end_);
        std::swap(cap_, rhs.cap_);
    }
}

// reinsert函数：重新插入元素（用于shrink_to_fit）
template <class T>
void vector<T>::reinsert(size_type size) {
    // 分配新内存
    auto new_begin = data_allocator().allocate(size);
    
    try {
        // 移动元素到新内存
        mystl::uninitialized_move(begin_, end_, new_begin);
    } catch (...) {
        // 如果发生异常，释放新内存
        data_allocator().deallocate(new_begin, size);
        throw;
    }
    
    // 销毁旧元素并释放旧内存
    destroy_and_recover(begin_, end_, cap_ - begin_);
    
    // 更新指针
    begin_ = new_begin;
    end_ = begin_ + size;
    cap_ = begin_ + size;
}

// 重载比较操作符
template <class T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T>
bool operator!=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs == rhs);
}

template <class T>
bool operator<(const vector<T>& lhs, const vector<T>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T>
bool operator>(const vector<T>& lhs, const vector<T>& rhs) {
    return rhs < lhs;
}

template <class T>
bool operator<=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(rhs < lhs);
}

template <class T>
bool operator>=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs < rhs);
}

// 重载swap
template <class T>
void swap(vector<T>& lhs, vector<T>& rhs) {
    lhs.swap(rhs);
}

} // namespace mystl

#endif // MY_VECTOR_H 