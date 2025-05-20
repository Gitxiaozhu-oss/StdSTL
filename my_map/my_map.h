#ifndef MY_MAP_H_
#define MY_MAP_H_

// 这个头文件包含了两个模板类 map 和 multimap
// map      : 映射，元素具有键值和实值，会根据键值大小自动排序，键值不允许重复
// multimap : 映射，元素具有键值和实值，会根据键值大小自动排序，键值允许重复

// 注释：
//
// 异常保证：
// mystl::map<Key, T> / mystl::multimap<Key, T> 满足基本异常保证，对以下等函数做强异常安全保证：
//   * emplace
//   * emplace_hint
//   * insert

#include "../my_rb_tree/my_rb_tree.h"
#include <initializer_list>
#include <functional>
#include <utility>
#include <stdexcept>

// 定义异常检查宏
#define THROW_OUT_OF_RANGE_IF(expr, what) \
    if ((expr)) throw std::out_of_range(what)

namespace my
{

// 前向声明比较函数模板
template <class T>
struct less : public std::binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x < y; }
};

// 前置声明move函数模板
template <class T>
typename std::remove_reference<T>::type&& move(T&& t) noexcept {
    return static_cast<typename std::remove_reference<T>::type&&>(t);
}

// 前置声明forward函数模板
template <class T>
T&& forward(typename std::remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

template <class T>
T&& forward(typename std::remove_reference<T>::type&& t) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "不能将右值引用转发为左值引用");
    return static_cast<T&&>(t);
}

/**
 * @brief 模板类 map，键值不允许重复
 * @tparam Key 键值类型
 * @tparam T 实值类型
 * @tparam Compare 键值比较方式，默认使用 my::less
 */
template <class Key, class T, class Compare = my::less<Key>>
class map
{
public:
    /**
     * @brief map 的嵌套型别定义
     */
    typedef Key                        key_type;        // 键值类型
    typedef T                          mapped_type;     // 实值类型
    typedef std::pair<const Key, T>    value_type;      // 键值对类型
    typedef Compare                    key_compare;     // 键值比较函数对象类型

    /**
     * @brief 定义一个用于元素比较的仿函数类
     */
    class value_compare : public std::binary_function<value_type, value_type, bool>
    {
        friend class map<Key, T, Compare>;
    private:
        Compare comp;
        value_compare(Compare c) : comp(c) {}
    public:
        /**
         * @brief 重载函数调用运算符，比较两个value_type元素的键值
         */
        bool operator()(const value_type& lhs, const value_type& rhs) const
        {
            return comp(lhs.first, rhs.first);  // 比较键值的大小
        }
    };

private:
    /**
     * @brief 以红黑树作为底层机制
     */
    typedef mystl::rb_tree<value_type, key_compare>  base_type;
    base_type tree_;  // 红黑树成员变量

public:
    /**
     * @brief 使用红黑树的型别定义
     */
    typedef typename base_type::node_type              node_type;
    typedef typename base_type::pointer                pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::reference              reference;
    typedef typename base_type::const_reference        const_reference;
    typedef typename base_type::iterator               iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::reverse_iterator       reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::difference_type        difference_type;
    typedef typename base_type::allocator_type         allocator_type;

    /**
     * @brief 构造、复制、移动、赋值函数
     */
    
    /**
     * @brief 默认构造函数
     */
    map() = default;

    /**
     * @brief 范围构造函数
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围起始
     * @param last 范围结束
     */
    template <class InputIterator>
    map(InputIterator first, InputIterator last)
        : tree_()
    {
        tree_.insert_unique(first, last);
    }

    /**
     * @brief 初始化列表构造函数
     * @param ilist 初始化列表
     */
    map(std::initializer_list<value_type> ilist)
        : tree_()
    {
        tree_.insert_unique(ilist.begin(), ilist.end());
    }

    /**
     * @brief 拷贝构造函数
     * @param rhs 被拷贝对象
     */
    map(const map& rhs)
        : tree_(rhs.tree_)
    {
    }

