#ifndef MY_UNORDERED_MAP_H
#define MY_UNORDERED_MAP_H

// 这个头文件包含两个模板类 unordered_map 和 unordered_multimap
// 功能与用法与 map 和 multimap 类似，不同的是使用 hashtable 作为底层实现机制，容器内的元素不会自动排序

// 异常保证：
// unordered_map<Key, T> / unordered_multimap<Key, T> 满足基本异常保证，对以下函数做强异常安全保证：
//   * emplace
//   * emplace_hint
//   * insert

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <utility>
#include <stdexcept>
#include "../my_hashtable/my_hashtable.h"

namespace mystl
{

// 前置声明
template <class Key, class T, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
class unordered_map;

template <class Key, class T, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
class unordered_multimap;

// 声明比较操作符
template <class Key, class T, class Hash, class KeyEqual>
bool operator==(const unordered_map<Key, T, Hash, KeyEqual>& lhs,
                const unordered_map<Key, T, Hash, KeyEqual>& rhs);

template <class Key, class T, class Hash, class KeyEqual>
bool operator!=(const unordered_map<Key, T, Hash, KeyEqual>& lhs,
                const unordered_map<Key, T, Hash, KeyEqual>& rhs);

template <class Key, class T, class Hash, class KeyEqual>
bool operator==(const unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
                const unordered_multimap<Key, T, Hash, KeyEqual>& rhs);

template <class Key, class T, class Hash, class KeyEqual>
bool operator!=(const unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
                const unordered_multimap<Key, T, Hash, KeyEqual>& rhs);

// 声明 swap 函数
template <class Key, class T, class Hash, class KeyEqual>
void swap(unordered_map<Key, T, Hash, KeyEqual>& lhs,
          unordered_map<Key, T, Hash, KeyEqual>& rhs) noexcept;

template <class Key, class T, class Hash, class KeyEqual>
void swap(unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
          unordered_multimap<Key, T, Hash, KeyEqual>& rhs) noexcept;

/**
 * @class unordered_map
 * @brief 实现基于哈希表的键值对容器，键值不允许重复
 * 
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Hash 哈希函数类型，默认使用 std::hash
 * @tparam KeyEqual 键比较函数类型，默认使用 std::equal_to
 */
template <class Key, class T, class Hash, class KeyEqual>
class unordered_map
{
private:
    // 使用 hashtable 作为底层实现
    typedef hashtable<std::pair<const Key, T>, Hash, KeyEqual> base_type;
    base_type ht_;

public:
    // 使用 hashtable 的类型定义
    typedef typename base_type::allocator_type       allocator_type;
    typedef typename base_type::key_type             key_type;
    typedef typename base_type::mapped_type          mapped_type;
    typedef typename base_type::value_type           value_type;
    typedef typename base_type::hasher               hasher;
    typedef typename base_type::key_equal            key_equal;

    typedef typename base_type::size_type            size_type;
    typedef typename base_type::difference_type      difference_type;
    typedef typename base_type::pointer              pointer;
    typedef typename base_type::const_pointer        const_pointer;
    typedef typename base_type::reference            reference;
    typedef typename base_type::const_reference      const_reference;

    typedef typename base_type::iterator             iterator;
    typedef typename base_type::const_iterator       const_iterator;
    typedef typename base_type::local_iterator       local_iterator;
    typedef typename base_type::const_local_iterator const_local_iterator;

    /**
     * @brief 获取分配器
     * @return 返回容器使用的分配器
     */
    allocator_type get_allocator() const { return ht_.get_allocator(); }

public:
    // 构造、复制、移动、析构函数
    
    /**
     * @brief 默认构造函数
     */
    unordered_map()
        : ht_(100, Hash(), KeyEqual())
    {
    }

    /**
     * @brief 构造函数，指定桶数和哈希函数
     * 
     * @param bucket_count 指定的桶数
     * @param hash 哈希函数
     * @param equal 键比较函数
     */
    explicit unordered_map(size_type bucket_count,
                           const Hash& hash = Hash(),
                           const KeyEqual& equal = KeyEqual())
        : ht_(bucket_count, hash, equal)
    {
    }

