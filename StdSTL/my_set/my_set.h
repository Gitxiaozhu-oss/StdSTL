#ifndef MY_SET_H_
#define MY_SET_H_

// 这个头文件包含两个模板类 set 和 multiset
// set      : 集合，键值即实值，集合内元素会自动排序，键值不允许重复
// multiset : 集合，键值即实值，集合内元素会自动排序，键值允许重复

#include "../my_rb_tree/my_rb_tree.h"
#include <initializer_list>
#include <functional>

namespace mystl
{

/**
 * @class set
 * @brief 模板类 set，基于红黑树实现的键值不允许重复的集合
 * @tparam Key 键值类型
 * @tparam Compare 键值比较方式，默认使用 std::less
 */
template <class Key, class Compare = std::less<Key>>
class set
{
public:
    // 类型定义
    typedef Key        key_type;      // 键值类型
    typedef Key        value_type;    // 实值类型，键值即实值
    typedef Compare    key_compare;   // 键值比较函数
    typedef Compare    value_compare; // 实值比较函数

private:
    // 使用红黑树作为底层实现机制
    typedef mystl::rb_tree<value_type, key_compare> base_type;
    base_type tree_; // 红黑树成员

public:
    // 使用 rb_tree 定义的类型
    typedef typename base_type::node_type              node_type;
    typedef typename base_type::const_pointer          pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::const_reference        reference;
    typedef typename base_type::const_reference        const_reference;
    typedef typename base_type::const_iterator         iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::const_reverse_iterator reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::difference_type        difference_type;
    typedef typename base_type::allocator_type         allocator_type;

public:
    /**
     * @brief 默认构造函数
     */
    set() = default;

    /**
     * @brief 迭代器范围构造函数
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <class InputIterator>
    set(InputIterator first, InputIterator last)
        :tree_()
    { tree_.insert_unique(first, last); }

    /**
     * @brief 初始化列表构造函数
     * @param ilist 初始化列表
     */
    set(std::initializer_list<value_type> ilist)
        :tree_()
    { tree_.insert_unique(ilist.begin(), ilist.end()); }

    /**
     * @brief 拷贝构造函数
     * @param rhs 源对象
     */
    set(const set& rhs)
        :tree_(rhs.tree_)
    { }

    /**
     * @brief 移动构造函数
     * @param rhs 源对象
     */
    set(set&& rhs) noexcept
        :tree_(std::move(rhs.tree_))
    { }

    /**
     * @brief 拷贝赋值运算符
     * @param rhs 源对象
     * @return 自引用
     */
    set& operator=(const set& rhs)
    {
        tree_ = rhs.tree_;
        return *this;
    }

    /**
     * @brief 移动赋值运算符
     * @param rhs 源对象
     * @return 自引用
     */
    set& operator=(set&& rhs) noexcept
    {
        tree_ = std::move(rhs.tree_);
        return *this;
    }

    /**
     * @brief 初始化列表赋值运算符
     * @param ilist 初始化列表
     * @return 自引用
     */
    set& operator=(std::initializer_list<value_type> ilist)
    {
        tree_.clear();
        tree_.insert_unique(ilist.begin(), ilist.end());
        return *this;
    }

    // 访问器相关接口

    /**
     * @brief 获取键值比较函数对象
     * @return 键值比较函数
     */
    key_compare key_comp() const { return tree_.key_comp(); }

    /**
     * @brief 获取值比较函数对象
     * @return 值比较函数
     */
    value_compare value_comp() const { return tree_.key_comp(); }

    /**
     * @brief 获取分配器
     * @return 分配器
     */
    allocator_type get_allocator() const { return tree_.get_allocator(); }

    // 迭代器相关

    /**
     * @brief 返回指向容器第一个元素的迭代器
     * @return 迭代器
     */
    iterator begin() noexcept
    { return tree_.begin(); }

    /**
     * @brief 返回指向容器第一个元素的常量迭代器
     * @return 常量迭代器
     */
    const_iterator begin() const noexcept
    { return tree_.begin(); }