    /**
     * @brief 移动构造函数
     * @param rhs 被移动对象
     */
    map(map&& rhs) noexcept
        : tree_(my::move(rhs.tree_))
    {
    }

    /**
     * @brief 拷贝赋值运算符
     * @param rhs 被拷贝对象
     * @return map& 返回自身引用
     */
    map& operator=(const map& rhs)
    {
        tree_ = rhs.tree_;
        return *this;
    }

    /**
     * @brief 移动赋值运算符
     * @param rhs 被移动对象
     * @return map& 返回自身引用
     */
    map& operator=(map&& rhs)
    {
        tree_ = my::move(rhs.tree_);
        return *this;
    }

    /**
     * @brief 初始化列表赋值运算符
     * @param ilist 初始化列表
     * @return map& 返回自身引用
     */
    map& operator=(std::initializer_list<value_type> ilist)
    {
        tree_.clear();
        tree_.insert_unique(ilist.begin(), ilist.end());
        return *this;
    }

    /**
     * @brief 相关接口函数
     */
    
    /**
     * @brief 获取键值比较函数对象
     * @return key_compare 键值比较函数对象
     */
    key_compare key_comp() const { return tree_.key_comp(); }
    
    /**
     * @brief 获取值比较函数对象
     * @return value_compare 值比较函数对象
     */
    value_compare value_comp() const { return value_compare(tree_.key_comp()); }
    
    /**
     * @brief 获取分配器
     * @return allocator_type 分配器
     */
    allocator_type get_allocator() const { return tree_.get_allocator(); }

    /**
     * @brief 迭代器相关操作
     */
    
    /**
     * @brief 获取起始迭代器
     * @return iterator 起始迭代器
     */
    iterator begin() noexcept
    { return tree_.begin(); }
    
    /**
     * @brief 获取起始常量迭代器
     * @return const_iterator 起始常量迭代器
     */
    const_iterator begin() const noexcept
    { return tree_.begin(); }
    
    /**
     * @brief 获取结束迭代器
     * @return iterator 结束迭代器
     */
    iterator end() noexcept
    { return tree_.end(); }
    
    /**
     * @brief 获取结束常量迭代器
     * @return const_iterator 结束常量迭代器
     */
    const_iterator end() const noexcept
    { return tree_.end(); }

    /**
     * @brief 获取反向起始迭代器
     * @return reverse_iterator 反向起始迭代器
     */
    reverse_iterator rbegin() noexcept
    { return reverse_iterator(end()); }
    
    /**
     * @brief 获取反向起始常量迭代器
     * @return const_reverse_iterator 反向起始常量迭代器
     */
    const_reverse_iterator rbegin() const noexcept
    { return const_reverse_iterator(end()); }
    
    /**
     * @brief 获取反向结束迭代器
     * @return reverse_iterator 反向结束迭代器
     */
    reverse_iterator rend() noexcept
    { return reverse_iterator(begin()); }
    
    /**
     * @brief 获取反向结束常量迭代器
     * @return const_reverse_iterator 反向结束常量迭代器
     */
    const_reverse_iterator rend() const noexcept
    { return const_reverse_iterator(begin()); }

    /**
     * @brief 获取常量起始迭代器
     * @return const_iterator 常量起始迭代器
     */
    const_iterator cbegin() const noexcept
    { return begin(); }
    
    /**
     * @brief 获取常量结束迭代器
     * @return const_iterator 常量结束迭代器
     */
    const_iterator cend() const noexcept
    { return end(); }
    
    /**
     * @brief 获取常量反向起始迭代器
     * @return const_reverse_iterator 常量反向起始迭代器
     */
    const_reverse_iterator crbegin() const noexcept
    { return rbegin(); }
    
    /**
     * @brief 获取常量反向结束迭代器
     * @return const_reverse_iterator 常量反向结束迭代器
     */
    const_reverse_iterator crend() const noexcept
    { return rend(); }

    /**
     * @brief 容量相关操作
     */
    
    /**
     * @brief 判断容器是否为空
     * @return bool 如果容器为空返回true，否则返回false
     */
    bool empty() const noexcept { return tree_.empty(); }
    