    /**
     * @brief 范围构造函数
     * 
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     * @param bucket_count 桶数量
     * @param hash 哈希函数
     * @param equal 键比较函数
     */
    template <class InputIterator>
    unordered_map(InputIterator first, InputIterator last,
                  const size_type bucket_count = 100,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual())
        : ht_(std::max(bucket_count, static_cast<size_type>(std::distance(first, last))), hash, equal)
    {
        for (; first != last; ++first)
            ht_.insert_unique_noresize(*first);
    }

    /**
     * @brief 初始化列表构造函数
     * 
     * @param ilist 初始化列表
     * @param bucket_count 桶数量
     * @param hash 哈希函数
     * @param equal 键比较函数
     */
    unordered_map(std::initializer_list<value_type> ilist,
                  const size_type bucket_count = 100,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual())
        : ht_(std::max(bucket_count, static_cast<size_type>(ilist.size())), hash, equal)
    {
        for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
            ht_.insert_unique_noresize(*first);
    }

    /**
     * @brief 拷贝构造函数
     * 
     * @param rhs 源对象
     */
    unordered_map(const unordered_map& rhs)
        : ht_(rhs.ht_)
    {
    }

    /**
     * @brief 移动构造函数
     * 
     * @param rhs 源对象
     */
    unordered_map(unordered_map&& rhs) noexcept
        : ht_(std::move(rhs.ht_))
    {
    }

    /**
     * @brief 拷贝赋值运算符
     * 
     * @param rhs 源对象
     * @return unordered_map& 返回自身引用
     */
    unordered_map& operator=(const unordered_map& rhs)
    {
        ht_ = rhs.ht_;
        return *this;
    }

    /**
     * @brief 移动赋值运算符
     * 
     * @param rhs 源对象
     * @return unordered_map& 返回自身引用
     */
    unordered_map& operator=(unordered_map&& rhs) noexcept
    {
        ht_ = std::move(rhs.ht_);
        return *this;
    }

    /**
     * @brief 初始化列表赋值运算符
     * 
     * @param ilist 初始化列表
     * @return unordered_map& 返回自身引用
     */
    unordered_map& operator=(std::initializer_list<value_type> ilist)
    {
        ht_.clear();
        ht_.reserve(ilist.size());
        for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
            ht_.insert_unique_noresize(*first);
        return *this;
    }

    /**
     * @brief 析构函数
     */
    ~unordered_map() = default;

    // 迭代器相关

    /**
     * @brief 返回指向容器第一个元素的迭代器
     * @return 迭代器
     */
    iterator begin() noexcept
    { return ht_.begin(); }

    /**
     * @brief 返回指向容器第一个元素的常量迭代器
     * @return 常量迭代器
     */
    const_iterator begin() const noexcept
    { return ht_.begin(); }

    /**
     * @brief 返回指向容器尾部的迭代器
     * @return 迭代器
     */
    iterator end() noexcept
    { return ht_.end(); }

    /**
     * @brief 返回指向容器尾部的常量迭代器
     * @return 常量迭代器
     */
    const_iterator end() const noexcept
    { return ht_.end(); }

    /**
     * @brief 返回指向容器第一个元素的常量迭代器
     * @return 常量迭代器
     */
    const_iterator cbegin() const noexcept
    { return ht_.cbegin(); }

    /**
     * @brief 返回指向容器尾部的常量迭代器
     * @return 常量迭代器
     */
    const_iterator cend() const noexcept
    { return ht_.cend(); }

    // 容量相关

    /**
     * @brief 判断容器是否为空
     * @return 如果容器为空，返回true；否则，返回false
     */
    bool empty() const noexcept { return ht_.empty(); }

    /**
     * @brief 返回容器中元素的数量
     * @return 元素数量
     */
    size_type size() const noexcept { return ht_.size(); }

    /**
     * @brief 返回容器能够容纳的最大元素数量
     * @return 最大元素数量
     */
    size_type max_size() const noexcept { return ht_.max_size(); }

    // 修改容器操作

    // emplace / emplace_hint

    /**
     * @brief 原位构造元素
     * 
     * @tparam Args 参数类型包
     * @param args 构造参数
     * @return std::pair<iterator, bool> 插入结果，包含指向元素的迭代器和是否成功插入的标志
     */
    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    { return ht_.emplace_unique(std::forward<Args>(args)...); }