    /**
     * @brief 返回指向容器尾部的迭代器
     * @return 迭代器
     */
    iterator end() noexcept
    { return tree_.end(); }

    /**
     * @brief 返回指向容器尾部的常量迭代器
     * @return 常量迭代器
     */
    const_iterator end() const noexcept
    { return tree_.end(); }

    /**
     * @brief 返回反向迭代器，指向容器最后一个元素
     * @return 反向迭代器
     */
    reverse_iterator rbegin() noexcept
    { return reverse_iterator(end()); }

    /**
     * @brief 返回常量反向迭代器，指向容器最后一个元素
     * @return 常量反向迭代器
     */
    const_reverse_iterator rbegin() const noexcept
    { return const_reverse_iterator(end()); }

    /**
     * @brief 返回反向迭代器，指向容器第一个元素之前的位置
     * @return 反向迭代器
     */
    reverse_iterator rend() noexcept
    { return reverse_iterator(begin()); }

    /**
     * @brief 返回常量反向迭代器，指向容器第一个元素之前的位置
     * @return 常量反向迭代器
     */
    const_reverse_iterator rend() const noexcept
    { return const_reverse_iterator(begin()); }

    /**
     * @brief 返回常量迭代器，指向容器第一个元素
     * @return 常量迭代器
     */
    const_iterator cbegin() const noexcept
    { return begin(); }

    /**
     * @brief 返回常量迭代器，指向容器尾部
     * @return 常量迭代器
     */
    const_iterator cend() const noexcept
    { return end(); }

    /**
     * @brief 返回常量反向迭代器，指向容器最后一个元素
     * @return 常量反向迭代器
     */
    const_reverse_iterator crbegin() const noexcept
    { return rbegin(); }

    /**
     * @brief 返回常量反向迭代器，指向容器第一个元素之前的位置
     * @return 常量反向迭代器
     */
    const_reverse_iterator crend() const noexcept
    { return rend(); }

    // 容量相关接口

    /**
     * @brief 检查容器是否为空
     * @return 如果容器为空返回true，否则返回false
     */
    bool empty() const noexcept { return tree_.empty(); }

    /**
     * @brief 返回容器中的元素数量
     * @return 元素数量
     */
    size_type size() const noexcept { return tree_.size(); }

    /**
     * @brief 返回容器可容纳的最大元素数量
     * @return 最大元素数量
     */
    size_type max_size() const noexcept { return tree_.max_size(); }

    // 修改容器操作

    /**
     * @brief 就地构造元素
     * @tparam Args 参数类型包
     * @param args 构造参数
     * @return 包含插入位置迭代器和成功标志的pair
     */
    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        return tree_.emplace_unique(std::forward<Args>(args)...);
    }

    /**
     * @brief 在指定位置附近就地构造元素
     * @tparam Args 参数类型包
     * @param hint 指定位置
     * @param args 构造参数
     * @return 插入位置迭代器
     */
    template <class... Args>
    iterator emplace_hint(iterator hint, Args&&... args)
    {
        return tree_.emplace_unique_use_hint(hint, std::forward<Args>(args)...);
    }

    /**
     * @brief 插入元素
     * @param value 要插入的值
     * @return 包含插入位置迭代器和成功标志的pair
     */
    std::pair<iterator, bool> insert(const value_type& value)
    {
        return tree_.insert_unique(value);
    }

    /**
     * @brief 插入右值元素
     * @param value 要插入的右值
     * @return 包含插入位置迭代器和成功标志的pair
     */
    std::pair<iterator, bool> insert(value_type&& value)
    {
        return tree_.insert_unique(std::move(value));
    }

    /**
     * @brief 在指定位置附近插入元素
     * @param hint 指定位置
     * @param value 要插入的值
     * @return 插入位置迭代器
     */
    iterator insert(iterator hint, const value_type& value)
    {
        return tree_.insert_unique(hint, value);
    }

    /**
     * @brief 在指定位置附近插入右值元素
     * @param hint 指定位置
     * @param value 要插入的右值
     * @return 插入位置迭代器
     */
    iterator insert(iterator hint, value_type&& value)
    {
        return tree_.insert_unique(hint, std::move(value));
    }

