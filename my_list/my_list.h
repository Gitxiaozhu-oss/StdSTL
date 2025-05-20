#ifndef MY_LIST_H
#define MY_LIST_H

/**
 * @file my_list.h
 * @brief 实现一个C++11标准的双向链表容器
 * 
 * 该文件实现了一个符合C++11标准的list容器，针对g++编译器进行了优化
 */

#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <limits>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <utility>

namespace mystl {

/**
 * @brief 链表节点基础结构的前置声明
 */
template <typename T> struct list_node_base;

/**
 * @brief 链表节点结构的前置声明
 */
template <typename T> struct list_node;

/**
 * @brief 节点特性类，用于获取不同类型节点的指针类型
 */
template <typename T>
struct node_traits {
    using base_ptr = list_node_base<T>*;  // 基础节点指针类型
    using node_ptr = list_node<T>*;       // 数据节点指针类型
};

/**
 * @brief 链表的基础节点结构
 * 
 * 包含指向前后节点的指针，构成链表的连接关系
 */
template <typename T>
struct list_node_base {
    using base_ptr = typename node_traits<T>::base_ptr;
    using node_ptr = typename node_traits<T>::node_ptr;
    
    base_ptr prev;  // 前一个节点
    base_ptr next;  // 后一个节点
    
    /**
     * @brief 默认构造函数
     */
    list_node_base() = default;
    
    /**
     * @brief 将当前节点转换为具体数据节点
     * @return 数据节点指针
     */
    node_ptr as_node() {
        return static_cast<node_ptr>(self());
    }
    
    /**
     * @brief 断开当前节点与其他节点的连接，将前后指针指向自身
     */
    void unlink() {
        prev = next = self();
    }
    
    /**
     * @brief 获取指向自身的指针
     * @return 基础节点指针
     */
    base_ptr self() {
        return static_cast<base_ptr>(&*this);
    }
};

/**
 * @brief 链表的数据节点结构，继承自基础节点
 * 
 * 在基础节点的基础上增加了数据成员
 */
template <typename T>
struct list_node : public list_node_base<T> {
    using base_ptr = typename node_traits<T>::base_ptr;
    using node_ptr = typename node_traits<T>::node_ptr;
    
    T value;  // 数据域，存储实际元素值
    
    /**
     * @brief 默认构造函数
     */
    list_node() = default;
    
    /**
     * @brief 使用特定值构造节点
     * @param v 要存储的值
     */
    list_node(const T& v) : value(v) {}
    
    /**
     * @brief 使用右值构造节点（移动语义）
     * @param v 要移动的值
     */
    list_node(T&& v) : value(std::move(v)) {}
    
    /**
     * @brief 将当前节点转换为基础节点
     * @return 基础节点指针
     */
    base_ptr as_base() {
        return static_cast<base_ptr>(&*this);
    }
    
    /**
     * @brief 获取指向自身的指针
     * @return 数据节点指针
     */
    node_ptr self() {
        return static_cast<node_ptr>(&*this);
    }
};

/**
 * @brief list的迭代器设计，双向迭代器
 */
template <typename T>
class list_iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
public:
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using base_ptr = typename node_traits<T>::base_ptr;
    using node_ptr = typename node_traits<T>::node_ptr;
    using self = list_iterator<T>;
    
    base_ptr node_;  // 指向当前节点的指针
    
    /**
     * @brief 默认构造函数
     */
    list_iterator() = default;
    
    /**
     * @brief 使用基础节点指针构造迭代器
     * @param x 基础节点指针
     */
    list_iterator(base_ptr x) : node_(x) {}
    
    /**
     * @brief 使用数据节点指针构造迭代器
     * @param x 数据节点指针
     */
    list_iterator(node_ptr x) : node_(x->as_base()) {}
    
    /**
     * @brief 拷贝构造函数
     * @param rhs 源迭代器
     */
    list_iterator(const list_iterator& rhs) : node_(rhs.node_) {}
    
    /**
     * @brief 解引用操作符，获取节点的值
     * @return 节点值的引用
     */
    reference operator*() const { return node_->as_node()->value; }
    
    /**
     * @brief 箭头操作符，用于访问节点数据的成员
     * @return 指向节点值的指针
     */
    pointer operator->() const { return &(operator*()); }
    
    /**
     * @brief 前置自增操作符，指向下一个节点
     * @return 自增后的迭代器引用
     */
    self& operator++() {
        node_ = node_->next;
        return *this;
    }
    
    /**
     * @brief 后置自增操作符，指向下一个节点
     * @return 自增前的迭代器副本
     */
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    
    /**
     * @brief 前置自减操作符，指向前一个节点
     * @return 自减后的迭代器引用
     */
    self& operator--() {
        node_ = node_->prev;
        return *this;
    }
    
    /**
     * @brief 后置自减操作符，指向前一个节点
     * @return 自减前的迭代器副本
     */
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
    
    /**
     * @brief 相等比较操作符
     * @param rhs 要比较的迭代器
     * @return 是否相等
     */
    bool operator==(const self& rhs) const { return node_ == rhs.node_; }
    
    /**
     * @brief 不等比较操作符
     * @param rhs 要比较的迭代器
     * @return 是否不等
     */
    bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
};

/**
 * @brief list的常量迭代器设计，双向迭代器
 */
