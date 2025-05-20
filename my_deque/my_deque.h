#ifndef MY_DEQUE_H
#define MY_DEQUE_H

// 这个头文件包含了一个模板类 deque
// deque: 双端队列

/**
 * @file my_deque.h
 * @brief 实现了双端队列容器
 * 
 * @details 双端队列(deque)是一种具有动态大小的序列容器，可以在两端(前端或后端)快速地进行扩展和收缩。
 * 不同于vector，deque在内存中不是连续存储的，它通常由多个连续的内存块(缓冲区)构成，并通过中央控制部分将这些块链接起来。
 * 主要特点: 
 * 1. 支持随机访问
 * 2. 在两端插入或删除元素的时间复杂度为常数O(1)
 * 3. 在内部插入或删除元素的时间复杂度为线性O(n)
 * 4. 不保证内存连续性
 */

#include <initializer_list>
#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <type_traits>
#include <memory>
#include <iterator>

// 预定义deque的map初始大小
#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

namespace mystl {

// 前置声明
template <class T, class Ref, class Ptr> class deque_iterator;
template <class T> class deque;

// 声明外部运算符函数
template <class T>
bool operator==(const deque<T>& lhs, const deque<T>& rhs);

template <class T>
bool operator<(const deque<T>& lhs, const deque<T>& rhs);

template <class T>
bool operator!=(const deque<T>& lhs, const deque<T>& rhs);

template <class T>
bool operator>(const deque<T>& lhs, const deque<T>& rhs);

template <class T>
bool operator<=(const deque<T>& lhs, const deque<T>& rhs);

template <class T>
bool operator>=(const deque<T>& lhs, const deque<T>& rhs);

template <class T>
void swap(deque<T>& lhs, deque<T>& rhs) noexcept;

/**
 * @brief 计算deque缓冲区大小的结构体
 * 
 * @tparam T 元素类型
 */
template <class T>
struct deque_buf_size {
    // 如果T类型大小小于256字节，则每个缓冲区存储4096/sizeof(T)个元素
    // 否则每个缓冲区存储16个元素
    static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
};

/**
 * @brief deque的迭代器
 * 
 * @tparam T 元素类型
 * @tparam Ref 引用类型
 * @tparam Ptr 指针类型
 */
template <class T, class Ref, class Ptr>
class deque_iterator {
public:
    // 迭代器相关类型定义
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_pointer = T*;
    using map_pointer = T**;
    using self = deque_iterator;

    // 同一个deque容器的两种迭代器类型
    using iterator = deque_iterator<T, T&, T*>;
    using const_iterator = deque_iterator<T, const T&, const T*>;

    // 缓冲区大小
    static constexpr size_type buffer_size = deque_buf_size<T>::value;

    // 迭代器成员变量
    value_pointer cur;    // 指向当前缓冲区中的当前元素
    value_pointer first;  // 指向当前缓冲区的头部
    value_pointer last;   // 指向当前缓冲区的尾部
    map_pointer   node;   // 指向map中控制当前缓冲区的节点

    // 构造、复制、移动函数
    deque_iterator() noexcept
        : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

    deque_iterator(value_pointer v, map_pointer n)
        : cur(v), first(*n), last(*n + buffer_size), node(n) {}

    // 拷贝构造函数
    deque_iterator(const iterator& rhs)
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

    // 从const_iterator构造
    deque_iterator(const const_iterator& rhs)
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

    // 移动构造函数
    deque_iterator(iterator&& rhs) noexcept
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {
        rhs.cur = nullptr;
        rhs.first = nullptr;
        rhs.last = nullptr;
        rhs.node = nullptr;
    }

    // 赋值运算符
    self& operator=(const iterator& rhs) {
        if (this != &rhs) {
            cur = rhs.cur;
            first = rhs.first;
            last = rhs.last;
            node = rhs.node;
        }
        return *this;
    }

    // 切换到另一个缓冲区
    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + buffer_size;
    }

    // 重载运算符
    reference operator*() const { return *cur; }
    pointer operator->() const { return cur; }

    // 计算两个迭代器之间的距离
    difference_type operator-(const self& x) const {
        return static_cast<difference_type>(buffer_size) * (node - x.node) + 
               (cur - first) - (x.cur - x.first);
    }

    // 前置递增
    self& operator++() {
        ++cur;
        if (cur == last) { // 如果到达当前缓冲区末尾
            set_node(node + 1); // 切换到下一个缓冲区
            cur = first;        // 定位到下一个缓冲区的起始位置
        }
        return *this;
    }

    // 后置递增
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    // 前置递减
    self& operator--() {
        if (cur == first) { // 如果在当前缓冲区起始位置
            set_node(node - 1); // 切换到上一个缓冲区
            cur = last;         // 定位到上一个缓冲区的末尾
        }
        --cur;
        return *this;
    }