    /**
     * @brief 插入一个范围的元素
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围开始
     * @param last 范围结束
     */
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        tree_.insert_unique(first, last);
    }

    /**
     * @brief 删除指定位置的元素
     * @param position 要删除的位置
     */
    void erase(iterator position)
    {
        tree_.erase(position);
    }

    /**
     * @brief 删除指定键的元素
     * @param key 要删除的键
     * @return 删除的元素数量
     */
    size_type erase(const key_type& key)
    {
        return tree_.erase_unique(key);
    }

    /**
     * @brief 删除一个范围的元素
     * @param first 范围开始
     * @param last 范围结束
     */
    void erase(iterator first, iterator last)
    {
        tree_.erase(first, last);
    }

    /**
     * @brief 清空容器
     */
    void clear()
    {
        tree_.clear();
    }

    // 查找操作

    /**
     * @brief 查找具有特定键的元素
     * @param key 要查找的键
     * @return 指向找到元素的迭代器，如果未找到则返回end()
     */
    iterator find(const key_type& key)
    {
        return tree_.find(key);
    }

    /**
     * @brief 查找具有特定键的元素（常量版本）
     * @param key 要查找的键
     * @return 指向找到元素的常量迭代器，如果未找到则返回end()
     */
    const_iterator find(const key_type& key) const
    {
        return tree_.find(key);
    }

    /**
     * @brief 计算特定键的元素数量
     * @param key 要计数的键
     * @return 元素数量（对于set，要么是0要么是1）
     */
    size_type count(const key_type& key) const
    {
        return tree_.count_unique(key);
    }

    /**
     * @brief 找到大于等于指定键的第一个位置
     * @param key 键值
     * @return 迭代器
     */
    iterator lower_bound(const key_type& key)
    {
        return tree_.lower_bound(key);
    }

    /**
     * @brief 找到大于等于指定键的第一个位置（常量版本）
     * @param key 键值
     * @return 常量迭代器
     */
    const_iterator lower_bound(const key_type& key) const
    {
        return tree_.lower_bound(key);
    }

    /**
     * @brief 找到大于指定键的第一个位置
     * @param key 键值
     * @return 迭代器
     */
    iterator upper_bound(const key_type& key)
    {
        return tree_.upper_bound(key);
    }

    /**
     * @brief 找到大于指定键的第一个位置（常量版本）
     * @param key 键值
     * @return 常量迭代器
     */
    const_iterator upper_bound(const key_type& key) const
    {
        return tree_.upper_bound(key);
    }

    /**
     * @brief 返回等于特定键的元素范围
     * @param key 键值
     * @return 迭代器对，表示范围
     */
    std::pair<iterator, iterator> equal_range(const key_type& key)
    {
        return tree_.equal_range_unique(key);
    }

    /**
     * @brief 返回等于特定键的元素范围（常量版本）
     * @param key 键值
     * @return 常量迭代器对，表示范围
     */
    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    {
        return tree_.equal_range_unique(key);
    }

    /**
     * @brief 交换两个set的内容
     * @param rhs 要交换的set
     */
    void swap(set& rhs) noexcept
    {
        tree_.swap(rhs.tree_);
    }

public:
    /**
     * @brief 相等比较运算符
     */
    friend bool operator==(const set& lhs, const set& rhs) { return lhs.tree_ == rhs.tree_; }
    
    /**
     * @brief 小于比较运算符
     */
    friend bool operator<(const set& lhs, const set& rhs) { return lhs.tree_ < rhs.tree_; }
};

// 重载比较操作符
/**
 * @brief 相等比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator==(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    return lhs == rhs;
}

/**
 * @brief 小于比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator<(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    return lhs < rhs;
}

/**
 * @brief 不等比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator!=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    return !(lhs == rhs);
}

/**
 * @brief 大于比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator>(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    return rhs < lhs;
}

/**
 * @brief 小于等于比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator<=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    return !(rhs < lhs);
}

/**
 * @brief 大于等于比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator>=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    return !(lhs < rhs);
}

/**
 * @brief 交换两个set
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
void swap(set<Key, Compare>& lhs, set<Key, Compare>& rhs) noexcept
{
    lhs.swap(rhs);
}

/*****************************************************************************************/