template <typename T>
class list_const_iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
public:
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;
    using base_ptr = typename node_traits<T>::base_ptr;
    using node_ptr = typename node_traits<T>::node_ptr;
    using self = list_const_iterator<T>;
    
    base_ptr node_;  // 指向当前节点的指针
    
    /**
     * @brief 默认构造函数
     */
    list_const_iterator() = default;
    
    /**
     * @brief 使用基础节点指针构造迭代器
     * @param x 基础节点指针
     */
    list_const_iterator(base_ptr x) : node_(x) {}
    
    /**
     * @brief 使用数据节点指针构造迭代器
     * @param x 数据节点指针
     */
    list_const_iterator(node_ptr x) : node_(x->as_base()) {}
    
    /**
     * @brief 使用非常量迭代器构造常量迭代器
     * @param rhs 非常量迭代器
     */
    list_const_iterator(const list_iterator<T>& rhs) : node_(rhs.node_) {}
    
    /**
     * @brief 拷贝构造函数
     * @param rhs 源常量迭代器
     */
    list_const_iterator(const list_const_iterator& rhs) : node_(rhs.node_) {}
    
    /**
     * @brief 解引用操作符，获取节点的值
     * @return 节点值的常量引用
     */
    reference operator*() const { return node_->as_node()->value; }
    
    /**
     * @brief 箭头操作符，用于访问节点数据的成员
     * @return 指向节点值的常量指针
     */
    pointer operator->() const { return &(operator*()); }
    
    /**
     * @brief 前置自增操作符，指向下一个节点
     * @return 自增后的迭代器引用
     */
    self& operator++() {
        node_ = node_->next;
        return *this;
    }
    
    /**
     * @brief 后置自增操作符，指向下一个节点
     * @return 自增前的迭代器副本
     */
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    
    /**
     * @brief 前置自减操作符，指向前一个节点
     * @return 自减后的迭代器引用
     */
    self& operator--() {
        node_ = node_->prev;
        return *this;
    }
    
    /**
     * @brief 后置自减操作符，指向前一个节点
     * @return 自减前的迭代器副本
     */
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
    
    /**
     * @brief 相等比较操作符
     * @param rhs 要比较的迭代器
     * @return 是否相等
     */
    bool operator==(const self& rhs) const { return node_ == rhs.node_; }
    
    /**
     * @brief 不等比较操作符
     * @param rhs 要比较的迭代器
     * @return 是否不等
     */
    bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
};

/**
 * @brief 双向链表容器类定义
 * 
 * @tparam T 元素类型
 */
template <typename T>
class list {
public:
    /**
     * @brief list容器相关类型定义
     */
    using allocator_type = std::allocator<T>;
    using data_allocator = std::allocator<T>;
    using base_allocator = std::allocator<list_node_base<T>>;
    using node_allocator = std::allocator<list_node<T>>;
    
    using value_type = T;
    using pointer = typename allocator_type::pointer;
    using const_pointer = typename allocator_type::const_pointer;
    using reference = typename allocator_type::reference;
    using const_reference = typename allocator_type::const_reference;
    using size_type = typename allocator_type::size_type;
    using difference_type = typename allocator_type::difference_type;
    
    using iterator = list_iterator<T>;
    using const_iterator = list_const_iterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    using base_ptr = typename node_traits<T>::base_ptr;
    using node_ptr = typename node_traits<T>::node_ptr;
    
    /**
     * @brief 获取分配器
     * @return 分配器对象
     */
    allocator_type get_allocator() { return node_allocator(); }

private:
    base_ptr node_;  // 末尾哨兵节点，表示链表的结束位置
    size_type size_; // 链表大小
    
public:
    // 以下是list类的接口声明，后面会给出具体实现
    
    // 构造函数、析构函数和赋值运算符
    list();
    explicit list(size_type n);
    list(size_type n, const T& value);
    
    template <class InputIter, typename = typename std::enable_if<
        std::is_convertible<typename std::iterator_traits<InputIter>::iterator_category, 
        std::input_iterator_tag>::value>::type>
    list(InputIter first, InputIter last);
    
    list(std::initializer_list<T> ilist);
    list(const list& rhs);
    list(list&& rhs) noexcept;
    
    list& operator=(const list& rhs);
    list& operator=(list&& rhs) noexcept;
    list& operator=(std::initializer_list<T> ilist);
    
    ~list();
    
    // 迭代器相关操作
    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    
    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;
    
    // 容量相关操作
    bool empty() const noexcept;
    size_type size() const noexcept;
    size_type max_size() const noexcept;
    
    // 元素访问操作
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;
    
    // 修改容器操作
    
    // assign操作
    void assign(size_type n, const value_type& value);
    
    template <class InputIter, typename = typename std::enable_if<
        std::is_convertible<typename std::iterator_traits<InputIter>::iterator_category, 
        std::input_iterator_tag>::value>::type>
    void assign(InputIter first, InputIter last);
    
    void assign(std::initializer_list<T> ilist);
    
    // emplace操作
    template <class... Args>
    void emplace_front(Args&&... args);
    
    template <class... Args>
    void emplace_back(Args&&... args);
    
    template <class... Args>
    iterator emplace(const_iterator pos, Args&&... args);
    
    // insert操作
    iterator insert(const_iterator pos, const value_type& value);
    iterator insert(const_iterator pos, value_type&& value);
    iterator insert(const_iterator pos, size_type n, const value_type& value);
    
    template <class InputIter, typename = typename std::enable_if<
        std::is_convertible<typename std::iterator_traits<InputIter>::iterator_category, 
        std::input_iterator_tag>::value>::type>
    iterator insert(const_iterator pos, InputIter first, InputIter last);
    