    /**
     * @brief 获取容器中元素数量
     * @return size_type 元素数量
     */
    size_type size() const noexcept { return tree_.size(); }
    
    /**
     * @brief 获取容器可容纳的最大元素数量
     * @return size_type 最大元素数量
     */
    size_type max_size() const noexcept { return tree_.max_size(); }

    /**
     * @brief 元素访问相关操作
     */
    
    /**
     * @brief 访问指定键的元素，若键不存在则抛出异常
     * @param key 要访问的键
     * @return mapped_type& 对应的值的引用
     * @throw std::out_of_range 如果键不存在
     */
    mapped_type& at(const key_type& key)
    {
        iterator it = lower_bound(key);
        // it->first >= key
        THROW_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
                            "map<Key, T> no such element exists");
        return it->second;
    }

    /**
     * @brief 访问指定键的元素，若键不存在则抛出异常（常量版本）
     * @param key 要访问的键
     * @return const mapped_type& 对应的值的常量引用
     * @throw std::out_of_range 如果键不存在
     */
    const mapped_type& at(const key_type& key) const
    {
        const_iterator it = lower_bound(key);
        // it->first >= key
        THROW_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
                            "map<Key, T> no such element exists");
        return it->second;
    }

    /**
     * @brief 访问或插入指定键的元素
     * @param key 要访问的键
     * @return mapped_type& 对应的值的引用
     * @note 如果键不存在，会创建一个新的元素插入
     */
    mapped_type& operator[](const key_type& key)
    {
        iterator it = lower_bound(key);
        // it->first >= key
        if (it == end() || key_comp()(key, it->first))
            it = emplace_hint(it, key, T{});
        return it->second;
    }

    /**
     * @brief 访问或插入指定键的元素（移动版本）
     * @param key 要访问的键（右值引用）
     * @return mapped_type& 对应的值的引用
     * @note 如果键不存在，会创建一个新的元素插入
     */
    mapped_type& operator[](key_type&& key)
    {
        iterator it = lower_bound(key);
        // it->first >= key
        if (it == end() || key_comp()(key, it->first))
            it = emplace_hint(it, my::move(key), T{});
        return it->second;
    }

    /**
     * @brief 插入和删除相关操作
     */
    
    /**
     * @brief 在容器中就地构造一个新元素
     * @tparam Args 参数类型包
     * @param args 构造元素的参数
     * @return std::pair<iterator,bool> 包含指向新元素的迭代器和是否插入成功的布尔值的对
     */
    template <class ...Args>
    std::pair<iterator, bool> emplace(Args&& ...args)
    {
        return tree_.emplace_unique(my::forward<Args>(args)...);
    }

    /**
     * @brief 在指定位置附近就地构造一个新元素
     * @tparam Args 参数类型包
     * @param hint 指定的位置
     * @param args 构造元素的参数
     * @return iterator 指向新元素的迭代器
     */
    template <class ...Args>
    iterator emplace_hint(iterator hint, Args&& ...args)
    {
        return tree_.emplace_unique_use_hint(hint, my::forward<Args>(args)...);
    }

    /**
     * @brief 插入一个元素
     * @param value 要插入的元素
     * @return std::pair<iterator,bool> 包含指向新元素的迭代器和是否插入成功的布尔值的对
     */
    std::pair<iterator, bool> insert(const value_type& value)
    {
        return tree_.insert_unique(value);
    }

    /**
     * @brief 插入一个元素（移动版本）
     * @param value 要插入的元素（右值引用）
     * @return std::pair<iterator,bool> 包含指向新元素的迭代器和是否插入成功的布尔值的对
     */
    std::pair<iterator, bool> insert(value_type&& value)
    {
        return tree_.insert_unique(my::move(value));
    }

    /**
     * @brief 在指定位置附近插入一个元素
     * @param hint 指定的位置
     * @param value 要插入的元素
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(iterator hint, const value_type& value)
    {
        return tree_.insert_unique(hint, value);
    }

    /**
     * @brief 在指定位置附近插入一个元素（移动版本）
     * @param hint 指定的位置
     * @param value 要插入的元素（右值引用）
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(iterator hint, value_type&& value)
    {
        return tree_.insert_unique(hint, my::move(value));
    }

    /**
     * @brief 插入一个范围内的元素
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围起始
     * @param last 范围结束
     */
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        tree_.insert_unique(first, last);
    }

    /**
     * @brief 删除指定位置的元素
     * @param position 指定的位置
     */
    void erase(iterator position)
    {
        tree_.erase(position);
    }

    /**
     * @brief 删除指定键的元素
     * @param key 要删除的键
     * @return size_type 删除的元素数量
     */
    size_type erase(const key_type& key)
    {
        return tree_.erase_unique(key);
    }

    /**
     * @brief 删除一个范围内的元素
     * @param first 范围起始
     * @param last 范围结束
     */
    void erase(iterator first, iterator last)
    {
        tree_.erase(first, last);
    }

    /**
     * @brief 清空容器中的所有元素
     */
    void clear()
    {
        tree_.clear();
    }

    /**
     * @brief map相关操作，主要是查找
     */
    
    /**
     * @brief 查找指定键的元素
     * @param key 要查找的键
     * @return iterator 指向找到的元素的迭代器，如果未找到则返回end()
     */
    iterator find(const key_type& key)
    {
        return tree_.find(key);
    }

    /**
     * @brief 查找指定键的元素（常量版本）
     * @param key 要查找的键
     * @return const_iterator 指向找到的元素的常量迭代器，如果未找到则返回end()
     */
    const_iterator find(const key_type& key) const
    {
        return tree_.find(key);
    }

    /**
     * @brief 统计指定键的元素数量
     * @param key 要统计的键
     * @return size_type 元素的数量（对于map，只会是0或1）
     */
    size_type count(const key_type& key) const
    {
        return tree_.count_unique(key);
    }

    /**
     * @brief 获取第一个不小于指定键的元素的迭代器
     * @param key 指定的键
     * @return iterator 第一个不小于key的元素的迭代器
     */
    iterator lower_bound(const key_type& key)
    {
        return tree_.lower_bound(key);
    }

    /**
     * @brief 获取第一个不小于指定键的元素的迭代器（常量版本）
     * @param key 指定的键
     * @return const_iterator 第一个不小于key的元素的常量迭代器
     */
    const_iterator lower_bound(const key_type& key) const
    {
        return tree_.lower_bound(key);
    }

    /**
     * @brief 获取第一个大于指定键的元素的迭代器
     * @param key 指定的键
     * @return iterator 第一个大于key的元素的迭代器
     */
    iterator upper_bound(const key_type& key)
    {
        return tree_.upper_bound(key);
    }

    /**
     * @brief 获取第一个大于指定键的元素的迭代器（常量版本）
     * @param key 指定的键
     * @return const_iterator 第一个大于key的元素的常量迭代器
     */
    const_iterator upper_bound(const key_type& key) const
    {
        return tree_.upper_bound(key);
    }

    /**
     * @brief 获取指定键的元素范围
     * @param key 指定的键
     * @return std::pair<iterator,iterator> 包含范围的迭代器对
     */
    std::pair<iterator, iterator> equal_range(const key_type& key)
    {
        return tree_.equal_range_unique(key);
    }

    /**
     * @brief 获取指定键的元素范围（常量版本）
     * @param key 指定的键
     * @return std::pair<const_iterator,const_iterator> 包含范围的常量迭代器对
     */
    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    {
        return tree_.equal_range_unique(key);
    }

    /**
     * @brief 交换两个map的内容
     * @param rhs 要交换的另一个map
     */
    void swap(map& rhs) noexcept
    {
        tree_.swap(rhs.tree_);
    }