/**
 * @class multiset
 * @brief 模板类 multiset，基于红黑树实现的键值允许重复的集合
 * @tparam Key 键值类型
 * @tparam Compare 键值比较方式，默认使用 std::less
 */
template <class Key, class Compare = std::less<Key>>
class multiset
{
public:
    // 类型定义
    typedef Key        key_type;      // 键值类型
    typedef Key        value_type;    // 实值类型，键值即实值 
    typedef Compare    key_compare;   // 键值比较函数
    typedef Compare    value_compare; // 实值比较函数

private:
    // 使用红黑树作为底层实现机制
    typedef mystl::rb_tree<value_type, key_compare> base_type;
    base_type tree_; // 红黑树成员

public:
    // 使用 rb_tree 定义的类型
    typedef typename base_type::node_type              node_type;
    typedef typename base_type::const_pointer          pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::const_reference        reference;
    typedef typename base_type::const_reference        const_reference;
    typedef typename base_type::const_iterator         iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::const_reverse_iterator reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::difference_type        difference_type;
    typedef typename base_type::allocator_type         allocator_type;

public:
    /**
     * @brief 默认构造函数
     */
    multiset() = default;

    /**
     * @brief 迭代器范围构造函数
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <class InputIterator>
    multiset(InputIterator first, InputIterator last)
        :tree_()
    { tree_.insert_multi(first, last); }

    /**
     * @brief 初始化列表构造函数
     * @param ilist 初始化列表
     */
    multiset(std::initializer_list<value_type> ilist)
        :tree_()
    { tree_.insert_multi(ilist.begin(), ilist.end()); }

    /**
     * @brief 拷贝构造函数
     * @param rhs 源对象
     */
    multiset(const multiset& rhs)
        :tree_(rhs.tree_)
    { }

    /**
     * @brief 移动构造函数
     * @param rhs 源对象
     */
    multiset(multiset&& rhs) noexcept
        :tree_(std::move(rhs.tree_))
    { }

    /**
     * @brief 拷贝赋值运算符
     * @param rhs 源对象
     * @return 自引用
     */
    multiset& operator=(const multiset& rhs)
    {
        tree_ = rhs.tree_;
        return *this;
    }

    /**
     * @brief 移动赋值运算符
     * @param rhs 源对象
     * @return 自引用
     */
    multiset& operator=(multiset&& rhs) noexcept
    {
        tree_ = std::move(rhs.tree_);
        return *this;
    }

    /**
     * @brief 初始化列表赋值运算符
     * @param ilist 初始化列表
     * @return 自引用
     */
    multiset& operator=(std::initializer_list<value_type> ilist)
    {
        tree_.clear();
        tree_.insert_multi(ilist.begin(), ilist.end());
        return *this;
    }

    // 访问器相关接口

    /**
     * @brief 获取键值比较函数对象
     * @return 键值比较函数
     */
    key_compare key_comp() const { return tree_.key_comp(); }

    /**
     * @brief 获取值比较函数对象
     * @return 值比较函数
     */
    value_compare value_comp() const { return tree_.key_comp(); }

    /**
     * @brief 获取分配器
     * @return 分配器
     */
    allocator_type get_allocator() const { return tree_.get_allocator(); }

    // 迭代器相关

    /**
     * @brief 返回指向容器第一个元素的迭代器
     * @return 迭代器
     */
    iterator begin() noexcept
    { return tree_.begin(); }

    /**
     * @brief 返回指向容器第一个元素的常量迭代器
     * @return 常量迭代器
     */
    const_iterator begin() const noexcept
    { return tree_.begin(); }

    /**
     * @brief 返回指向容器尾部的迭代器
     * @return 迭代器
     */
    iterator end() noexcept
    { return tree_.end(); }

    /**
     * @brief 返回指向容器尾部的常量迭代器
     * @return 常量迭代器
     */
    const_iterator end() const noexcept
    { return tree_.end(); }