    iterator insert(const_iterator pos, std::initializer_list<T> ilist);
    
    // push/pop操作
    void push_front(const value_type& value);
    void push_front(value_type&& value);
    void push_back(const value_type& value);
    void push_back(value_type&& value);
    void pop_front();
    void pop_back();
    
    // erase/clear操作
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    void clear();
    
    // resize操作
    void resize(size_type new_size);
    void resize(size_type new_size, const value_type& value);
    
    // swap操作
    void swap(list& rhs) noexcept;
    
    // list特有操作
    void splice(const_iterator pos, list& other);
    void splice(const_iterator pos, list& other, const_iterator it);
    void splice(const_iterator pos, list& other, const_iterator first, const_iterator last);
    
    void remove(const value_type& value);
    template <class UnaryPredicate>
    void remove_if(UnaryPredicate pred);
    
    void unique();
    template <class BinaryPredicate>
    void unique(BinaryPredicate pred);
    
    void merge(list& x);
    template <class Compare>
    void merge(list& x, Compare comp);
    
    void sort();
    template <class Compare>
    void sort(Compare comp);
    
    void reverse() noexcept;
    
private:
    // 以下是辅助函数声明，后面会给出具体实现
    
    // 创建/销毁节点
    template <class... Args>
    node_ptr create_node(Args&&... args);
    void destroy_node(node_ptr p);
    
    // 初始化
    void init();
    void fill_init(size_type n, const value_type& value);
    template <class InputIter>
    void copy_init(InputIter first, InputIter last);
    
    // 链接/断开节点
    iterator link_iter_node(const_iterator pos, base_ptr node);
    void link_nodes(base_ptr pos, base_ptr first, base_ptr last);
    void link_nodes_at_front(base_ptr first, base_ptr last);
    void link_nodes_at_back(base_ptr first, base_ptr last);
    void unlink_nodes(base_ptr first, base_ptr last);
    
    // 赋值操作
    void fill_assign(size_type n, const value_type& value);
    template <class InputIter>
    void copy_assign(InputIter first, InputIter last);
    
    // 插入操作
    iterator fill_insert(const_iterator pos, size_type n, const value_type& value);
    template <class InputIter>
    iterator copy_insert(const_iterator pos, InputIter first, InputIter last);
    
    // 排序操作
    template <class Compare>
    iterator list_sort(iterator first, iterator last, size_type n, Compare comp);
};

// 非成员函数
template <typename T>
bool operator==(const list<T>& lhs, const list<T>& rhs);

template <typename T>
bool operator<(const list<T>& lhs, const list<T>& rhs);

template <typename T>
bool operator!=(const list<T>& lhs, const list<T>& rhs);

template <typename T>
bool operator>(const list<T>& lhs, const list<T>& rhs);

template <typename T>
bool operator<=(const list<T>& lhs, const list<T>& rhs);

template <typename T>
bool operator>=(const list<T>& lhs, const list<T>& rhs);

// 重载mystl的swap
template <typename T>
void swap(list<T>& lhs, list<T>& rhs) noexcept;

// 以下是list类的具体实现

/**
 * @brief 创建节点的辅助函数
 * @param args 构造节点值的参数
 * @return 创建的节点指针
 */
template <typename T>
template <class... Args>
typename list<T>::node_ptr list<T>::create_node(Args&&... args) {
    node_ptr p = node_allocator().allocate(1);
    try {
        // 在节点上构造值
        data_allocator().construct(std::addressof(p->value), std::forward<Args>(args)...);
        p->prev = nullptr;
        p->next = nullptr;
    }
    catch (...) {
        node_allocator().deallocate(p, 1);
        throw;
    }
    return p;
}

/**
 * @brief 销毁节点的辅助函数
 * @param p 要销毁的节点指针
 */
template <typename T>
void list<T>::destroy_node(node_ptr p) {
    data_allocator().destroy(std::addressof(p->value));
    node_allocator().deallocate(p, 1);
}

/**
 * @brief 初始化链表哨兵节点
 */
template <typename T>
void list<T>::init() {
    node_ = base_allocator().allocate(1);
    node_->unlink();  // 初始化哨兵节点的前后指针为自身
    size_ = 0;
}

/**
 * @brief 从迭代器范围复制数据进行初始化
 * @param first 起始迭代器
 * @param last 结束迭代器
 */
template <typename T>
template <class InputIter>
void list<T>::copy_init(InputIter first, InputIter last) {
    init();
    try {
        for (; first != last; ++first) {
            push_back(*first);
        }
    }
    catch (...) {
        clear();
        base_allocator().deallocate(node_, 1);
        node_ = nullptr;
        throw;
    }
}

/**
 * @brief 使用n个指定值初始化链表
 * @param n 元素个数
 * @param value 元素值
 */
template <typename T>
void list<T>::fill_init(size_type n, const value_type& value) {
    init();
    try {
        for (size_type i = 0; i < n; ++i) {
            push_back(value);
        }
    }
    catch (...) {
        clear();
        base_allocator().deallocate(node_, 1);
        node_ = nullptr;
        throw;
    }
}

/**
 * @brief 用初始化列表构造链表
 * @param ilist 初始化列表
 */
template <typename T>
list<T>::list(std::initializer_list<T> ilist) {
    init();
    try {
        for (auto& item : ilist) {
            push_back(item);
        }
    }
    catch (...) {
        clear();
        base_allocator().deallocate(node_, 1);
        node_ = nullptr;
        throw;
    }
}