public:
    /**
     * @brief 友元操作符，比较两个map是否相等
     */
    friend bool operator==(const map& lhs, const map& rhs) { return lhs.tree_ == rhs.tree_; }
    
    /**
     * @brief 友元操作符，比较两个map的大小关系
     */
    friend bool operator< (const map& lhs, const map& rhs) { return lhs.tree_ <  rhs.tree_; }
};

/**
 * @brief 重载比较操作符
 */

/**
 * @brief 判断两个map是否相等
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧map
 * @param rhs 右侧map
 * @return bool 如果相等返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator==(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
    return lhs == rhs;
}

/**
 * @brief 判断一个map是否小于另一个map
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧map
 * @param rhs 右侧map
 * @return bool 如果lhs小于rhs返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator<(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
    return lhs < rhs;
}

/**
 * @brief 判断两个map是否不相等
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧map
 * @param rhs 右侧map
 * @return bool 如果不相等返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator!=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
    return !(lhs == rhs);
}

/**
 * @brief 判断一个map是否大于另一个map
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧map
 * @param rhs 右侧map
 * @return bool 如果lhs大于rhs返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator>(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
    return rhs < lhs;
}

/**
 * @brief 判断一个map是否小于等于另一个map
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧map
 * @param rhs 右侧map
 * @return bool 如果lhs小于等于rhs返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator<=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
    return !(rhs < lhs);
}

/**
 * @brief 判断一个map是否大于等于另一个map
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧map
 * @param rhs 右侧map
 * @return bool 如果lhs大于等于rhs返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator>=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
    return !(lhs < rhs);
}

/**
 * @brief 交换两个map的内容
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧map
 * @param rhs 右侧map
 */