    // 后置递减
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    // 迭代器加上一个差值
    self& operator+=(difference_type n) {
        // 计算cur与first之间的偏移量加上n
        difference_type offset = n + (cur - first);
        
        // 如果偏移量在当前缓冲区内
        if (offset >= 0 && offset < static_cast<difference_type>(buffer_size)) {
            cur += n;
        } else { // 需要跨缓冲区
            // 计算需要移动的缓冲区数量
            difference_type node_offset;
            if (offset > 0) {
                node_offset = offset / static_cast<difference_type>(buffer_size);
            } else {
                // 对于负偏移，需要特殊处理向上取整的情况
                node_offset = -static_cast<difference_type>((-offset - 1) / buffer_size) - 1;
            }
            
            // 切换到新的缓冲区
            set_node(node + node_offset);
            
            // 设置在新缓冲区内的正确位置
            cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size));
        }
        return *this;
    }

    // 迭代器加上一个差值（非成员函数版本）
    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }

    // 迭代器减去一个差值
    self& operator-=(difference_type n) {
        return *this += -n;
    }

    // 迭代器减去一个差值（非成员函数版本）
    self operator-(difference_type n) const {
        self tmp = *this;
        return tmp -= n;
    }

    // 下标运算符
    reference operator[](difference_type n) const {
        return *(*this + n);
    }

    // 比较运算符
    bool operator==(const self& rhs) const { return cur == rhs.cur; }
    bool operator!=(const self& rhs) const { return !(*this == rhs); }
    bool operator<(const self& rhs) const {
        return (node == rhs.node) ? (cur < rhs.cur) : (node < rhs.node);
    }
    bool operator>(const self& rhs) const { return rhs < *this; }
    bool operator<=(const self& rhs) const { return !(rhs < *this); }
    bool operator>=(const self& rhs) const { return !(*this < rhs); }
};

/**
 * @brief deque 容器类模板
 * 
 * @tparam T 元素类型
 */
template <class T>
class deque {
public:
    // deque的型别定义
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using allocator_type = std::allocator<T>;
    
    using iterator = deque_iterator<T, T&, T*>;
    using const_iterator = deque_iterator<T, const T&, const T*>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using map_pointer = pointer*;
    using const_map_pointer = const pointer*;
    
    // deque缓冲区大小
    static constexpr size_type buffer_size = deque_buf_size<T>::value;
    
    // 内存分配器
    using data_allocator_type = std::allocator<T>;
    using map_allocator_type = std::allocator<T*>;

private:
    // deque的内部数据成员
    iterator begin_;      // 指向第一个元素的迭代器
    iterator end_;        // 指向最后一个元素的后一个位置的迭代器
    map_pointer map_;     // 指向map，map是一个T**指针数组
    size_type map_size_;  // map中的节点数
    
    // 内存分配器对象
    data_allocator_type data_allocator;
    map_allocator_type map_allocator;

private:
    // 辅助函数

    // 创建/销毁存储空间
    /**
     * @brief 创建map数组
     * @param size 数组大小
     * @return 指向新创建的map数组的指针
     */
    map_pointer create_map(size_type size);
    
    /**
     * @brief 创建缓冲区
     * @param nstart 开始节点
     * @param nfinish 结束节点
     */
    void create_buffer(map_pointer nstart, map_pointer nfinish);
    
    /**
     * @brief 销毁缓冲区
     * @param nstart 开始节点
     * @param nfinish 结束节点
     */
    void destroy_buffer(map_pointer nstart, map_pointer nfinish);

    // 初始化相关函数
    /**
     * @brief 初始化map
     * @param nelem 元素数量
     */
    void map_init(size_type nelem);
    
    /**
     * @brief 以指定值填充初始化
     * @param n 元素数量
     * @param value 填充值
     */
    void fill_init(size_type n, const value_type& value);
    
    /**
     * @brief 从输入迭代器复制初始化(输入迭代器版本)
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    template <class InputIter>
    void copy_init(InputIter first, InputIter last, std::input_iterator_tag);
    
    /**
     * @brief 从前向迭代器复制初始化(前向迭代器版本)
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    template <class ForwardIter>
    void copy_init(ForwardIter first, ForwardIter last, std::forward_iterator_tag);

    // 容量调整辅助函数
    /**
     * @brief 确保容器有足够的容量
     */
    void require_capacity(size_type n, bool front);
    
    /**
     * @brief 在头部重新分配map
     */
    void reallocate_map_at_front(size_type need_buffer);
    
    /**
     * @brief 在尾部重新分配map
     */
    void reallocate_map_at_back(size_type need_buffer);

    // 插入辅助函数
    /**
     * @brief 在指定位置插入元素的辅助函数
     * @param pos 插入位置
     * @param args 构造参数
     * @return 指向新元素的迭代器
     */
    template <class... Args>
    iterator insert_aux(iterator pos, Args&&... args);
    
    /**
     * @brief 在指定位置填充插入元素
     * @param pos 插入位置
     * @param n 元素数量
     * @param value 填充值
     */
    void fill_insert(iterator pos, size_type n, const value_type& value);

public:
    // 构造、析构函数
    
    /**
     * @brief 默认构造函数
     */
    deque() { 
        fill_init(0, value_type()); 
    }
    
    /**
     * @brief 构造指定大小的deque
     * @param n 元素数量
     */
    explicit deque(size_type n) { 
        fill_init(n, value_type()); 
    }
    
    /**
     * @brief 构造指定大小并填充相同值的deque
     * @param n 元素数量
     * @param value 填充值
     */
    deque(size_type n, const value_type& value) { 
        fill_init(n, value); 
    }
    
    /**
     * @brief 从输入迭代器构造deque
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    template <class InputIter, typename = typename 
        std::enable_if<std::is_convertible<
            typename std::iterator_traits<InputIter>::iterator_category, 
            std::input_iterator_tag>::value>::type>
    deque(InputIter first, InputIter last) {
        copy_init(first, last, typename std::iterator_traits<InputIter>::iterator_category());
    }
    
    /**
     * @brief 从初始化列表构造deque
     * @param ilist 初始化列表
     */
    deque(std::initializer_list<value_type> ilist) {
        copy_init(ilist.begin(), ilist.end(), std::forward_iterator_tag());
    }
    
    /**
     * @brief 拷贝构造函数
     * @param rhs 拷贝源
     */
    deque(const deque& rhs) {
        copy_init(rhs.begin(), rhs.end(), std::forward_iterator_tag());
    }
    