/**
 * @brief 默认构造函数
 */
template <typename T>
list<T>::list() {
    init();
}

/**
 * @brief 指定大小的构造函数，使用默认值填充
 * @param n 元素个数
 */
template <typename T>
list<T>::list(size_type n) {
    fill_init(n, value_type());
}

/**
 * @brief 指定大小和值的构造函数
 * @param n 元素个数
 * @param value 填充值
 */
template <typename T>
list<T>::list(size_type n, const T& value) {
    fill_init(n, value);
}

/**
 * @brief 使用迭代器范围的构造函数
 * @param first 起始迭代器
 * @param last 结束迭代器
 */
template <typename T>
template <class InputIter, typename>
list<T>::list(InputIter first, InputIter last) {
    copy_init(first, last);
}

/**
 * @brief 拷贝构造函数
 * @param rhs 源list
 */
template <typename T>
list<T>::list(const list& rhs) {
    copy_init(rhs.cbegin(), rhs.cend());
}

/**
 * @brief 移动构造函数
 * @param rhs 源list（右值引用）
 */
template <typename T>
list<T>::list(list&& rhs) noexcept : node_(rhs.node_), size_(rhs.size_) {
    // 初始化一个空的哨兵节点给rhs，而不是设为nullptr
    rhs.init();
}

/**
 * @brief 析构函数
 */
template <typename T>
list<T>::~list() {
    clear();
    base_allocator().deallocate(node_, 1);
    node_ = nullptr;
}

/**
 * @brief 连接节点的辅助函数，返回连接后的迭代器
 * @param pos 插入位置
 * @param node 要连接的节点
 * @return 指向新连接节点的迭代器
 */
template <typename T>
typename list<T>::iterator list<T>::link_iter_node(const_iterator pos, base_ptr node) {
    if (pos == node_->next) {
        link_nodes_at_front(node, node);
    }
    else if (pos == node_) {
        link_nodes_at_back(node, node);
    }
    else {
        link_nodes(pos.node_, node, node);
    }
    return iterator(node);
}

/**
 * @brief 在pos前连接[first, last]范围的节点
 * @param pos 连接位置
 * @param first 要连接范围的开始节点
 * @param last 要连接范围的结束节点
 */
template <typename T>
void list<T>::link_nodes(base_ptr pos, base_ptr first, base_ptr last) {
    pos->prev->next = first;
    first->prev = pos->prev;
    pos->prev = last;
    last->next = pos;
}

/**
 * @brief 在链表前端连接[first, last]范围的节点
 * @param first 要连接范围的开始节点
 * @param last 要连接范围的结束节点
 */
template <typename T>
void list<T>::link_nodes_at_front(base_ptr first, base_ptr last) {
    first->prev = node_;
    last->next = node_->next;
    last->next->prev = last;
    node_->next = first;
}

/**
 * @brief 在链表后端连接[first, last]范围的节点
 * @param first 要连接范围的开始节点
 * @param last 要连接范围的结束节点
 */
template <typename T>
void list<T>::link_nodes_at_back(base_ptr first, base_ptr last) {
    last->next = node_;
    first->prev = node_->prev;
    first->prev->next = first;
    node_->prev = last;
}

/**
 * @brief 断开[first, last]范围的节点连接
 * @param first 要断开范围的开始节点
 * @param last 要断开范围的结束节点
 */
template <typename T>
void list<T>::unlink_nodes(base_ptr first, base_ptr last) {
    first->prev->next = last->next;
    last->next->prev = first->prev;
}

/**
 * @brief 拷贝赋值运算符
 * @param rhs 源链表
 * @return 当前链表的引用
 */
template <typename T>
list<T>& list<T>::operator=(const list& rhs) {
    if (this != &rhs) {
        assign(rhs.begin(), rhs.end());
    }
    return *this;
}

/**
 * @brief 移动赋值运算符
 * @param rhs 源链表（右值引用）
 * @return 当前链表的引用
 */
template <typename T>
list<T>& list<T>::operator=(list&& rhs) noexcept {
    clear();
    splice(end(), rhs);
    return *this;
}

/**
 * @brief 使用初始化列表赋值
 * @param ilist 初始化列表
 * @return 当前链表的引用
 */
template <typename T>
list<T>& list<T>::operator=(std::initializer_list<T> ilist) {
    list tmp(ilist.begin(), ilist.end());
    swap(tmp);
    return *this;
}

/**
 * @brief 指向链表第一个元素的迭代器
 * @return 开始迭代器
 */
template <typename T>
typename list<T>::iterator list<T>::begin() noexcept {
    return node_->next;
}

/**
 * @brief 指向链表第一个元素的常量迭代器
 * @return 常量开始迭代器
 */
template <typename T>
typename list<T>::const_iterator list<T>::begin() const noexcept {
    return node_->next;
}

/**
 * @brief 指向链表尾部的迭代器（超出末尾）
 * @return 结束迭代器
 */
template <typename T>
typename list<T>::iterator list<T>::end() noexcept {
    return node_;
}

/**
 * @brief 指向链表尾部的常量迭代器（超出末尾）
 * @return 常量结束迭代器
 */
template <typename T>
typename list<T>::const_iterator list<T>::end() const noexcept {
    return node_;
}

/**
 * @brief 反向迭代器的起始位置（对应容器的最后一个元素）
 * @return 反向开始迭代器
 */