template <class Key, class T, class Compare>
void swap(map<Key, T, Compare>& lhs, map<Key, T, Compare>& rhs) noexcept
{
    lhs.swap(rhs);
}

/*****************************************************************************************/

/**
 * @brief 模板类 multimap，键值允许重复
 * @tparam Key 键值类型
 * @tparam T 实值类型
 * @tparam Compare 键值比较方式，默认使用 my::less
 */
template <class Key, class T, class Compare = my::less<Key>>
class multimap
{
public:
    /**
     * @brief multimap 的型别定义
     */
    typedef Key                        key_type;
    typedef T                          mapped_type;
    typedef std::pair<const Key, T>    value_type;
    typedef Compare                    key_compare;

    /**
     * @brief 定义一个用于元素比较的仿函数类
     */
    class value_compare : public std::binary_function<value_type, value_type, bool>
    {
        friend class multimap<Key, T, Compare>;
    private:
        Compare comp;
        value_compare(Compare c) : comp(c) {}
    public:
        /**
         * @brief 重载函数调用运算符，比较两个value_type元素的键值
         */
        bool operator()(const value_type& lhs, const value_type& rhs) const
        {
            return comp(lhs.first, rhs.first);
        }
    };

private:
    /**
     * @brief 以红黑树作为底层机制
     */
    typedef mystl::rb_tree<value_type, key_compare>  base_type;
    base_type tree_;  // 红黑树成员变量

public:
    /**
     * @brief 使用红黑树的型别定义
     */
    typedef typename base_type::node_type              node_type;
    typedef typename base_type::pointer                pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::reference              reference;
    typedef typename base_type::const_reference        const_reference;
    typedef typename base_type::iterator               iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::reverse_iterator       reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::difference_type        difference_type;
    typedef typename base_type::allocator_type         allocator_type;

public:
    /**
     * @brief 构造、复制、移动函数
     */
    
    /**
     * @brief 默认构造函数
     */
    multimap() = default;

    /**
     * @brief 范围构造函数
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围起始
     * @param last 范围结束
     */
    template <class InputIterator>
    multimap(InputIterator first, InputIterator last)
        : tree_()
    {
        tree_.insert_multi(first, last);
    }
    
    /**
     * @brief 初始化列表构造函数
     * @param ilist 初始化列表
     */
    multimap(std::initializer_list<value_type> ilist)
        : tree_()
    {
        tree_.insert_multi(ilist.begin(), ilist.end());
    }

    /**
     * @brief 拷贝构造函数
     * @param rhs 被拷贝对象
     */
    multimap(const multimap& rhs)
        : tree_(rhs.tree_)
    {
    }
    