    /**
     * @brief 移动构造函数
     * @param rhs 移动源
     */
    deque(deque&& rhs) noexcept
        : begin_(std::move(rhs.begin_)),
          end_(std::move(rhs.end_)),
          map_(rhs.map_),
          map_size_(rhs.map_size_) {
        rhs.map_ = nullptr;
        rhs.map_size_ = 0;
    }
    
    /**
     * @brief 析构函数
     */
    ~deque() {
        if (map_ != nullptr) {
            clear();
            data_allocator.deallocate(*begin_.node, buffer_size);
            *begin_.node = nullptr;
            map_allocator.deallocate(map_, map_size_);
            map_ = nullptr;
        }
    }

    // 赋值操作
    
    /**
     * @brief 拷贝赋值运算符
     * @param rhs 拷贝源
     * @return 返回自身引用
     */
    deque& operator=(const deque& rhs) {
        if (this != &rhs) {
            const auto len = size();
            if (len >= rhs.size()) {
                // 如果当前容器大小大于等于rhs，只需要复制rhs的内容，并删除多余元素
                auto new_end = std::copy(rhs.begin(), rhs.end(), begin_);
                // 删除多余元素
                erase(new_end, end_);
            } else {
                // 如果当前容器小于rhs，分两段处理
                auto mid = rhs.begin() + static_cast<difference_type>(len);
                std::copy(rhs.begin(), mid, begin_);
                // 插入剩余元素
                for (auto it = mid; it != rhs.end(); ++it) {
                    push_back(*it);
                }
            }
        }
        return *this;
    }
    
    /**
     * @brief 移动赋值运算符
     * @param rhs 移动源
     * @return 返回自身引用
     */
    deque& operator=(deque&& rhs) noexcept {
        clear();
        begin_ = std::move(rhs.begin_);
        end_ = std::move(rhs.end_);
        map_ = rhs.map_;
        map_size_ = rhs.map_size_;
        rhs.map_ = nullptr;
        rhs.map_size_ = 0;
        return *this;
    }
    
    /**
     * @brief 从初始化列表赋值
     * @param ilist 初始化列表
     * @return 返回自身引用
     */
    deque& operator=(std::initializer_list<value_type> ilist) {
        deque tmp(ilist);
        swap(tmp);
        return *this;
    }

public:
    // 迭代器相关操作
    
    /**
     * @brief 返回指向容器第一个元素的迭代器
     * @return 迭代器
     */
    iterator begin() noexcept { return begin_; }
    
    /**
     * @brief 返回指向容器第一个元素的常量迭代器
     * @return 常量迭代器
     */
    const_iterator begin() const noexcept { return begin_; }
    
    /**
     * @brief 返回指向容器尾部的迭代器（指向最后一个元素之后的位置）
     * @return 迭代器
     */
    iterator end() noexcept { return end_; }
    
    /**
     * @brief 返回指向容器尾部的常量迭代器
     * @return 常量迭代器
     */
    const_iterator end() const noexcept { return end_; }
    
    /**
     * @brief 返回反向迭代器，指向容器最后一个元素
     * @return 反向迭代器
     */
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    
    /**
     * @brief 返回常量反向迭代器，指向容器最后一个元素
     * @return 常量反向迭代器
     */
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    
    /**
     * @brief 返回反向迭代器，指向容器第一个元素之前的位置
     * @return 反向迭代器
     */
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    
    /**
     * @brief 返回常量反向迭代器，指向容器第一个元素之前的位置
     * @return 常量反向迭代器
     */
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    
    /**
     * @brief 返回常量迭代器，指向容器第一个元素
     * @return 常量迭代器
     */
    const_iterator cbegin() const noexcept { return begin(); }
    
    /**
     * @brief 返回常量迭代器，指向容器尾部
     * @return 常量迭代器
     */
    const_iterator cend() const noexcept { return end(); }
    
    /**
     * @brief 返回常量反向迭代器，指向容器最后一个元素
     * @return 常量反向迭代器
     */
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    
    /**
     * @brief 返回常量反向迭代器，指向容器第一个元素之前的位置
     * @return 常量反向迭代器
     */
    const_reverse_iterator crend() const noexcept { return rend(); }

    // 容量相关操作
    
    /**
     * @brief 检查容器是否为空
     * @return 如果容器为空则返回true，否则返回false
     */
    bool empty() const noexcept { return begin() == end(); }
    
    /**
     * @brief 返回容器中的元素个数
     * @return 元素个数
     */
    size_type size() const noexcept { return end_ - begin_; }
    
    /**
     * @brief 返回容器能容纳的最大元素数量
     * @return 最大元素数量
     */
    size_type max_size() const noexcept { return static_cast<size_type>(-1); }
    
    /**
     * @brief 调整容器大小
     * @param new_size 新的大小
     */
    void resize(size_type new_size) { resize(new_size, value_type()); }
    
    /**
     * @brief 调整容器大小，并用指定值填充新增元素
     * @param new_size 新的大小
     * @param value 填充值
     */
    void resize(size_type new_size, const value_type& value);
    
    /**
     * @brief 减少容器占用的内存，不改变元素个数
     */
    void shrink_to_fit() noexcept;

    // 访问元素相关操作
    
    /**
     * @brief 访问指定位置的元素
     * @param n 位置索引
     * @return 指定位置元素的引用
     */
    reference operator[](size_type n) {
        return begin_[n];
    }
    
    /**
     * @brief 访问指定位置的元素（常量版本）
     * @param n 位置索引
     * @return 指定位置元素的常量引用
     */
    const_reference operator[](size_type n) const {
        return begin_[n];
    }
    