    /**
     * @brief 使用提示的原位构造元素
     * 
     * @tparam Args 参数类型包
     * @param hint 提示位置
     * @param args 构造参数
     * @return iterator 指向新元素的迭代器
     */
    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    { return ht_.emplace_unique_use_hint(hint, std::forward<Args>(args)...); }

    // insert

    /**
     * @brief 插入元素
     * 
     * @param value 要插入的值
     * @return std::pair<iterator, bool> 插入结果，包含指向元素的迭代器和是否成功插入的标志
     */
    std::pair<iterator, bool> insert(const value_type& value)
    { return ht_.insert_unique(value); }

    /**
     * @brief 插入元素(移动版本)
     * 
     * @param value 要插入的值
     * @return std::pair<iterator, bool> 插入结果，包含指向元素的迭代器和是否成功插入的标志
     */
    std::pair<iterator, bool> insert(value_type&& value)
    { return ht_.emplace_unique(std::move(value)); }

    /**
     * @brief 使用提示位置插入元素
     * 
     * @param hint 提示位置
     * @param value 要插入的值
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(const_iterator hint, const value_type& value)
    { return ht_.insert_unique_use_hint(hint, value); }

    /**
     * @brief 使用提示位置插入元素(移动版本)
     * 
     * @param hint 提示位置
     * @param value 要插入的值
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(const_iterator hint, value_type&& value)
    { return ht_.emplace_unique_use_hint(hint, std::move(value)); }

    /**
     * @brief 插入范围内的元素
     * 
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    { ht_.insert_unique(first, last); }

    // erase / clear

    /**
     * @brief 删除指定位置的元素
     * 
     * @param it 要删除元素的迭代器
     */
    void erase(iterator it)
    { ht_.erase(it); }

    /**
     * @brief 删除范围内的元素
     * 
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    void erase(iterator first, iterator last)
    { ht_.erase(first, last); }

    /**
     * @brief 删除指定键的元素
     * 
     * @param key 要删除的键
     * @return size_type 删除的元素数量
     */
    size_type erase(const key_type& key)
    { return ht_.erase_unique(key); }

    /**
     * @brief 清空容器
     */
    void clear()
    { ht_.clear(); }

    /**
     * @brief 交换两个容器的内容
     * 
     * @param other 要交换的容器
     */
    void swap(unordered_map& other) noexcept
    { ht_.swap(other.ht_); }

    // 查找相关

    /**
     * @brief 访问指定键的元素
     * 
     * @param key 要访问的键
     * @return mapped_type& 对应值的引用
     * @throw std::out_of_range 如果键不存在则抛出异常
     */
    mapped_type& at(const key_type& key)
    {
        iterator it = ht_.find(key);
        if (it == end())
            throw std::out_of_range("unordered_map::at: key not found");
        return it->second;
    }

    /**
     * @brief 访问指定键的元素(常量版本)
     * 
     * @param key 要访问的键
     * @return const mapped_type& 对应值的常量引用
     * @throw std::out_of_range 如果键不存在则抛出异常
     */
    const mapped_type& at(const key_type& key) const
    {
        const_iterator it = ht_.find(key);
        if (it == end())
            throw std::out_of_range("unordered_map::at: key not found");
        return it->second;
    }

    /**
     * @brief 访问或插入元素
     * 
     * @param key 要访问的键
     * @return mapped_type& 对应值的引用
     */
    mapped_type& operator[](const key_type& key)
    {
        iterator it = ht_.find(key);
        if (it == end())
            it = ht_.emplace_unique(key, T{}).first;
        return it->second;
    }

    /**
     * @brief 访问或插入元素(移动版本)
     * 
     * @param key 要访问的键
     * @return mapped_type& 对应值的引用
     */
    mapped_type& operator[](key_type&& key)
    {
        iterator it = ht_.find(key);
        if (it == end())
            it = ht_.emplace_unique(std::move(key), T{}).first;
        return it->second;
    }

    /**
     * @brief 统计指定键的元素数量
     * 
     * @param key 要统计的键
     * @return size_type 元素数量(0或1)
     */
    size_type count(const key_type& key) const
    { return ht_.count(key); }

    /**
     * @brief 查找指定键的元素
     * 
     * @param key 要查找的键
     * @return iterator 指向找到元素的迭代器，若未找到则返回end()
     */
    iterator find(const key_type& key)
    { return ht_.find(key); }