    /**
     * @brief 移动构造函数
     * @param rhs 被移动对象
     */
    multimap(multimap&& rhs) noexcept
        : tree_(my::move(rhs.tree_))
    {
    }

    /**
     * @brief 拷贝赋值运算符
     * @param rhs 被拷贝对象
     * @return multimap& 返回自身引用
     */
    multimap& operator=(const multimap& rhs)
    {
        tree_ = rhs.tree_;
        return *this;
    }
    
    /**
     * @brief 移动赋值运算符
     * @param rhs 被移动对象
     * @return multimap& 返回自身引用
     */
    multimap& operator=(multimap&& rhs)
    {
        tree_ = my::move(rhs.tree_);
        return *this;
    }

    /**
     * @brief 初始化列表赋值运算符
     * @param ilist 初始化列表
     * @return multimap& 返回自身引用
     */
    multimap& operator=(std::initializer_list<value_type> ilist)
    {
        tree_.clear();
        tree_.insert_multi(ilist.begin(), ilist.end());
        return *this;
    }

    /**
     * @brief 相关接口
     */
    
    /**
     * @brief 获取键值比较函数对象
     * @return key_compare 键值比较函数对象
     */
    key_compare key_comp() const { return tree_.key_comp(); }
    
    /**
     * @brief 获取值比较函数对象
     * @return value_compare 值比较函数对象
     */
    value_compare value_comp() const { return value_compare(tree_.key_comp()); }
    
    /**
     * @brief 获取分配器
     * @return allocator_type 分配器
     */
    allocator_type get_allocator() const { return tree_.get_allocator(); }

    /**
     * @brief 迭代器相关
     */
    
    /**
     * @brief 获取起始迭代器
     * @return iterator 起始迭代器
     */
    iterator begin() noexcept
    { return tree_.begin(); }
    
    /**
     * @brief 获取起始常量迭代器
     * @return const_iterator 起始常量迭代器
     */
    const_iterator begin() const noexcept
    { return tree_.begin(); }
    
    /**
     * @brief 获取结束迭代器
     * @return iterator 结束迭代器
     */
    iterator end() noexcept
    { return tree_.end(); }
    
    /**
     * @brief 获取结束常量迭代器
     * @return const_iterator 结束常量迭代器
     */
    const_iterator end() const noexcept
    { return tree_.end(); }

    /**
     * @brief 获取反向起始迭代器
     * @return reverse_iterator 反向起始迭代器
     */
    reverse_iterator rbegin() noexcept
    { return reverse_iterator(end()); }
    
    /**
     * @brief 获取反向起始常量迭代器
     * @return const_reverse_iterator 反向起始常量迭代器
     */
    const_reverse_iterator rbegin() const noexcept
    { return const_reverse_iterator(end()); }
    
    /**
     * @brief 获取反向结束迭代器
     * @return reverse_iterator 反向结束迭代器
     */
    reverse_iterator rend() noexcept
    { return reverse_iterator(begin()); }
    
    /**
     * @brief 获取反向结束常量迭代器
     * @return const_reverse_iterator 反向结束常量迭代器
     */
    const_reverse_iterator rend() const noexcept
    { return const_reverse_iterator(begin()); }

    /**
     * @brief 获取常量起始迭代器
     * @return const_iterator 常量起始迭代器
     */
    const_iterator cbegin() const noexcept
    { return begin(); }
    
    /**
     * @brief 获取常量结束迭代器
     * @return const_iterator 常量结束迭代器
     */
    const_iterator cend() const noexcept
    { return end(); }
    
    /**
     * @brief 获取常量反向起始迭代器
     * @return const_reverse_iterator 常量反向起始迭代器
     */
    const_reverse_iterator crbegin() const noexcept
    { return rbegin(); }
    
    /**
     * @brief 获取常量反向结束迭代器
     * @return const_reverse_iterator 常量反向结束迭代器
     */
    const_reverse_iterator crend() const noexcept
    { return rend(); }