    /**
     * @brief 访问指定位置的元素，带边界检查
     * @param n 位置索引
     * @return 指定位置元素的引用
     * @throw std::out_of_range 如果n超出有效范围
     */
    reference at(size_type n) {
        if (n >= size())
            throw std::out_of_range("deque::at() - Index out of range");
        return (*this)[n];
    }
    
    /**
     * @brief 访问指定位置的元素，带边界检查（常量版本）
     * @param n 位置索引
     * @return 指定位置元素的常量引用
     * @throw std::out_of_range 如果n超出有效范围
     */
    const_reference at(size_type n) const {
        if (n >= size())
            throw std::out_of_range("deque::at() - Index out of range");
        return (*this)[n];
    }
    
    /**
     * @brief 访问容器第一个元素
     * @return 第一个元素的引用
     */
    reference front() {
        return *begin();
    }
    
    /**
     * @brief 访问容器第一个元素（常量版本）
     * @return 第一个元素的常量引用
     */
    const_reference front() const {
        return *begin();
    }
    
    /**
     * @brief 访问容器最后一个元素
     * @return 最后一个元素的引用
     */
    reference back() {
        return *(end() - 1);
    }
    
    /**
     * @brief 访问容器最后一个元素（常量版本）
     * @return 最后一个元素的常量引用
     */
    const_reference back() const {
        return *(end() - 1);
    }

    // 修改容器相关操作
    
    /**
     * @brief 在容器头部添加元素
     * @param value 要添加的元素值
     */
    void push_front(const value_type& value);
    
    /**
     * @brief 在容器头部添加元素(移动)
     * @param value 要添加的元素值
     */
    void push_front(value_type&& value) { 
        emplace_front(std::move(value)); 
    }
    
    /**
     * @brief 在容器尾部添加元素
     * @param value 要添加的元素值
     */
    void push_back(const value_type& value);
    
    /**
     * @brief 在容器尾部添加元素(移动)
     * @param value 要添加的元素值
     */
    void push_back(value_type&& value) { 
        emplace_back(std::move(value)); 
    }
    
    /**
     * @brief 在容器头部原地构造元素
     * @param args 构造参数
     */
    template <class... Args>
    void emplace_front(Args&&... args);
    
    /**
     * @brief 在容器尾部原地构造元素
     * @param args 构造参数
     */
    template <class... Args>
    void emplace_back(Args&&... args);
    
    /**
     * @brief 在容器指定位置原地构造元素
     * @param pos 插入位置
     * @param args 构造参数
     * @return 指向新元素的迭代器
     */
    template <class... Args>
    iterator emplace(iterator pos, Args&&... args);
    
    /**
     * @brief 在容器指定位置插入元素
     * @param pos 插入位置
     * @param value 要插入的元素值
     * @return 指向新元素的迭代器
     */
    iterator insert(iterator pos, const value_type& value);
    
    /**
     * @brief 在容器指定位置插入元素(移动)
     * @param pos 插入位置
     * @param value 要插入的元素值
     * @return 指向新元素的迭代器
     */
    iterator insert(iterator pos, value_type&& value);
    
    /**
     * @brief 在容器指定位置插入n个元素
     * @param pos 插入位置
     * @param n 元素数量
     * @param value 要插入的元素值
     */
    void insert(iterator pos, size_type n, const value_type& value);
    
    /**
     * @brief 从容器头部删除元素
     */
    void pop_front();
    
    /**
     * @brief 从容器尾部删除元素
     */
    void pop_back();
    
    /**
     * @brief 删除指定位置的元素
     * @param pos 要删除的元素位置
     * @return 指向被删除元素下一个位置的迭代器
     */
    iterator erase(iterator pos);
    
    /**
     * @brief 删除范围内的元素
     * @param first 范围起始位置
     * @param last 范围结束位置
     * @return 指向最后被删除元素下一个位置的迭代器
     */
    iterator erase(iterator first, iterator last);
    
    /**
     * @brief 清空容器
     */
    void clear();
    
    /**
     * @brief 交换两个容器的内容
     * @param rhs 要交换的容器
     */
    void swap(deque& rhs) noexcept;