    /**
     * @brief 查找指定键的元素(常量版本)
     * 
     * @param key 要查找的键
     * @return const_iterator 指向找到元素的常量迭代器，若未找到则返回end()
     */
    const_iterator find(const key_type& key) const
    { return ht_.find(key); }

    /**
     * @brief 查找指定键的元素范围
     * 
     * @param key 要查找的键
     * @return std::pair<iterator, iterator> 指向范围的迭代器对
     */
    std::pair<iterator, iterator> equal_range(const key_type& key)
    { return ht_.equal_range_unique(key); }

    /**
     * @brief 查找指定键的元素范围(常量版本)
     * 
     * @param key 要查找的键
     * @return std::pair<const_iterator, const_iterator> 指向范围的常量迭代器对
     */
    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    { return ht_.equal_range_unique(key); }

    // 桶接口

    /**
     * @brief 返回指定桶的起始迭代器
     * 
     * @param n 桶索引
     * @return local_iterator 桶本地迭代器
     */
    local_iterator begin(size_type n) noexcept
    { return ht_.begin(n); }

    /**
     * @brief 返回指定桶的起始常量迭代器
     * 
     * @param n 桶索引
     * @return const_local_iterator 桶本地常量迭代器
     */
    const_local_iterator begin(size_type n) const noexcept
    { return ht_.begin(n); }

    /**
     * @brief 返回指定桶的起始常量迭代器
     * 
     * @param n 桶索引
     * @return const_local_iterator 桶本地常量迭代器
     */
    const_local_iterator cbegin(size_type n) const noexcept
    { return ht_.cbegin(n); }

    /**
     * @brief 返回指定桶的结束迭代器
     * 
     * @param n 桶索引
     * @return local_iterator 桶本地迭代器
     */
    local_iterator end(size_type n) noexcept
    { return ht_.end(n); }

    /**
     * @brief 返回指定桶的结束常量迭代器
     * 
     * @param n 桶索引
     * @return const_local_iterator 桶本地常量迭代器
     */
    const_local_iterator end(size_type n) const noexcept
    { return ht_.end(n); }

    /**
     * @brief 返回指定桶的结束常量迭代器
     * 
     * @param n 桶索引
     * @return const_local_iterator 桶本地常量迭代器
     */
    const_local_iterator cend(size_type n) const noexcept
    { return ht_.cend(n); }

    /**
     * @brief 返回桶数量
     * @return 桶数量
     */
    size_type bucket_count() const noexcept
    { return ht_.bucket_count(); }

    /**
     * @brief 返回可能的最大桶数
     * @return 最大桶数
     */
    size_type max_bucket_count() const noexcept
    { return ht_.max_bucket_count(); }

    /**
     * @brief 返回指定桶中的元素数量
     * 
     * @param n 桶索引
     * @return size_type 元素数量
     */
    size_type bucket_size(size_type n) const noexcept
    { return ht_.bucket_size(n); }

    /**
     * @brief 返回键所在的桶索引
     * 
     * @param key 键
     * @return size_type 桶索引
     */
    size_type bucket(const key_type& key) const
    { return ht_.bucket(key); }

    // 哈希策略

    /**
     * @brief 返回当前的负载因子
     * @return 负载因子
     */
    float load_factor() const noexcept { return ht_.load_factor(); }

    /**
     * @brief 返回最大负载因子
     * @return 最大负载因子
     */
    float max_load_factor() const noexcept { return ht_.max_load_factor(); }

    /**
     * @brief 设置最大负载因子
     * @param ml 新的最大负载因子
     */
    void max_load_factor(float ml) { ht_.max_load_factor(ml); }

    /**
     * @brief 重新哈希表，设置新的桶数
     * @param count 新的桶数
     */
    void rehash(size_type count) { ht_.rehash(count); }

    /**
     * @brief 预留空间，使容器能够容纳指定数量的元素
     * @param count 元素数量
     */
    void reserve(size_type count) { ht_.reserve(count); }

    /**
     * @brief 返回哈希函数
     * @return 哈希函数
     */
    hasher hash_function() const { return ht_.hash_function(); }

    /**
     * @brief 返回键比较函数
     * @return 键比较函数
     */
    key_equal key_eq() const { return ht_.key_eq(); }