    /**
     * @brief 容量相关
     */
    
    /**
     * @brief 判断容器是否为空
     * @return bool 如果容器为空返回true，否则返回false
     */
    bool empty() const noexcept { return tree_.empty(); }
    
    /**
     * @brief 获取容器中元素数量
     * @return size_type 元素数量
     */
    size_type size() const noexcept { return tree_.size(); }
    
    /**
     * @brief 获取容器可容纳的最大元素数量
     * @return size_type 最大元素数量
     */
    size_type max_size() const noexcept { return tree_.max_size(); }

    /**
     * @brief 插入删除操作
     */
    
    /**
     * @brief 在容器中就地构造一个新元素
     * @tparam Args 参数类型包
     * @param args 构造元素的参数
     * @return iterator 指向新元素的迭代器
     */
    template <class ...Args>
    iterator emplace(Args&& ...args)
    {
        return tree_.emplace_multi(my::forward<Args>(args)...);
    }

    /**
     * @brief 在指定位置附近就地构造一个新元素
     * @tparam Args 参数类型包
     * @param hint 指定的位置
     * @param args 构造元素的参数
     * @return iterator 指向新元素的迭代器
     */
    template <class ...Args>
    iterator emplace_hint(iterator hint, Args&& ...args)
    {
        return tree_.emplace_multi_use_hint(hint, my::forward<Args>(args)...);
    }

    /**
     * @brief 插入一个元素
     * @param value 要插入的元素
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(const value_type& value)
    {
        return tree_.insert_multi(value);
    }
    
    /**
     * @brief 插入一个元素（移动版本）
     * @param value 要插入的元素（右值引用）
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(value_type&& value)
    {
        return tree_.insert_multi(my::move(value));
    }

    /**
     * @brief 在指定位置附近插入一个元素
     * @param hint 指定的位置
     * @param value 要插入的元素
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(iterator hint, const value_type& value)
    {
        return tree_.insert_multi(hint, value);
    }
    
    /**
     * @brief 在指定位置附近插入一个元素（移动版本）
     * @param hint 指定的位置
     * @param value 要插入的元素（右值引用）
     * @return iterator 指向新元素的迭代器
     */
    iterator insert(iterator hint, value_type&& value)
    {
        return tree_.insert_multi(hint, my::move(value));
    }