    /**
     * @brief 从迭代器范围插入元素
     * @param pos 插入位置
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    template <class InputIter>
    void insert(iterator pos, InputIter first, InputIter last) {
        // 如果是插入到头部
        if (pos == begin_) {
            size_type n = std::distance(first, last);
            require_capacity(n, true);
            auto new_begin = begin_ - n;
            begin_ = new_begin; // 先更新begin_
            
            try {
                // 复制元素到前面的缓冲区
                iterator cur = begin_;
                for (; first != last; ++first, ++cur) {
                    data_allocator.construct(cur.cur, *first);
                }
            }
            catch (...) {
                // 异常恢复，这里仅简单还原begin_
                begin_ = begin_ + n;
                throw;
            }
        }
        // 如果是插入到尾部
        else if (pos == end_) {
            for (; first != last; ++first) {
                push_back(*first); // 直接调用push_back
            }
        }
        // 插入到中间位置
        else {
            // 简化实现，逐个插入
            // 实际可以优化为分块复制
            for (; first != last; ++first) {
                pos = insert(pos, *first) + 1;
            }
        }
    }
};

/**
 * @brief 调整容器大小，并用指定值填充新增元素
 */
template <class T>
void deque<T>::resize(size_type new_size, const value_type& value) {
    const auto len = size();
    if (new_size < len) {
        // 缩小容器，删除多余元素
        erase(begin_ + new_size, end_);
    } else {
        // 扩大容器，在尾部插入新元素
        insert(end_, new_size - len, value);
    }
}

/**
 * @brief 减少容器占用的内存，不改变元素个数
 */
template <class T>
void deque<T>::shrink_to_fit() noexcept {
    // 至少会留下头部缓冲区
    // 释放前面未使用的缓冲区
    for (auto cur = map_; cur < begin_.node; ++cur) {
        data_allocator.deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
    
    // 释放后面未使用的缓冲区
    for (auto cur = end_.node + 1; cur < map_ + map_size_; ++cur) {
        data_allocator.deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
}

/**
 * @brief 创建map数组
 */
template <class T>
typename deque<T>::map_pointer
deque<T>::create_map(size_type size) {
    map_pointer mp = nullptr;
    // 分配足够大小的内存
    mp = map_allocator.allocate(size);
    // 初始化所有指针为nullptr
    for (size_type i = 0; i < size; ++i) {
        *(mp + i) = nullptr;
    }
    return mp;
}

/**
 * @brief 创建缓冲区
 */
template <class T>
void deque<T>::create_buffer(map_pointer nstart, map_pointer nfinish) {
    map_pointer cur;
    try {
        // 为每个map节点分配一个缓冲区
        for (cur = nstart; cur <= nfinish; ++cur) {
            *cur = data_allocator.allocate(buffer_size);
        }
    }
    catch (...) {
        // 异常处理，释放已分配的缓冲区
        while (cur != nstart) {
            --cur;
            data_allocator.deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
        throw; // 重新抛出异常
    }
}

/**
 * @brief 销毁缓冲区
 */
template <class T>
void deque<T>::destroy_buffer(map_pointer nstart, map_pointer nfinish) {
    // 释放所有缓冲区
    for (map_pointer n = nstart; n <= nfinish; ++n) {
        data_allocator.deallocate(*n, buffer_size);
        *n = nullptr;
    }
}

/**
 * @brief 初始化map
 */
template <class T>
void deque<T>::map_init(size_type nElem) {
    // 计算需要的缓冲区数量
    // 至少需要一个缓冲区(nElem / buffer_size + 1)
    const size_type nNode = nElem / buffer_size + 1;
    
    // 分配map大小，预留一些空间以便于扩展
    map_size_ = std::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), nNode + 2);
    
    try {
        // 分配map内存
        map_ = create_map(map_size_);
    }
    catch (...) {
        // 异常处理
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }

    // 计算开始和结束节点的位置，使得缓冲区位于map中央
    // 这样可以方便向两端扩展
    map_pointer nstart = map_ + (map_size_ - nNode) / 2;
    map_pointer nfinish = nstart + nNode - 1;
    
    try {
        // 创建实际需要的缓冲区
        create_buffer(nstart, nfinish);
    }
    catch (...) {
        // 异常处理，清理已分配的资源
        map_allocator.deallocate(map_, map_size_);
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }
    
    // 设置迭代器位置
    begin_.set_node(nstart);
    end_.set_node(nfinish);
    begin_.cur = begin_.first;
    // 计算最后一个缓冲区中的元素偏移位置
    end_.cur = end_.first + (nElem % buffer_size);
}

/**
 * @brief 以指定值填充初始化
 */
template <class T>
void deque<T>::fill_init(size_type n, const value_type& value) {
    // 初始化map
    map_init(n);
    
    if (n != 0) {
        // 填充缓冲区
        for (auto cur = begin_.node; cur < end_.node; ++cur) {
            std::uninitialized_fill(*cur, *cur + buffer_size, value);
        }
        // 对最后一个缓冲区单独处理
        std::uninitialized_fill(end_.first, end_.cur, value);
    }
}

/**
 * @brief 从输入迭代器复制初始化(输入迭代器版本)
 */
template <class T>
template <class InputIter>
void deque<T>::copy_init(InputIter first, InputIter last, std::input_iterator_tag) {
    // 计算元素个数
    const size_type n = std::distance(first, last);
    // 初始化map
    map_init(n);
    
    // 逐个复制元素
    for (; first != last; ++first) {
        emplace_back(*first);
    }
}

/**
 * @brief 从前向迭代器复制初始化(前向迭代器版本)
 */
template <class T>
template <class ForwardIter>
void deque<T>::copy_init(ForwardIter first, ForwardIter last, std::forward_iterator_tag) {
    // 计算元素个数
    const size_type n = std::distance(first, last);
    // 初始化map
    map_init(n);
    
    // 逐个缓冲区填充
    for (auto cur = begin_.node; cur < end_.node; ++cur) {
        auto next = first;
        std::advance(next, buffer_size);
        std::uninitialized_copy(first, next, *cur);
        first = next;
    }
    
    // 对最后一个缓冲区单独处理
    std::uninitialized_copy(first, last, end_.first);
}

/**
 * @brief 在容器头部添加元素
 */
template <class T>
void deque<T>::push_front(const value_type& value) {
    if (begin_.cur != begin_.first) {
        // 缓冲区前部有剩余空间
        // 在当前位置之前构造元素
        data_allocator.construct(begin_.cur - 1, value);
        --begin_.cur;
    } else {
        // 需要在前面分配新的缓冲区
        require_capacity(1, true);
        try {
            --begin_;
            data_allocator.construct(begin_.cur, value);
        } catch (...) {
            ++begin_;
            throw;
        }
    }
}

/**
 * @brief 在容器尾部添加元素
 */
template <class T>
void deque<T>::push_back(const value_type& value) {
    if (end_.cur != end_.last - 1) {
        // 缓冲区尾部有剩余空间
        // 在当前位置构造元素
        data_allocator.construct(end_.cur, value);
        ++end_.cur;
    } else {
        // 需要在后面分配新的缓冲区
        require_capacity(1, false);
        data_allocator.construct(end_.cur, value);
        ++end_;
    }
}

/**
 * @brief 在容器头部原地构造元素
 */
template <class T>
template <class... Args>
void deque<T>::emplace_front(Args&&... args) {
    if (begin_.cur != begin_.first) {
        // 缓冲区前部有剩余空间
        data_allocator.construct(begin_.cur - 1, std::forward<Args>(args)...);
        --begin_.cur;
    } else {
        // 需要在前面分配新的缓冲区
        require_capacity(1, true);
        try {
            --begin_;
            data_allocator.construct(begin_.cur, std::forward<Args>(args)...);
        } catch (...) {
            ++begin_;
            throw;
        }
    }
}

/**
 * @brief 在容器尾部原地构造元素
 */
template <class T>
template <class... Args>
void deque<T>::emplace_back(Args&&... args) {
    if (end_.cur != end_.last - 1) {
        // 缓冲区尾部有剩余空间
        data_allocator.construct(end_.cur, std::forward<Args>(args)...);
        ++end_.cur;
    } else {
        // 需要在后面分配新的缓冲区
        require_capacity(1, false);
        data_allocator.construct(end_.cur, std::forward<Args>(args)...);
        ++end_;
    }
}

/**
 * @brief 从容器头部删除元素
 */
template <class T>
void deque<T>::pop_front() {
    if (empty()) {
        return;
    }
    
    if (begin_.cur != begin_.last - 1) {
        // 不是缓冲区最后一个元素
        // 销毁当前元素
        data_allocator.destroy(begin_.cur);
        ++begin_.cur;
    } else {
        // 销毁当前元素
        data_allocator.destroy(begin_.cur);
        ++begin_; // 移动到下一个缓冲区
        // 释放空缓冲区
        destroy_buffer(begin_.node - 1, begin_.node - 1);
    }
}

/**
 * @brief 从容器尾部删除元素
 */
template <class T>
void deque<T>::pop_back() {
    if (empty()) {
        return;
    }
    
    if (end_.cur != end_.first) {
        // 不是缓冲区第一个元素
        // 销毁当前元素
        --end_.cur;
        data_allocator.destroy(end_.cur);
    } else {
        // 移动到前一个缓冲区
        --end_;
        // 销毁当前元素
        data_allocator.destroy(end_.cur);
        // 释放空缓冲区
        destroy_buffer(end_.node + 1, end_.node + 1);
    }
}

/**
 * @brief 清空容器
 */
template <class T>
void deque<T>::clear() {
    // 保留头部缓冲区
    
    // 销毁中间缓冲区的所有元素
    for (map_pointer cur = begin_.node + 1; cur < end_.node; ++cur) {
        for (pointer p = *cur; p < *cur + buffer_size; ++p) {
            data_allocator.destroy(p);
        }
    }
    
    if (begin_.node != end_.node) { // 有多个缓冲区
        // 销毁第一个缓冲区中的元素
        for (pointer p = begin_.cur; p < begin_.last; ++p) {
            data_allocator.destroy(p);
        }
        // 销毁最后一个缓冲区中的元素
        for (pointer p = end_.first; p < end_.cur; ++p) {
            data_allocator.destroy(p);
        }
    } else { // 只有一个缓冲区
        // 销毁当前缓冲区中的所有元素
        for (pointer p = begin_.cur; p < end_.cur; ++p) {
            data_allocator.destroy(p);
        }
    }
    
    // 释放多余的缓冲区
    shrink_to_fit();
    
    // 重置迭代器
    end_ = begin_;
}

/**
 * @brief 确保容器有足够的容量
 */
template <class T>
void deque<T>::require_capacity(size_type n, bool front) {
    if (front && (static_cast<size_type>(begin_.cur - begin_.first) < n)) {
        // 前端空间不足
        // 计算需要额外分配的缓冲区数量
        const size_type need_buffer = (n - (begin_.cur - begin_.first)) / buffer_size + 1;
        
        // 检查map前端是否有足够空间放置新缓冲区
        if (need_buffer > static_cast<size_type>(begin_.node - map_)) {
            // map前端空间不足，需要重新分配map
            reallocate_map_at_front(need_buffer);
            return;
        }
        
        // 在前端创建新的缓冲区
        create_buffer(begin_.node - need_buffer, begin_.node - 1);
    }
    else if (!front && (static_cast<size_type>(end_.last - end_.cur - 1) < n)) {
        // 后端空间不足
        // 计算需要额外分配的缓冲区数量
        const size_type need_buffer = (n - (end_.last - end_.cur - 1)) / buffer_size + 1;
        
        // 检查map后端是否有足够空间放置新缓冲区
        if (need_buffer > static_cast<size_type>((map_ + map_size_) - end_.node - 1)) {
            // map后端空间不足，需要重新分配map
            reallocate_map_at_back(need_buffer);
            return;
        }
        
        // 在后端创建新的缓冲区
        create_buffer(end_.node + 1, end_.node + need_buffer);
    }
}

/**
 * @brief 在头部重新分配map
 */
template <class T>
void deque<T>::reallocate_map_at_front(size_type need_buffer) {
    // 分配一个更大的map
    // 新大小至少是原来的两倍或者足够放置新缓冲区
    const size_type new_map_size = std::max(map_size_ << 1,
                                           map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE);
    
    // 创建新map
    map_pointer new_map = create_map(new_map_size);
    const size_type old_buffer = end_.node - begin_.node + 1;
    const size_type new_buffer = old_buffer + need_buffer;
    
    // 计算新的缓冲区位置，保持在新map中居中
    auto begin = new_map + (new_map_size - new_buffer) / 2;
    auto mid = begin + need_buffer;
    auto end = mid + old_buffer;
    
    // 为新增的缓冲区申请内存
    create_buffer(begin, mid - 1);
    
    // 把旧map中的缓冲区指针复制到新map对应位置
    for (auto begin1 = mid, begin2 = begin_.node; begin1 != end; ++begin1, ++begin2) {
        *begin1 = *begin2;
    }
    
    // 更新迭代器
    begin_ = iterator(*mid + (begin_.cur - begin_.first), mid);
    end_ = iterator(*(end - 1) + (end_.cur - end_.first), end - 1);
    
    // 释放旧map
    map_allocator.deallocate(map_, map_size_);
    map_ = new_map;
    map_size_ = new_map_size;
}

/**
 * @brief 在尾部重新分配map
 */
template <class T>
void deque<T>::reallocate_map_at_back(size_type need_buffer) {
    // 分配一个更大的map
    // 新大小至少是原来的两倍或者足够放置新缓冲区
    const size_type new_map_size = std::max(map_size_ << 1,
                                           map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE);
    
    // 创建新map
    map_pointer new_map = create_map(new_map_size);
    const size_type old_buffer = end_.node - begin_.node + 1;
    const size_type new_buffer = old_buffer + need_buffer;
    
    // 计算新的缓冲区位置，保持在新map中居中
    auto begin = new_map + (new_map_size - new_buffer) / 2;
    auto mid = begin + old_buffer;
    auto end = mid + need_buffer;
    
    // 把旧map中的缓冲区指针复制到新map对应位置
    for (auto begin1 = begin, begin2 = begin_.node; begin1 != mid; ++begin1, ++begin2) {
        *begin1 = *begin2;
    }
    
    // 为新增的缓冲区申请内存
    create_buffer(mid, end - 1);
    
    // 更新迭代器
    begin_ = iterator(*begin + (begin_.cur - begin_.first), begin);
    end_ = iterator(*(mid - 1) + (end_.cur - end_.first), mid - 1);
    
    // 释放旧map
    map_allocator.deallocate(map_, map_size_);
    map_ = new_map;
    map_size_ = new_map_size;
}

/**
 * @brief 交换两个容器的内容
 */
template <class T>
void deque<T>::swap(deque& rhs) noexcept {
    if (this != &rhs) {
        std::swap(begin_, rhs.begin_);
        std::swap(end_, rhs.end_);
        std::swap(map_, rhs.map_);
        std::swap(map_size_, rhs.map_size_);
    }
}

/**
 * @brief 重载比较操作符==
 */
template <class T>
bool operator==(const deque<T>& lhs, const deque<T>& rhs) {
    return lhs.size() == rhs.size() && 
           std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

/**
 * @brief 重载比较操作符<
 */
template <class T>
bool operator<(const deque<T>& lhs, const deque<T>& rhs) {
    return std::lexicographical_compare(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/**
 * @brief 重载比较操作符!=
 */
template <class T>
bool operator!=(const deque<T>& lhs, const deque<T>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief 重载比较操作符>
 */
template <class T>
bool operator>(const deque<T>& lhs, const deque<T>& rhs) {
    return rhs < lhs;
}

/**
 * @brief 重载比较操作符<=
 */
template <class T>
bool operator<=(const deque<T>& lhs, const deque<T>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief 重载比较操作符>=
 */
template <class T>
bool operator>=(const deque<T>& lhs, const deque<T>& rhs) {
    return !(lhs < rhs);
}

/**
 * @brief 重载全局swap函数
 */
template <class T>
void swap(deque<T>& lhs, deque<T>& rhs) noexcept {
    lhs.swap(rhs);
}

/**
 * @brief 在指定位置插入元素
 */
template <class T>
typename deque<T>::iterator
deque<T>::insert(iterator pos, const value_type& value) {
    if (pos.cur == begin_.cur) {
        // 在头部插入
        push_front(value);
        return begin_;
    }
    else if (pos.cur == end_.cur) {
        // 在尾部插入
        push_back(value);
        auto tmp = end_;
        --tmp;
        return tmp;
    }
    else {
        // 在中间插入
        return insert_aux(pos, value);
    }
}

/**
 * @brief 在指定位置插入元素(移动版本)
 */
template <class T>
typename deque<T>::iterator
deque<T>::insert(iterator pos, value_type&& value) {
    if (pos.cur == begin_.cur) {
        // 在头部插入
        emplace_front(std::move(value));
        return begin_;
    }
    else if (pos.cur == end_.cur) {
        // 在尾部插入
        emplace_back(std::move(value));
        auto tmp = end_;
        --tmp;
        return tmp;
    }
    else {
        // 在中间插入
        return insert_aux(pos, std::move(value));
    }
}

/**
 * @brief 在指定位置插入n个元素
 */
template <class T>
void deque<T>::insert(iterator pos, size_type n, const value_type& value) {
    if (pos.cur == begin_.cur) {
        // 在头部插入
        require_capacity(n, true);
        auto new_begin = begin_ - n;
        std::uninitialized_fill_n(new_begin, n, value);
        begin_ = new_begin;
    }
    else if (pos.cur == end_.cur) {
        // 在尾部插入
        require_capacity(n, false);
        auto new_end = end_ + n;
        std::uninitialized_fill_n(end_, n, value);
        end_ = new_end;
    }
    else {
        // 在中间插入
        fill_insert(pos, n, value);
    }
}

/**
 * @brief 在指定位置插入元素的辅助函数
 */
template <class T>
template <class... Args>
typename deque<T>::iterator
deque<T>::insert_aux(iterator pos, Args&&... args) {
    const size_type elems_before = pos - begin_;
    value_type value_copy(std::forward<Args>(args)...);
    
    if (elems_before < (size() / 2)) {
        // 前半段，向前移动元素
        emplace_front(front()); // 在前端添加一个元素
        
        // 移动元素
        auto front1 = begin_;
        ++front1;
        auto front2 = front1;
        ++front2;
        pos = begin_ + elems_before;
        auto pos1 = pos;
        ++pos1;
        std::copy(front2, pos1, front1);
    }
    else {
        // 后半段，向后移动元素
        emplace_back(back()); // 在后端添加一个元素
        
        // 移动元素
        auto back1 = end_;
        --back1;
        auto back2 = back1;
        --back2;
        pos = begin_ + elems_before;
        std::copy_backward(pos, back2, back1);
    }
    
    // 在指定位置放入新元素
    *pos = std::move(value_copy);
    return pos;
}

/**
 * @brief 在指定位置原地构造元素
 */
template <class T>
template <class... Args>
typename deque<T>::iterator
deque<T>::emplace(iterator pos, Args&&... args) {
    if (pos.cur == begin_.cur) {
        // 在头部插入
        emplace_front(std::forward<Args>(args)...);
        return begin_;
    }
    else if (pos.cur == end_.cur) {
        // 在尾部插入
        emplace_back(std::forward<Args>(args)...);
        auto tmp = end_;
        --tmp;
        return tmp;
    }
    else {
        // 在中间插入
        return insert_aux(pos, std::forward<Args>(args)...);
    }
}

/**
 * @brief 在指定位置填充插入元素
 */
template <class T>
void deque<T>::fill_insert(iterator pos, size_type n, const value_type& value) {
    const size_type elems_before = pos - begin_;
    const size_type len = size();
    auto value_copy = value;
    
    if (elems_before < (len / 2)) {
        // 前半段，扩展前端
        require_capacity(n, true);
        
        // 原来的迭代器可能会失效，重新计算位置
        auto old_begin = begin_;
        auto new_begin = begin_ - n;
        pos = begin_ + elems_before;
        
        try {
            if (elems_before >= n) {
                // 前端元素足够移动到前面新分配的空间
                auto begin_n = begin_ + n;
                std::uninitialized_copy(begin_, begin_n, new_begin);
                begin_ = new_begin;
                std::copy(begin_n, pos, old_begin);
                std::fill(pos - n, pos, value_copy);
            }
            else {
                // 前端元素不足，分两段处理
                // 先将前端元素移动到新空间，剩余空间用value填充
                auto mid = std::uninitialized_copy(begin_, pos, new_begin);
                std::uninitialized_fill(mid, begin_, value_copy);
                begin_ = new_begin;
                std::fill(old_begin, pos, value_copy);
            }
        }
        catch (...) {
            // 异常处理
            if (new_begin.node != begin_.node)
                destroy_buffer(new_begin.node, begin_.node - 1);
            throw;
        }
    }
    else {
        // 后半段，扩展后端
        require_capacity(n, false);
        
        // 原来的迭代器可能会失效，重新计算位置
        auto old_end = end_;
        auto new_end = end_ + n;
        const size_type elems_after = len - elems_before;
        pos = end_ - elems_after;
        
        try {
            if (elems_after > n) {
                // 后端元素足够移动到后面新分配的空间
                auto end_n = end_ - n;
                std::uninitialized_copy(end_n, end_, end_);
                end_ = new_end;
                std::copy_backward(pos, end_n, old_end);
                std::fill(pos, pos + n, value_copy);
            }
            else {
                // 后端元素不足，分两段处理
                // 先用value填充新空间一部分，再将后端元素移动到剩余空间
                std::uninitialized_fill(end_, pos + n, value_copy);
                std::uninitialized_copy(pos, end_, pos + n);
                end_ = new_end;
                std::fill(pos, old_end, value_copy);
            }
        }
        catch (...) {
            // 异常处理
            if (new_end.node != end_.node)
                destroy_buffer(end_.node + 1, new_end.node);
            throw;
        }
    }
}

/**
 * @brief 删除指定位置的元素
 */
template <class T>
typename deque<T>::iterator
deque<T>::erase(iterator pos) {
    auto next = pos;
    ++next;
    const size_type elems_before = pos - begin_;
    
    if (elems_before < (size() / 2)) {
        // 前半段，将前面元素向后移动
        std::copy_backward(begin_, pos, next);
        pop_front();
    }
    else {
        // 后半段，将后面元素向前移动
        std::copy(next, end_, pos);
        pop_back();
    }
    
    return begin_ + elems_before;
}

/**
 * @brief 删除范围内的元素
 */
template <class T>
typename deque<T>::iterator
deque<T>::erase(iterator first, iterator last) {
    if (first == begin_ && last == end_) {
        // 清空整个容器
        clear();
        return end_;
    }
    else {
        // 计算要删除的元素个数
        const size_type len = last - first;
        const size_type elems_before = first - begin_;
        
        if (elems_before < ((size() - len) / 2)) {
            // 前半段，将前面元素向后移动
            std::copy_backward(begin_, first, last);
            
            // 删除前面的元素
            auto new_begin = begin_ + len;
            // 销毁多余元素
            for (auto cur = begin_.cur; cur != new_begin.cur; ++cur) {
                data_allocator.destroy(cur);
            }
            begin_ = new_begin;
        }
        else {
            // 后半段，将后面元素向前移动
            std::copy(last, end_, first);
            
            // 删除后面的元素
            auto new_end = end_ - len;
            // 销毁多余元素
            for (auto cur = new_end.cur; cur != end_.cur; ++cur) {
                data_allocator.destroy(cur);
            }
            end_ = new_end;
        }
        
        return begin_ + elems_before;
    }
}

} // namespace mystl

#endif // MY_DEQUE_H 