    /**
     * @brief 返回反向迭代器，指向容器最后一个元素
     * @return 反向迭代器
     */
    reverse_iterator rbegin() noexcept
    { return reverse_iterator(end()); }

    /**
     * @brief 返回常量反向迭代器，指向容器最后一个元素
     * @return 常量反向迭代器
     */
    const_reverse_iterator rbegin() const noexcept
    { return const_reverse_iterator(end()); }

    /**
     * @brief 返回反向迭代器，指向容器第一个元素之前的位置
     * @return 反向迭代器
     */
    reverse_iterator rend() noexcept
    { return reverse_iterator(begin()); }

    /**
     * @brief 返回常量反向迭代器，指向容器第一个元素之前的位置
     * @return 常量反向迭代器
     */
    const_reverse_iterator rend() const noexcept
    { return const_reverse_iterator(begin()); }

    /**
     * @brief 返回常量迭代器，指向容器第一个元素
     * @return 常量迭代器
     */
    const_iterator cbegin() const noexcept
    { return begin(); }

    /**
     * @brief 返回常量迭代器，指向容器尾部
     * @return 常量迭代器
     */
    const_iterator cend() const noexcept
    { return end(); }

    /**
     * @brief 返回常量反向迭代器，指向容器最后一个元素
     * @return 常量反向迭代器
     */
    const_reverse_iterator crbegin() const noexcept
    { return rbegin(); }

    /**
     * @brief 返回常量反向迭代器，指向容器第一个元素之前的位置
     * @return 常量反向迭代器
     */
    const_reverse_iterator crend() const noexcept
    { return rend(); }

    // 容量相关接口

    /**
     * @brief 检查容器是否为空
     * @return 如果容器为空返回true，否则返回false
     */
    bool empty() const noexcept { return tree_.empty(); }

    /**
     * @brief 返回容器中的元素数量
     * @return 元素数量
     */
    size_type size() const noexcept { return tree_.size(); }

    /**
     * @brief 返回容器可容纳的最大元素数量
     * @return 最大元素数量
     */
    size_type max_size() const noexcept { return tree_.max_size(); }

    // 修改容器操作

    /**
     * @brief 就地构造元素
     * @tparam Args 参数类型包
     * @param args 构造参数
     * @return 插入位置迭代器
     */
    template <class... Args>
    iterator emplace(Args&&... args)
    {
        return tree_.emplace_multi(std::forward<Args>(args)...);
    }

    /**
     * @brief 在指定位置附近就地构造元素
     * @tparam Args 参数类型包
     * @param hint 指定位置
     * @param args 构造参数
     * @return 插入位置迭代器
     */
    template <class... Args>
    iterator emplace_hint(iterator hint, Args&&... args)
    {
        return tree_.emplace_multi_use_hint(hint, std::forward<Args>(args)...);
    }

    /**
     * @brief 插入元素
     * @param value 要插入的值
     * @return 插入位置迭代器
     */
    iterator insert(const value_type& value)
    {
        return tree_.insert_multi(value);
    }

    /**
     * @brief 插入右值元素
     * @param value 要插入的右值
     * @return 插入位置迭代器
     */
    iterator insert(value_type&& value)
    {
        return tree_.insert_multi(std::move(value));
    }

    /**
     * @brief 在指定位置附近插入元素
     * @param hint 指定位置
     * @param value 要插入的值
     * @return 插入位置迭代器
     */
    iterator insert(iterator hint, const value_type& value)
    {
        return tree_.insert_multi(hint, value);
    }

    /**
     * @brief 在指定位置附近插入右值元素
     * @param hint 指定位置
     * @param value 要插入的右值
     * @return 插入位置迭代器
     */
    iterator insert(iterator hint, value_type&& value)
    {
        return tree_.insert_multi(hint, std::move(value));
    }