    /**
     * @brief 插入一个范围内的元素
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围起始
     * @param last 范围结束
     */
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        tree_.insert_multi(first, last);
    }

    /**
     * @brief 删除指定位置的元素
     * @param position 指定的位置
     */
    void erase(iterator position)
    {
        tree_.erase(position);
    }
    
    /**
     * @brief 删除指定键的元素
     * @param key 要删除的键
     * @return size_type 删除的元素数量
     */
    size_type erase(const key_type& key)
    {
        return tree_.erase_multi(key);
    }
    
    /**
     * @brief 删除一个范围内的元素
     * @param first 范围起始
     * @param last 范围结束
     */
    void erase(iterator first, iterator last)
    {
        tree_.erase(first, last);
    }

    /**
     * @brief 清空容器中的所有元素
     */
    void clear()
    {
        tree_.clear();
    }

    /**
     * @brief multimap 相关操作
     */
    
    /**
     * @brief 查找指定键的元素
     * @param key 要查找的键
     * @return iterator 指向找到的元素的迭代器，如果未找到则返回end()
     */
    iterator find(const key_type& key)
    {
        return tree_.find(key);
    }
    
    /**
     * @brief 查找指定键的元素（常量版本）
     * @param key 要查找的键
     * @return const_iterator 指向找到的元素的常量迭代器，如果未找到则返回end()
     */
    const_iterator find(const key_type& key) const
    {
        return tree_.find(key);
    }

    /**
     * @brief 统计指定键的元素数量
     * @param key 要统计的键
     * @return size_type 元素的数量
     */
    size_type count(const key_type& key) const
    {
        return tree_.count_multi(key);
    }

    /**
     * @brief 获取第一个不小于指定键的元素的迭代器
     * @param key 指定的键
     * @return iterator 第一个不小于key的元素的迭代器
     */
    iterator lower_bound(const key_type& key)
    {
        return tree_.lower_bound(key);
    }
    
    /**
     * @brief 获取第一个不小于指定键的元素的迭代器（常量版本）
     * @param key 指定的键
     * @return const_iterator 第一个不小于key的元素的常量迭代器
     */
    const_iterator lower_bound(const key_type& key) const
    {
        return tree_.lower_bound(key);
    }

    /**
     * @brief 获取第一个大于指定键的元素的迭代器
     * @param key 指定的键
     * @return iterator 第一个大于key的元素的迭代器
     */
    iterator upper_bound(const key_type& key)
    {
        return tree_.upper_bound(key);
    }
    
    /**
     * @brief 获取第一个大于指定键的元素的迭代器（常量版本）
     * @param key 指定的键
     * @return const_iterator 第一个大于key的元素的常量迭代器
     */
    const_iterator upper_bound(const key_type& key) const
    {
        return tree_.upper_bound(key);
    }

    /**
     * @brief 获取指定键的元素范围
     * @param key 指定的键
     * @return std::pair<iterator,iterator> 包含范围的迭代器对
     */
    std::pair<iterator, iterator> equal_range(const key_type& key)
    {
        return tree_.equal_range_multi(key);
    }
    
    /**
     * @brief 获取指定键的元素范围（常量版本）
     * @param key 指定的键
     * @return std::pair<const_iterator,const_iterator> 包含范围的常量迭代器对
     */
    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    {
        return tree_.equal_range_multi(key);
    }

    /**
     * @brief 交换两个multimap的内容
     * @param rhs 要交换的另一个multimap
     */
    void swap(multimap& rhs) noexcept
    {
        tree_.swap(rhs.tree_);
    }

public:
    /**
     * @brief 友元操作符，比较两个multimap是否相等
     */
    friend bool operator==(const multimap& lhs, const multimap& rhs) { return lhs.tree_ == rhs.tree_; }
    
    /**
     * @brief 友元操作符，比较两个multimap的大小关系
     */
    friend bool operator< (const multimap& lhs, const multimap& rhs) { return lhs.tree_ <  rhs.tree_; }
};

/**
 * @brief 重载比较操作符
 */

/**
 * @brief 判断两个multimap是否相等
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧multimap
 * @param rhs 右侧multimap
 * @return bool 如果相等返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator==(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
    return lhs == rhs;
}

/**
 * @brief 判断一个multimap是否小于另一个multimap
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧multimap
 * @param rhs 右侧multimap
 * @return bool 如果lhs小于rhs返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator<(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
    return lhs < rhs;
}

/**
 * @brief 判断两个multimap是否不相等
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧multimap
 * @param rhs 右侧multimap
 * @return bool 如果不相等返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator!=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
    return !(lhs == rhs);
}

/**
 * @brief 判断一个multimap是否大于另一个multimap
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧multimap
 * @param rhs 右侧multimap
 * @return bool 如果lhs大于rhs返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator>(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
    return rhs < lhs;
}

/**
 * @brief 判断一个multimap是否小于等于另一个multimap
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧multimap
 * @param rhs 右侧multimap
 * @return bool 如果lhs小于等于rhs返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator<=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
    return !(rhs < lhs);
}

/**
 * @brief 判断一个multimap是否大于等于另一个multimap
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧multimap
 * @param rhs 右侧multimap
 * @return bool 如果lhs大于等于rhs返回true，否则返回false
 */
template <class Key, class T, class Compare>
bool operator>=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
    return !(lhs < rhs);
}

/**
 * @brief 交换两个multimap的内容
 * @tparam Key 键类型
 * @tparam T 值类型
 * @tparam Compare 比较函数类型
 * @param lhs 左侧multimap
 * @param rhs 右侧multimap
 */
template <class Key, class T, class Compare>
void swap(multimap<Key, T, Compare>& lhs, multimap<Key, T, Compare>& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace my
#endif // !MY_MAP_H_ 