template <typename T>
typename list<T>::reverse_iterator list<T>::rbegin() noexcept {
    return reverse_iterator(end());
}

/**
 * @brief 常量反向迭代器的起始位置
 * @return 常量反向开始迭代器
 */
template <typename T>
typename list<T>::const_reverse_iterator list<T>::rbegin() const noexcept {
    return const_reverse_iterator(end());
}

/**
 * @brief 反向迭代器的结束位置（对应容器的第一个元素之前）
 * @return 反向结束迭代器
 */
template <typename T>
typename list<T>::reverse_iterator list<T>::rend() noexcept {
    return reverse_iterator(begin());
}

/**
 * @brief 常量反向迭代器的结束位置
 * @return 常量反向结束迭代器
 */
template <typename T>
typename list<T>::const_reverse_iterator list<T>::rend() const noexcept {
    return const_reverse_iterator(begin());
}

/**
 * @brief 返回常量开始迭代器
 * @return 常量开始迭代器
 */
template <typename T>
typename list<T>::const_iterator list<T>::cbegin() const noexcept {
    return begin();
}

/**
 * @brief 返回常量结束迭代器
 * @return 常量结束迭代器
 */
template <typename T>
typename list<T>::const_iterator list<T>::cend() const noexcept {
    return end();
}

/**
 * @brief 返回常量反向开始迭代器
 * @return 常量反向开始迭代器
 */
template <typename T>
typename list<T>::const_reverse_iterator list<T>::crbegin() const noexcept {
    return rbegin();
}

/**
 * @brief 返回常量反向结束迭代器
 * @return 常量反向结束迭代器
 */
template <typename T>
typename list<T>::const_reverse_iterator list<T>::crend() const noexcept {
    return rend();
}

/**
 * @brief 判断容器是否为空
 * @return 如果容器为空返回true，否则返回false
 */
template <typename T>
bool list<T>::empty() const noexcept {
    return node_->next == node_;
}

/**
 * @brief 返回容器中的元素数量
 * @return 元素数量
 */
template <typename T>
typename list<T>::size_type list<T>::size() const noexcept {
    return size_;
}

/**
 * @brief 返回容器能容纳的最大元素数量
 * @return 最大元素数量
 */
template <typename T>
typename list<T>::size_type list<T>::max_size() const noexcept {
    return static_cast<size_type>(-1);
}

/**
 * @brief 访问容器第一个元素
 * @return 第一个元素的引用
 * @throw 如果容器为空，行为未定义
 */
template <typename T>
typename list<T>::reference list<T>::front() {
    return *begin();
}

/**
 * @brief 访问容器第一个元素（常量版本）
 * @return 第一个元素的常量引用
 * @throw 如果容器为空，行为未定义
 */
template <typename T>
typename list<T>::const_reference list<T>::front() const {
    return *begin();
}

/**
 * @brief 访问容器最后一个元素
 * @return 最后一个元素的引用
 * @throw 如果容器为空，行为未定义
 */
template <typename T>
typename list<T>::reference list<T>::back() {
    return *(--end());
}

/**
 * @brief 访问容器最后一个元素（常量版本）
 * @return 最后一个元素的常量引用
 * @throw 如果容器为空，行为未定义
 */
template <typename T>
typename list<T>::const_reference list<T>::back() const {
    return *(--end());
}

/**
 * @brief 用n个指定值填充容器
 * @param n 元素个数
 * @param value 填充值
 */
template <typename T>
void list<T>::fill_assign(size_type n, const value_type& value) {
    auto i = begin();
    auto e = end();
    for (; n > 0 && i != e; --n, ++i) {
        *i = value;
    }
    if (n > 0) {
        // 如果还有更多元素需要添加
        insert(e, n, value);
    }
    else {
        // 如果需要删除多余元素
        erase(i, e);
    }
}

/**
 * @brief 用迭代器范围内的元素给容器赋值
 * @param first 起始迭代器
 * @param last 结束迭代器
 */
template <typename T>
template <class InputIter>
void list<T>::copy_assign(InputIter first, InputIter last) {
    auto i = begin();
    auto e = end();
    for (; first != last && i != e; ++first, ++i) {
        *i = *first;
    }
    if (first == last) {
        // 如果输入迭代器用完了，删除多余元素
        erase(i, e);
    }
    else {
        // 如果容器空间用完了，插入剩余元素
        insert(e, first, last);
    }
}

/**
 * @brief 用n个指定值为容器赋值
 * @param n 元素个数
 * @param value 填充值
 */
template <typename T>
void list<T>::assign(size_type n, const value_type& value) {
    fill_assign(n, value);
}

/**
 * @brief 用迭代器范围内的元素为容器赋值
 * @param first 起始迭代器
 * @param last 结束迭代器
 */
template <typename T>
template <class InputIter, typename>
void list<T>::assign(InputIter first, InputIter last) {
    copy_assign(first, last);
}

/**
 * @brief 用初始化列表为容器赋值
 * @param ilist 初始化列表
 */
template <typename T>
void list<T>::assign(std::initializer_list<T> ilist) {
    copy_assign(ilist.begin(), ilist.end());
}

/**
 * @brief 在容器起始位置构造元素
 * @param args 构造参数
 */
template <typename T>
template <class... Args>
void list<T>::emplace_front(Args&&... args) {
    if (size_ >= max_size()) {
        throw std::length_error("list<T>::emplace_front - 链表大小超出最大限制");
    }
    auto link_node = create_node(std::forward<Args>(args)...);
    link_nodes_at_front(link_node->as_base(), link_node->as_base());
    ++size_;
}