    // 友元函数
    friend bool operator== <>(const unordered_map& lhs, const unordered_map& rhs);
    friend bool operator!= <>(const unordered_map& lhs, const unordered_map& rhs);
};

// 重载比较操作符
template <class Key, class T, class Hash, class KeyEqual>
bool operator==(const unordered_map<Key, T, Hash, KeyEqual>& lhs,
                const unordered_map<Key, T, Hash, KeyEqual>& rhs)
{
    return lhs.ht_.equal_range_unique(rhs.ht_);
}

template <class Key, class T, class Hash, class KeyEqual>
bool operator!=(const unordered_map<Key, T, Hash, KeyEqual>& lhs,
                const unordered_map<Key, T, Hash, KeyEqual>& rhs)
{
    return !lhs.ht_.equal_range_unique(rhs.ht_);
}

// 重载 swap
template <class Key, class T, class Hash, class KeyEqual>
void swap(unordered_map<Key, T, Hash, KeyEqual>& lhs,
          unordered_map<Key, T, Hash, KeyEqual>& rhs) noexcept
{
    lhs.swap(rhs);
}

/**
 * @class unordered_multimap
 * @brief 实现基于哈希表的键值对容器，键值允许重复
 * 
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Hash 哈希函数类型，默认使用 std::hash
 * @tparam KeyEqual 键比较函数类型，默认使用 std::equal_to
 */
template <class Key, class T, class Hash, class KeyEqual>
class unordered_multimap
{
private:
    // 使用 hashtable 作为底层实现
    typedef hashtable<std::pair<const Key, T>, Hash, KeyEqual> base_type;
    base_type ht_;

public:
    // 使用 hashtable 的类型定义
    typedef typename base_type::allocator_type       allocator_type;
    typedef typename base_type::key_type             key_type;
    typedef typename base_type::mapped_type          mapped_type;
    typedef typename base_type::value_type           value_type;
    typedef typename base_type::hasher               hasher;
    typedef typename base_type::key_equal            key_equal;

    typedef typename base_type::size_type            size_type;
    typedef typename base_type::difference_type      difference_type;
    typedef typename base_type::pointer              pointer;
    typedef typename base_type::const_pointer        const_pointer;
    typedef typename base_type::reference            reference;
    typedef typename base_type::const_reference      const_reference;

    typedef typename base_type::iterator             iterator;
    typedef typename base_type::const_iterator       const_iterator;
    typedef typename base_type::local_iterator       local_iterator;
    typedef typename base_type::const_local_iterator const_local_iterator;

    /**
     * @brief 获取分配器
     * @return 返回容器使用的分配器
     */
    allocator_type get_allocator() const { return ht_.get_allocator(); }

public:
    // 构造、复制、移动、析构函数
    
    /**
     * @brief 默认构造函数
     */
    unordered_multimap()
        : ht_(100, Hash(), KeyEqual())
    {
    }

    /**
     * @brief 构造函数，指定桶数和哈希函数
     * 
     * @param bucket_count 指定的桶数
     * @param hash 哈希函数
     * @param equal 键比较函数
     */
    explicit unordered_multimap(size_type bucket_count,
                             const Hash& hash = Hash(),
                             const KeyEqual& equal = KeyEqual())
        : ht_(bucket_count, hash, equal)
    {
    }

    /**
     * @brief 范围构造函数
     * 
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     * @param bucket_count 桶数量
     * @param hash 哈希函数
     * @param equal 键比较函数
     */
    template <class InputIterator>
    unordered_multimap(InputIterator first, InputIterator last,
                    const size_type bucket_count = 100,
                    const Hash& hash = Hash(),
                    const KeyEqual& equal = KeyEqual())
        : ht_(std::max(bucket_count, static_cast<size_type>(std::distance(first, last))), hash, equal)
    {
        for (; first != last; ++first)
            ht_.insert_multi_noresize(*first);
    }

    /**
     * @brief 初始化列表构造函数
     * 
     * @param ilist 初始化列表
     * @param bucket_count 桶数量
     * @param hash 哈希函数
     * @param equal 键比较函数
     */
    unordered_multimap(std::initializer_list<value_type> ilist,
                    const size_type bucket_count = 100,
                    const Hash& hash = Hash(),
                    const KeyEqual& equal = KeyEqual())
        : ht_(std::max(bucket_count, static_cast<size_type>(ilist.size())), hash, equal)
    {
        for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
            ht_.insert_multi_noresize(*first);
    }