    /**
     * @brief 插入一个范围的元素
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围开始
     * @param last 范围结束
     */
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        tree_.insert_multi(first, last);
    }

    /**
     * @brief 删除指定位置的元素
     * @param position 要删除的位置
     */
    void erase(iterator position)
    {
        tree_.erase(position);
    }

    /**
     * @brief 删除指定键的元素
     * @param key 要删除的键
     * @return 删除的元素数量
     */
    size_type erase(const key_type& key)
    {
        return tree_.erase_multi(key);
    }

    /**
     * @brief 删除一个范围的元素
     * @param first 范围开始
     * @param last 范围结束
     */
    void erase(iterator first, iterator last)
    {
        tree_.erase(first, last);
    }

    /**
     * @brief 清空容器
     */
    void clear()
    {
        tree_.clear();
    }

    // 查找操作

    /**
     * @brief 查找具有特定键的元素
     * @param key 要查找的键
     * @return 指向找到元素的迭代器，如果未找到则返回end()
     */
    iterator find(const key_type& key)
    {
        return tree_.find(key);
    }

    /**
     * @brief 查找具有特定键的元素（常量版本）
     * @param key 要查找的键
     * @return 指向找到元素的常量迭代器，如果未找到则返回end()
     */
    const_iterator find(const key_type& key) const
    {
        return tree_.find(key);
    }

    /**
     * @brief 计算特定键的元素数量
     * @param key 要计数的键
     * @return 元素数量
     */
    size_type count(const key_type& key) const
    {
        return tree_.count_multi(key);
    }

    /**
     * @brief 找到大于等于指定键的第一个位置
     * @param key 键值
     * @return 迭代器
     */
    iterator lower_bound(const key_type& key)
    {
        return tree_.lower_bound(key);
    }

    /**
     * @brief 找到大于等于指定键的第一个位置（常量版本）
     * @param key 键值
     * @return 常量迭代器
     */
    const_iterator lower_bound(const key_type& key) const
    {
        return tree_.lower_bound(key);
    }

    /**
     * @brief 找到大于指定键的第一个位置
     * @param key 键值
     * @return 迭代器
     */
    iterator upper_bound(const key_type& key)
    {
        return tree_.upper_bound(key);
    }

    /**
     * @brief 找到大于指定键的第一个位置（常量版本）
     * @param key 键值
     * @return 常量迭代器
     */
    const_iterator upper_bound(const key_type& key) const
    {
        return tree_.upper_bound(key);
    }

    /**
     * @brief 返回等于特定键的元素范围
     * @param key 键值
     * @return 迭代器对，表示范围
     */
    std::pair<iterator, iterator> equal_range(const key_type& key)
    {
        return tree_.equal_range_multi(key);
    }

    /**
     * @brief 返回等于特定键的元素范围（常量版本）
     * @param key 键值
     * @return 常量迭代器对，表示范围
     */
    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    {
        return tree_.equal_range_multi(key);
    }

    /**
     * @brief 交换两个multiset的内容
     * @param rhs 要交换的multiset
     */
    void swap(multiset& rhs) noexcept
    {
        tree_.swap(rhs.tree_);
    }

public:
    /**
     * @brief 相等比较运算符
     */
    friend bool operator==(const multiset& lhs, const multiset& rhs) { return lhs.tree_ == rhs.tree_; }
    
    /**
     * @brief 小于比较运算符
     */
    friend bool operator<(const multiset& lhs, const multiset& rhs) { return lhs.tree_ < rhs.tree_; }
};

// 重载比较操作符
/**
 * @brief 相等比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator==(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
    return lhs == rhs;
}

/**
 * @brief 小于比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator<(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
    return lhs < rhs;
}

/**
 * @brief 不等比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator!=(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
    return !(lhs == rhs);
}

/**
 * @brief 大于比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator>(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
    return rhs < lhs;
}

/**
 * @brief 小于等于比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator<=(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
    return !(rhs < lhs);
}

/**
 * @brief 大于等于比较运算符
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
bool operator>=(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
    return !(lhs < rhs);
}

/**
 * @brief 交换两个multiset
 * @tparam Key 键类型
 * @tparam Compare 比较函数类型
 */
template <class Key, class Compare>
void swap(multiset<Key, Compare>& lhs, multiset<Key, Compare>& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace mystl
#endif // MY_SET_H_ 