/**
 * @brief 在容器末尾构造元素
 * @param args 构造参数
 */
template <typename T>
template <class... Args>
void list<T>::emplace_back(Args&&... args) {
    if (size_ >= max_size()) {
        throw std::length_error("list<T>::emplace_back - 链表大小超出最大限制");
    }
    auto link_node = create_node(std::forward<Args>(args)...);
    link_nodes_at_back(link_node->as_base(), link_node->as_base());
    ++size_;
}

/**
 * @brief 在指定位置构造元素
 * @param pos 插入位置
 * @param args 构造参数
 * @return 指向新插入元素的迭代器
 */
template <typename T>
template <class... Args>
typename list<T>::iterator list<T>::emplace(const_iterator pos, Args&&... args) {
    if (size_ >= max_size()) {
        throw std::length_error("list<T>::emplace - 链表大小超出最大限制");
    }
    auto link_node = create_node(std::forward<Args>(args)...);
    link_nodes(pos.node_, link_node->as_base(), link_node->as_base());
    ++size_;
    return iterator(link_node);
}

/**
 * @brief 在指定位置插入一个元素
 * @param pos 插入位置
 * @param value 插入值
 * @return 指向新插入元素的迭代器
 */
template <typename T>
typename list<T>::iterator list<T>::insert(const_iterator pos, const value_type& value) {
    if (size_ >= max_size()) {
        throw std::length_error("list<T>::insert - 链表大小超出最大限制");
    }
    auto link_node = create_node(value);
    ++size_;
    return link_iter_node(pos, link_node->as_base());
}

/**
 * @brief 在指定位置插入一个元素（移动语义）
 * @param pos 插入位置
 * @param value 插入值（右值引用）
 * @return 指向新插入元素的迭代器
 */
template <typename T>
typename list<T>::iterator list<T>::insert(const_iterator pos, value_type&& value) {
    return emplace(pos, std::move(value));
}

/**
 * @brief 在指定位置插入n个相同的元素
 * @param pos 插入位置
 * @param n 插入数量
 * @param value 插入值
 * @return 指向第一个新插入元素的迭代器
 */
template <typename T>
typename list<T>::iterator list<T>::insert(const_iterator pos, size_type n, const value_type& value) {
    if (size_ >= max_size() - n) {
        throw std::length_error("list<T>::insert - 链表大小超出最大限制");
    }
    return fill_insert(pos, n, value);
}

/**
 * @brief 在pos位置用n个相同的值填充插入的辅助函数
 * @param pos 插入位置
 * @param n 插入数量
 * @param value 插入值
 * @return 指向第一个新插入元素的迭代器
 */
template <typename T>
typename list<T>::iterator list<T>::fill_insert(const_iterator pos, size_type n, const value_type& value) {
    iterator r(pos.node_);
    if (n != 0) {
        const auto add_size = n;
        auto node = create_node(value);
        node->prev = nullptr;
        r = iterator(node);
        iterator end = r;
        try {
            // 前面已经创建了一个节点，还需 n - 1 个
            for (--n; n > 0; --n, ++end) {
                auto next = create_node(value);
                end.node_->next = next->as_base();  // 链接节点
                next->prev = end.node_;
            }
            size_ += add_size;
        }
        catch (...) {
            // 处理异常：删除已创建的节点
            auto enode = end.node_;
            while (true) {
                auto prev = enode->prev;
                destroy_node(enode->as_node());
                if (prev == nullptr)
                    break;
                enode = prev;
            }
            throw;
        }
        link_nodes(pos.node_, r.node_, end.node_);
    }
    return r;
}

/**
 * @brief 在指定位置插入迭代器范围内的元素
 * @param pos 插入位置
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @return 指向第一个新插入元素的迭代器
 */
template <typename T>
template <class InputIter, typename>
typename list<T>::iterator list<T>::insert(const_iterator pos, InputIter first, InputIter last) {
    size_type n = std::distance(first, last);
    if (size_ >= max_size() - n) {
        throw std::length_error("list<T>::insert - 链表大小超出最大限制");
    }
    return copy_insert(pos, first, last);
}

/**
 * @brief 在指定位置插入初始化列表中的元素
 * @param pos 插入位置
 * @param ilist 初始化列表
 * @return 指向第一个新插入元素的迭代器
 */
template <typename T>
typename list<T>::iterator list<T>::insert(const_iterator pos, std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
}

/**
 * @brief 在容器起始位置添加一个元素
 * @param value 插入值
 */
template <typename T>
void list<T>::push_front(const value_type& value) {
    if (size_ >= max_size()) {
        throw std::length_error("list<T>::push_front - 链表大小超出最大限制");
    }
    auto link_node = create_node(value);
    link_nodes_at_front(link_node->as_base(), link_node->as_base());
    ++size_;
}

/**
 * @brief 在容器起始位置添加一个元素（移动语义）
 * @param value 插入值（右值引用）
 */
template <typename T>
void list<T>::push_front(value_type&& value) {
    emplace_front(std::move(value));
}

/**
 * @brief 在容器末尾添加一个元素
 * @param value 插入值
 */
template <typename T>
void list<T>::push_back(const value_type& value) {
    if (size_ >= max_size()) {
        throw std::length_error("list<T>::push_back - 链表大小超出最大限制");
    }
    auto link_node = create_node(value);
    link_nodes_at_back(link_node->as_base(), link_node->as_base());
    ++size_;
}