    /**
     * @brief 拷贝构造函数
     * 
     * @param rhs 源对象
     */
    unordered_multimap(const unordered_multimap& rhs)
        : ht_(rhs.ht_)
    {
    }

    /**
     * @brief 移动构造函数
     * 
     * @param rhs 源对象
     */
    unordered_multimap(unordered_multimap&& rhs) noexcept
        : ht_(std::move(rhs.ht_))
    {
    }

    /**
     * @brief 拷贝赋值运算符
     * 
     * @param rhs 源对象
     * @return unordered_multimap& 返回自身引用
     */
    unordered_multimap& operator=(const unordered_multimap& rhs)
    {
        ht_ = rhs.ht_;
        return *this;
    }

    /**
     * @brief 移动赋值运算符
     * 
     * @param rhs 源对象
     * @return unordered_multimap& 返回自身引用
     */
    unordered_multimap& operator=(unordered_multimap&& rhs) noexcept
    {
        ht_ = std::move(rhs.ht_);
        return *this;
    }

    /**
     * @brief 初始化列表赋值运算符
     * 
     * @param ilist 初始化列表
     * @return unordered_multimap& 返回自身引用
     */
    unordered_multimap& operator=(std::initializer_list<value_type> ilist)
    {
        ht_.clear();
        ht_.reserve(ilist.size());
        for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
            ht_.insert_multi_noresize(*first);
        return *this;
    }

    /**
     * @brief 析构函数
     */
    ~unordered_multimap() = default;

    // 迭代器相关

    /**
     * @brief 返回指向容器第一个元素的迭代器
     * @return 迭代器
     */
    iterator begin() noexcept
    { return ht_.begin(); }

    /**
     * @brief 返回指向容器第一个元素的常量迭代器
     * @return 常量迭代器
     */
    const_iterator begin() const noexcept
    { return ht_.begin(); }

    /**
     * @brief 返回指向容器尾部的迭代器
     * @return 迭代器
     */
    iterator end() noexcept
    { return ht_.end(); }

    /**
     * @brief 返回指向容器尾部的常量迭代器
     * @return 常量迭代器
     */
    const_iterator end() const noexcept
    { return ht_.end(); }

    /**
     * @brief 返回指向容器第一个元素的常量迭代器
     * @return 常量迭代器
     */
    const_iterator cbegin() const noexcept
    { return ht_.cbegin(); }

    /**
     * @brief 返回指向容器尾部的常量迭代器
     * @return 常量迭代器
     */
    const_iterator cend() const noexcept
    { return ht_.cend(); }

    // 容量相关

    /**
     * @brief 判断容器是否为空
     * @return 如果容器为空，返回true；否则，返回false
     */
    bool empty() const noexcept { return ht_.empty(); }

    /**
     * @brief 返回容器中元素的数量
     * @return 元素数量
     */
    size_type size() const noexcept { return ht_.size(); }

    /**
     * @brief 返回容器能够容纳的最大元素数量
     * @return 最大元素数量
     */
    size_type max_size() const noexcept { return ht_.max_size(); }

    // 修改容器操作

    // emplace / emplace_hint

    /**
     * @brief 原位构造元素
     * 
     * @tparam Args 参数类型包
     * @param args 构造参数
     * @return iterator 指向新元素的迭代器
     */
    template <class... Args>
    iterator emplace(Args&&... args)
    { return ht_.emplace_multi(std::forward<Args>(args)...); }

    /**
     * @brief 使用提示的原位构造元素
     * 
     * @tparam Args 参数类型包
     * @param hint 提示位置
     * @param args 构造参数
     * @return iterator 指向新元素的迭代器
     */
    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    { return ht_.emplace_multi_use_hint(hint, std::forward<Args>(args)...); }

    // insert