/**
 * @brief 在容器末尾添加一个元素（移动语义）
 * @param value 插入值（右值引用）
 */
template <typename T>
void list<T>::push_back(value_type&& value) {
    emplace_back(std::move(value));
}

/**
 * @brief 移除容器第一个元素
 */
template <typename T>
void list<T>::pop_front() {
    auto n = node_->next;
    unlink_nodes(n, n);
    destroy_node(n->as_node());
    --size_;
}

/**
 * @brief 移除容器最后一个元素
 */
template <typename T>
void list<T>::pop_back() {
    auto n = node_->prev;
    unlink_nodes(n, n);
    destroy_node(n->as_node());
    --size_;
}

/**
 * @brief 移除指定位置的元素
 * @param pos 删除位置
 * @return 指向被删除元素后一个位置的迭代器
 */
template <typename T>
typename list<T>::iterator list<T>::erase(const_iterator pos) {
    auto n = pos.node_;
    auto next = n->next;
    unlink_nodes(n, n);
    destroy_node(n->as_node());
    --size_;
    return iterator(next);
}

/**
 * @brief 移除指定范围的元素
 * @param first 范围起始
 * @param last 范围结束
 * @return 指向最后一个被删除元素后一个位置的迭代器
 */
template <typename T>
typename list<T>::iterator list<T>::erase(const_iterator first, const_iterator last) {
    if (first != last) {
        unlink_nodes(first.node_, last.node_->prev);
        while (first != last) {
            auto cur = first.node_;
            ++first;
            destroy_node(cur->as_node());
            --size_;
        }
    }
    return iterator(last.node_);
}

/**
 * @brief 清空容器
 */
template <typename T>
void list<T>::clear() {
    if (size_ != 0) {
        auto cur = node_->next;
        for (base_ptr next = cur->next; cur != node_; cur = next, next = cur->next) {
            destroy_node(cur->as_node());
        }
        node_->unlink();
        size_ = 0;
    }
}

/**
 * @brief 修改容器大小
 * @param new_size 新容器大小
 */
template <typename T>
void list<T>::resize(size_type new_size) {
    resize(new_size, value_type());
}

/**
 * @brief 修改容器大小，若新大小大于原大小，用指定值填充
 * @param new_size 新容器大小
 * @param value 填充值
 */
template <typename T>
void list<T>::resize(size_type new_size, const value_type& value) {
    auto i = begin();
    size_type len = 0;
    while (i != end() && len < new_size) {
        ++i;
        ++len;
    }
    if (len == new_size) {
        // 如果新大小小于原大小，删除多余元素
        erase(i, node_);
    }
    else {
        // 如果新大小大于原大小，插入新元素
        insert(node_, new_size - len, value);
    }
}

/**
 * @brief 与另一个list交换内容
 * @param rhs 另一个list
 */
template <typename T>
void list<T>::swap(list& rhs) noexcept {
    std::swap(node_, rhs.node_);
    std::swap(size_, rhs.size_);
}

/**
 * @brief 在pos位置复制迭代器范围内的元素的辅助函数
 * @param pos 插入位置
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @return 指向第一个新插入元素的迭代器
 */
template <typename T>
template <class InputIter>
typename list<T>::iterator list<T>::copy_insert(const_iterator pos, InputIter first, InputIter last) {
    iterator r(pos.node_);
    if (first != last) {
        auto node = create_node(*first);
        node->prev = nullptr;
        r = iterator(node);
        iterator end = r;
        try {
            for (++first; first != last; ++first, ++end) {
                auto next = create_node(*first);
                end.node_->next = next->as_base();  // 链接节点
                next->prev = end.node_;
            }
            size_ += std::distance(r, iterator(end.node_->next));
        }
        catch (...) {
            // 处理异常：删除已创建的节点
            auto enode = end.node_;
            while (true) {
                auto prev = enode->prev;
                destroy_node(enode->as_node());
                if (prev == nullptr)
                    break;
                enode = prev;
            }
            throw;
        }
        link_nodes(pos.node_, r.node_, end.node_);
    }
    return r;
}

/**
 * @brief 将一个链表接合到当前链表的pos位置之前
 * @param pos 接合位置
 * @param other 被接合的链表
 */
template <typename T>
void list<T>::splice(const_iterator pos, list& other) {
    if (this != &other && !other.empty()) {
        size_ += other.size_;
        auto first = other.node_->next;
        auto last = other.node_->prev;
        unlink_nodes(first, last);
        link_nodes(pos.node_, first, last);
        other.size_ = 0;
    }
}

/**
 * @brief 将other中的元素it移动到当前链表的pos位置之前
 * @param pos 接合位置
 * @param other 被接合的链表
 * @param it 被移动的元素
 */
template <typename T>
void list<T>::splice(const_iterator pos, list& other, const_iterator it) {
    if (pos.node_ != it.node_ && pos.node_ != std::next(it).node_) {
        auto node = it.node_;
        unlink_nodes(node, node);
        if (this != &other) {
            --other.size_;
            ++size_;
        }
        link_nodes(pos.node_, node, node);
    }
}

/**
 * @brief 将[first, last)范围的元素移动到当前链表的pos位置之前
 * @param pos 接合位置
 * @param other 被接合的链表
 * @param first 范围起始
 * @param last 范围结束
 */
template <typename T>
void list<T>::splice(const_iterator pos, list& other, const_iterator first, const_iterator last) {
    if (first != last && this != &other) {
        size_type n = std::distance(first, last);
        other.size_ -= n;
        size_ += n;
        auto f = first.node_;
        auto l = last.node_->prev;
        unlink_nodes(f, l);
        link_nodes(pos.node_, f, l);
    }
    else if (first != last) {
        // 自我接合
        auto it = pos.node_;
        auto f = first.node_;
        auto l = last.node_->prev;
        if (it != f && it != l->next) {
            unlink_nodes(f, l);
            link_nodes(it, f, l);
        }
    }
}

/**
 * @brief 移除链表中所有与value相等的元素
 * @param value 目标值
 */
template <typename T>
void list<T>::remove(const value_type& value) {
    remove_if([&](const value_type& v) { return v == value; });
}

/**
 * @brief 移除链表中所有满足谓词条件的元素
 * @param pred 一元谓词
 */
template <typename T>
template <class UnaryPredicate>
void list<T>::remove_if(UnaryPredicate pred) {
    auto first = begin();
    auto last = end();
    for (auto next = first; first != last; first = next) {
        ++next;
        if (pred(*first)) {
            erase(first);
        }
    }
}

/**
 * @brief 删除链表中连续重复的元素，只保留一个
 */
template <typename T>
void list<T>::unique() {
    unique(std::equal_to<T>());
}

/**
 * @brief 删除链表中按照谓词判断为重复的元素
 * @param binary_pred 二元谓词
 */
template <typename T>
template <class BinaryPredicate>
void list<T>::unique(BinaryPredicate binary_pred) {
    if (size_ <= 1) return;
    auto first = begin();
    auto last = end();
    auto next = first;
    ++next;
    while (next != last) {
        if (binary_pred(*first, *next)) {
            erase(next);
            next = first;
            ++next;
        }
        else {
            first = next;
            ++next;
        }
    }
}

/**
 * @brief 将链表按升序排序（使用operator<）
 */
template <typename T>
void list<T>::sort() {
    sort(std::less<T>());
}

/**
 * @brief 将链表按照指定的比较函数排序（归并排序）
 * @param compare 比较函数
 */
template <typename T>
template <class Compare>
void list<T>::sort(Compare compare) {
    if (size_ <= 1) return;
    
    // 使用归并排序
    list carry;
    list counter[64];  // 存储2^i长度的有序链表
    int fill = 0;      // 当前使用的计数器索引
    
    while (!empty()) {
        // 取出链表第一个元素
        carry.splice(carry.begin(), *this, begin());
        
        int i = 0;
        while (i < fill && !counter[i].empty()) {
            // 合并相同长度的两个有序链表
            counter[i].merge(carry, compare);
            carry.swap(counter[i++]);
        }
        
        // 将结果放入下一个计数器
        carry.swap(counter[i]);
        if (i == fill) ++fill;
    }
    
    // 合并所有计数器中的链表
    for (int i = 1; i < fill; ++i) {
        counter[i].merge(counter[i-1], compare);
    }
    
    swap(counter[fill-1]);
}

/**
 * @brief 合并两个有序链表
 * @param x 另一个链表
 */
template <typename T>
void list<T>::merge(list& x) {
    merge(x, std::less<T>());
}

/**
 * @brief 按指定比较函数合并两个有序链表
 * @param x 另一个链表
 * @param comp 比较函数
 */
template <typename T>
template <class Compare>
void list<T>::merge(list& x, Compare comp) {
    if (this != &x) {
        auto first1 = begin();
        auto last1 = end();
        auto first2 = x.begin();
        auto last2 = x.end();
        
        while (first1 != last1 && first2 != last2) {
            if (comp(*first2, *first1)) {
                // 将x中的元素移动到当前链表
                auto next = first2;
                ++next;
                splice(first1, x, first2);
                first2 = next;
            }
            else {
                ++first1;
            }
        }
        
        // 如果x中还有剩余元素，全部移动到当前链表末尾
        if (first2 != last2) {
            splice(last1, x, first2, last2);
        }
    }
}

/**
 * @brief 反转链表
 */
template <typename T>
void list<T>::reverse() noexcept {
    if (size_ <= 1) return;
    
    // 从头到尾逐一交换节点的前后指针
    auto cur = node_->next;
    node_->next = node_->prev;
    node_->prev = cur;
    
    while (cur != node_) {
        auto temp = cur->next;
        cur->next = cur->prev;
        cur->prev = temp;
        cur = temp;
    }
}

/**
 * @brief 重载==操作符
 */
template <typename T>
bool operator==(const list<T>& lhs, const list<T>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

/**
 * @brief 重载!=操作符
 */
template <typename T>
bool operator!=(const list<T>& lhs, const list<T>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief 重载<操作符
 */
template <typename T>
bool operator<(const list<T>& lhs, const list<T>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/**
 * @brief 重载>操作符
 */
template <typename T>
bool operator>(const list<T>& lhs, const list<T>& rhs) {
    return rhs < lhs;
}

/**
 * @brief 重载<=操作符
 */
template <typename T>
bool operator<=(const list<T>& lhs, const list<T>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief 重载>=操作符
 */
template <typename T>
bool operator>=(const list<T>& lhs, const list<T>& rhs) {
    return !(lhs < rhs);
}

/**
 * @brief 重载标准库swap函数
 */
template <typename T>
void swap(list<T>& lhs, list<T>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace mystl

#endif // MY_LIST_H 