    /**
     * @brief 插入元素
     * 
     * @param value 要插入的值
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(const value_type& value)
    { return ht_.insert_multi(value); }

    /**
     * @brief 插入元素(移动版本)
     * 
     * @param value 要插入的值
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(value_type&& value)
    { return ht_.emplace_multi(std::move(value)); }

    /**
     * @brief 使用提示位置插入元素
     * 
     * @param hint 提示位置
     * @param value 要插入的值
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(const_iterator hint, const value_type& value)
    { return ht_.insert_multi_use_hint(hint, value); }

    /**
     * @brief 使用提示位置插入元素(移动版本)
     * 
     * @param hint 提示位置
     * @param value 要插入的值
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(const_iterator hint, value_type&& value)
    { return ht_.emplace_multi_use_hint(hint, std::move(value)); }

    /**
     * @brief 插入范围内的元素
     * 
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    { ht_.insert_multi(first, last); }

    // erase / clear

    /**
     * @brief 删除指定位置的元素
     * 
     * @param it 要删除元素的迭代器
     */
    void erase(iterator it)
    { ht_.erase(it); }

    /**
     * @brief 删除范围内的元素
     * 
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    void erase(iterator first, iterator last)
    { ht_.erase(first, last); }

    /**
     * @brief 删除指定键的元素
     * 
     * @param key 要删除的键
     * @return size_type 删除的元素数量
     */
    size_type erase(const key_type& key)
    { return ht_.erase_multi(key); }

    /**
     * @brief 清空容器
     */
    void clear()
    { ht_.clear(); }

    /**
     * @brief 交换两个容器的内容
     * 
     * @param other 要交换的容器
     */
    void swap(unordered_multimap& other) noexcept
    { ht_.swap(other.ht_); }

    // 查找相关

    /**
     * @brief 访问指定键的元素
     * 
     * @param key 要访问的键
     * @return mapped_type& 对应值的引用
     * @throw std::out_of_range 如果键不存在则抛出异常
     */
    mapped_type& at(const key_type& key)
    {
        iterator it = ht_.find(key);
        if (it == end())
            throw std::out_of_range("unordered_multimap::at: key not found");
        return it->second;
    }

    /**
     * @brief 访问指定键的元素(常量版本)
     * 
     * @param key 要访问的键
     * @return const mapped_type& 对应值的常量引用
     * @throw std::out_of_range 如果键不存在则抛出异常
     */
    const mapped_type& at(const key_type& key) const
    {
        const_iterator it = ht_.find(key);
        if (it == end())
            throw std::out_of_range("unordered_multimap::at: key not found");
        return it->second;
    }

    /**
     * @brief 访问或插入元素
     * 
     * @param key 要访问的键
     * @return mapped_type& 对应值的引用
     */
    mapped_type& operator[](const key_type& key)
    {
        iterator it = ht_.find(key);
        if (it == end())
            it = ht_.emplace_multi(key, T{}).first;
        return it->second;
    }

    /**
     * @brief 访问或插入元素(移动版本)
     * 
     * @param key 要访问的键
     * @return mapped_type& 对应值的引用
     */
    mapped_type& operator[](key_type&& key)
    {
        iterator it = ht_.find(key);
        if (it == end())
            it = ht_.emplace_multi(std::move(key), T{}).first;
        return it->second;
    }

    /**
     * @brief 统计指定键的元素数量
     * 
     * @param key 要统计的键
     * @return size_type 元素数量
     */
    size_type count(const key_type& key) const
    { return ht_.count(key); }

    /**
     * @brief 查找指定键的元素
     * 
     * @param key 要查找的键
     * @return iterator 指向找到元素的迭代器，若未找到则返回end()
     */
    iterator find(const key_type& key)
    { return ht_.find(key); }

    /**
     * @brief 查找指定键的元素(常量版本)
     * 
     * @param key 要查找的键
     * @return const_iterator 指向找到元素的常量迭代器，若未找到则返回end()
     */
    const_iterator find(const key_type& key) const
    { return ht_.find(key); }

    /**
     * @brief 查找指定键的元素范围
     * 
     * @param key 要查找的键
     * @return std::pair<iterator, iterator> 指向范围的迭代器对
     */
    std::pair<iterator, iterator> equal_range(const key_type& key)
    { return ht_.equal_range_multi(key); }

    /**
     * @brief 查找指定键的元素范围(常量版本)
     * 
     * @param key 要查找的键
     * @return std::pair<const_iterator, const_iterator> 指向范围的常量迭代器对
     */
    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    { return ht_.equal_range_multi(key); }

    // 桶接口

    /**
     * @brief 返回指定桶的起始迭代器
     * 
     * @param n 桶索引
     * @return local_iterator 桶本地迭代器
     */
    local_iterator begin(size_type n) noexcept
    { return ht_.begin(n); }

    /**
     * @brief 返回指定桶的起始常量迭代器
     * 
     * @param n 桶索引
     * @return const_local_iterator 桶本地常量迭代器
     */
    const_local_iterator begin(size_type n) const noexcept
    { return ht_.begin(n); }

    /**
     * @brief 返回指定桶的起始常量迭代器
     * 
     * @param n 桶索引
     * @return const_local_iterator 桶本地常量迭代器
     */
    const_local_iterator cbegin(size_type n) const noexcept
    { return ht_.cbegin(n); }

    /**
     * @brief 返回指定桶的结束迭代器
     * 
     * @param n 桶索引
     * @return local_iterator 桶本地迭代器
     */
    local_iterator end(size_type n) noexcept
    { return ht_.end(n); }

    /**
     * @brief 返回指定桶的结束常量迭代器
     * 
     * @param n 桶索引
     * @return const_local_iterator 桶本地常量迭代器
     */
    const_local_iterator end(size_type n) const noexcept
    { return ht_.end(n); }

    /**
     * @brief 返回指定桶的结束常量迭代器
     * 
     * @param n 桶索引
     * @return const_local_iterator 桶本地常量迭代器
     */
    const_local_iterator cend(size_type n) const noexcept
    { return ht_.cend(n); }

    /**
     * @brief 返回桶数量
     * @return 桶数量
     */
    size_type bucket_count() const noexcept
    { return ht_.bucket_count(); }

    /**
     * @brief 返回可能的最大桶数
     * @return 最大桶数
     */
    size_type max_bucket_count() const noexcept
    { return ht_.max_bucket_count(); }

    /**
     * @brief 返回指定桶中的元素数量
     * 
     * @param n 桶索引
     * @return size_type 元素数量
     */
    size_type bucket_size(size_type n) const noexcept
    { return ht_.bucket_size(n); }

    /**
     * @brief 返回键所在的桶索引
     * 
     * @param key 键
     * @return size_type 桶索引
     */
    size_type bucket(const key_type& key) const
    { return ht_.bucket(key); }

    // 哈希策略

    /**
     * @brief 返回当前的负载因子
     * @return 负载因子
     */
    float load_factor() const noexcept { return ht_.load_factor(); }

    /**
     * @brief 返回最大负载因子
     * @return 最大负载因子
     */
    float max_load_factor() const noexcept { return ht_.max_load_factor(); }

    /**
     * @brief 设置最大负载因子
     * @param ml 新的最大负载因子
     */
    void max_load_factor(float ml) { ht_.max_load_factor(ml); }

    /**
     * @brief 重新哈希表，设置新的桶数
     * @param count 新的桶数
     */
    void rehash(size_type count) { ht_.rehash(count); }

    /**
     * @brief 预留空间，使容器能够容纳指定数量的元素
     * @param count 元素数量
     */
    void reserve(size_type count) { ht_.reserve(count); }

    /**
     * @brief 返回哈希函数
     * @return 哈希函数
     */
    hasher hash_function() const { return ht_.hash_function(); }

    /**
     * @brief 返回键比较函数
     * @return 键比较函数
     */
    key_equal key_eq() const { return ht_.key_eq(); }

    // 友元函数
    friend bool operator== <>(const unordered_multimap& lhs, const unordered_multimap& rhs);
    friend bool operator!= <>(const unordered_multimap& lhs, const unordered_multimap& rhs);
};

// 重载比较操作符
template <class Key, class T, class Hash, class KeyEqual>
bool operator==(const unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
                const unordered_multimap<Key, T, Hash, KeyEqual>& rhs)
{
    return lhs.ht_.equal_range_multi(rhs.ht_);
}

template <class Key, class T, class Hash, class KeyEqual>
bool operator!=(const unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
                const unordered_multimap<Key, T, Hash, KeyEqual>& rhs)
{
    return !lhs.ht_.equal_range_multi(rhs.ht_);
}

// 重载 swap
template <class Key, class T, class Hash, class KeyEqual>
void swap(unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
          unordered_multimap<Key, T, Hash, KeyEqual>& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace mystl

#endif // MY_UNORDERED_MAP_H 