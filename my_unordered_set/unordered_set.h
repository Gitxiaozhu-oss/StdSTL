#ifndef MY_UNORDERED_SET_H_
#define MY_UNORDERED_SET_H_

/**
 * @file unordered_set.h
 * @brief 实现了unordered_set和unordered_multiset容器
 * 
 * 这个头文件包含两个模板类 unordered_set 和 unordered_multiset
 * 功能与用法与 set 和 multiset 类似，不同的是使用 hashtable 作为底层实现机制
 * 容器中的元素不会自动排序
 * 
 * @note 异常保证：
 * unordered_set<Key>/unordered_multiset<Key> 满足基本异常保证
 * 对以下等函数做强异常安全保证：
 *   - emplace
 *   - emplace_hint
 *   - insert
 */

#include "../my_hashtable/my_hashtable.h"
#include <functional>  // 使用std::hash和std::equal_to作为默认参数
#include <initializer_list>
#include <utility>    // 使用std::pair

namespace mystl
{

// 使用std命名空间的函数和类型
using std::hash;
using std::equal_to;
using std::pair;
using std::forward;
using std::move;

// 前置声明
template <class Key, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>> 
class unordered_set;

template <class Key, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>> 
class unordered_multiset;

/**
 * @brief 模板类 unordered_set，键值不允许重复
 * 
 * @tparam Key 键值类型
 * @tparam Hash 哈希函数，缺省使用 std::hash
 * @tparam KeyEqual 键值比较方式，缺省使用 std::equal_to
 */
template <class Key, class Hash, class KeyEqual>
class unordered_set
{
private:
  // 使用 hashtable 作为底层机制
  typedef hashtable<Key, Hash, KeyEqual> base_type;
  base_type ht_;

public:
  // 使用 hashtable 的类型定义
  typedef typename base_type::allocator_type       allocator_type;
  typedef typename base_type::key_type             key_type;
  typedef typename base_type::value_type           value_type;
  typedef typename base_type::hasher               hasher;
  typedef typename base_type::key_equal            key_equal;

  typedef typename base_type::size_type            size_type;
  typedef typename base_type::difference_type      difference_type;
  typedef typename base_type::pointer              pointer;
  typedef typename base_type::const_pointer        const_pointer;
  typedef typename base_type::reference            reference;
  typedef typename base_type::const_reference      const_reference;

  typedef typename base_type::const_iterator       iterator;
  typedef typename base_type::const_iterator       const_iterator;
  typedef typename base_type::const_local_iterator local_iterator;
  typedef typename base_type::const_local_iterator const_local_iterator;

  /**
   * @brief 获取分配器实例
   * @return 分配器
   */
  allocator_type get_allocator() const noexcept { return ht_.get_allocator(); }

  // 构造、复制、移动函数

  /**
   * @brief 默认构造函数
   */
  unordered_set() noexcept
    : ht_(100, Hash(), KeyEqual())
  {
  }

  /**
   * @brief 指定桶数量的构造函数
   * 
   * @param bucket_count 桶数量
   * @param hash 哈希函数
   * @param equal 键值比较函数
   */
  explicit unordered_set(size_type bucket_count,
                         const Hash& hash = Hash(),
                         const KeyEqual& equal = KeyEqual()) noexcept
    : ht_(bucket_count, hash, equal)
  {
  }

  /**
   * @brief 从迭代器范围构造
   * 
   * @tparam InputIterator 输入迭代器类型
   * @param first 范围起始迭代器
   * @param last 范围结束迭代器
   * @param bucket_count 桶数量
   * @param hash 哈希函数
   * @param equal 键值比较函数
   */
  template <class InputIterator>
  unordered_set(InputIterator first, InputIterator last,
                const size_type bucket_count = 100,
                const Hash& hash = Hash(),
                const KeyEqual& equal = KeyEqual())
    : ht_(std::max(bucket_count, static_cast<size_type>(std::distance(first, last))), hash, equal)
  {
    for (; first != last; ++first)
      ht_.insert_unique_noresize(*first);
  }

  /**
   * @brief 从初始化列表构造
   * 
   * @param ilist 初始化列表
   * @param bucket_count 桶数量
   * @param hash 哈希函数
   * @param equal 键值比较函数
   */
  unordered_set(std::initializer_list<value_type> ilist,
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
   * @param rhs 被拷贝的unordered_set
   */
  unordered_set(const unordered_set& rhs)
    : ht_(rhs.ht_)
  {
  }

  /**
   * @brief 移动构造函数
   * 
   * @param rhs 被移动的unordered_set
   */
  unordered_set(unordered_set&& rhs) noexcept
    : ht_(std::move(rhs.ht_))
  {
  }

  /**
   * @brief 拷贝赋值操作符
   * 
   * @param rhs 被拷贝的unordered_set
   * @return 自身引用
   */
  unordered_set& operator=(const unordered_set& rhs)
  {
    ht_ = rhs.ht_;
    return *this;
  }

  /**
   * @brief 移动赋值操作符
   * 
   * @param rhs 被移动的unordered_set
   * @return 自身引用
   */
  unordered_set& operator=(unordered_set&& rhs) noexcept
  {
    ht_ = std::move(rhs.ht_);
    return *this;
  }

  /**
   * @brief 从初始化列表赋值
   * 
   * @param ilist 初始化列表
   * @return 自身引用
   */
  unordered_set& operator=(std::initializer_list<value_type> ilist)
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
  ~unordered_set() = default;

  // 迭代器相关

  /**
   * @brief 返回容器的起始迭代器
   * @return 起始迭代器
   */
  iterator       begin()        noexcept { return ht_.begin(); }
  
  /**
   * @brief 返回容器的起始常量迭代器
   * @return 起始常量迭代器
   */
  const_iterator begin()  const noexcept { return ht_.begin(); }
  
  /**
   * @brief 返回容器的结束迭代器
   * @return 结束迭代器
   */
  iterator       end()          noexcept { return ht_.end(); }
  
  /**
   * @brief 返回容器的结束常量迭代器
   * @return 结束常量迭代器
   */
  const_iterator end()    const noexcept { return ht_.end(); }

  /**
   * @brief 返回容器的起始常量迭代器
   * @return 起始常量迭代器
   */
  const_iterator cbegin() const noexcept { return ht_.cbegin(); }
  
  /**
   * @brief 返回容器的结束常量迭代器
   * @return 结束常量迭代器
   */
  const_iterator cend()   const noexcept { return ht_.cend(); }

  // 容量相关

  /**
   * @brief 检查容器是否为空
   * @return 如果容器为空，返回true，否则返回false
   */
  bool      empty()    const noexcept { return ht_.empty(); }
  
  /**
   * @brief 返回容器中的元素数量
   * @return 元素数量
   */
  size_type size()     const noexcept { return ht_.size(); }
  
  /**
   * @brief 返回容器可以容纳的最大元素数量
   * @return 最大元素数量
   */
  size_type max_size() const noexcept { return ht_.max_size(); }

  // 修改容器操作

  // emplace / emplace_hint

  /**
   * @brief 在容器中构造元素
   * 
   * @tparam Args 参数类型包
   * @param args 构造元素的参数
   * @return 包含插入位置迭代器和是否成功插入的pair
   */
  template <class ...Args>
  pair<iterator, bool> emplace(Args&& ...args)
  { 
    return ht_.emplace_unique(std::forward<Args>(args)...); 
  }

  /**
   * @brief 在指定位置构造元素
   * 
   * @tparam Args 参数类型包
   * @param hint 指定的插入位置
   * @param args 构造元素的参数
   * @return 插入位置的迭代器
   */
  template <class ...Args>
  iterator emplace_hint(const_iterator hint, Args&& ...args)
  { 
    return ht_.emplace_unique_use_hint(hint, std::forward<Args>(args)...); 
  }

  // insert

  /**
   * @brief 插入元素
   * 
   * @param value 要插入的元素值
   * @return 包含插入位置迭代器和是否成功插入的pair
   */
  pair<iterator, bool> insert(const value_type& value)
  { 
    return ht_.insert_unique(value); 
  }
  
  /**
   * @brief 插入右值元素
   * 
   * @param value 要插入的右值元素
   * @return 包含插入位置迭代器和是否成功插入的pair
   */
  pair<iterator, bool> insert(value_type&& value)
  { 
    return ht_.emplace_unique(std::move(value)); 
  }

  /**
   * @brief 在指定位置插入元素
   * 
   * @param hint 插入位置提示
   * @param value 要插入的元素值
   * @return 插入位置的迭代器
   */
  iterator insert(const_iterator hint, const value_type& value)
  { 
    return ht_.insert_unique_use_hint(hint, value); 
  }
  
  /**
   * @brief 在指定位置插入右值元素
   * 
   * @param hint 插入位置提示
   * @param value 要插入的右值元素
   * @return 插入位置的迭代器
   */
  iterator insert(const_iterator hint, value_type&& value)
  { 
    return ht_.emplace_unique_use_hint(hint, std::move(value)); 
  }

  /**
   * @brief 插入迭代器范围内的元素
   * 
   * @tparam InputIterator 输入迭代器类型
   * @param first 范围起始迭代器
   * @param last 范围结束迭代器
   */
  template <class InputIterator>
  void insert(InputIterator first, InputIterator last)
  { 
    ht_.insert_unique(first, last); 
  }

  // erase / clear

  /**
   * @brief 删除迭代器指向的元素
   * 
   * @param it 指向要删除元素的迭代器
   */
  void erase(iterator it)
  { 
    ht_.erase(it); 
  }
  
  /**
   * @brief 删除迭代器范围内的元素
   * 
   * @param first 范围起始迭代器
   * @param last 范围结束迭代器
   */
  void erase(iterator first, iterator last)
  { 
    ht_.erase(first, last); 
  }

  /**
   * @brief 删除指定键的元素
   * 
   * @param key 要删除的键
   * @return 删除的元素数量
   */
  size_type erase(const key_type& key)
  { 
    return ht_.erase_unique(key); 
  }

  /**
   * @brief 清空容器
   */
  void clear()
  { 
    ht_.clear(); 
  }

  /**
   * @brief 与另一个unordered_set交换内容
   * 
   * @param other 要交换的unordered_set
   */
  void swap(unordered_set& other) noexcept
  { 
    ht_.swap(other.ht_); 
  }

  // 查找相关

  /**
   * @brief 计算指定键的元素数量
   * 
   * @param key 要查找的键
   * @return 元素数量
   */
  size_type count(const key_type& key) const 
  { 
    return ht_.count(key); 
  }

  /**
   * @brief 查找指定键的元素
   * 
   * @param key 要查找的键
   * @return 指向找到元素的迭代器，如果未找到则返回end()
   */
  iterator find(const key_type& key) 
  { 
    return ht_.find(key); 
  }
  
  /**
   * @brief 查找指定键的元素（常量版本）
   * 
   * @param key 要查找的键
   * @return 指向找到元素的常量迭代器，如果未找到则返回end()
   */
  const_iterator find(const key_type& key) const 
  { 
    return ht_.find(key); 
  }

  /**
   * @brief 返回指定键的元素范围
   * 
   * @param key 要查找的键
   * @return 一对迭代器，表示元素范围
   */
  pair<iterator, iterator> equal_range(const key_type& key)
  { 
    return ht_.equal_range_unique(key); 
  }
  
  /**
   * @brief 返回指定键的元素范围（常量版本）
   * 
   * @param key 要查找的键
   * @return 一对常量迭代器,表示元素范围
   */
  pair<const_iterator, const_iterator> equal_range(const key_type& key) const
  { 
    return ht_.equal_range_unique(key); 
  }

  // bucket 接口

  /**
   * @brief 返回指定桶的起始迭代器
   * 
   * @param n 桶的索引
   * @return 桶的起始局部迭代器
   */
  local_iterator begin(size_type n) noexcept
  { 
    return ht_.begin(n); 
  }
  
  /**
   * @brief 返回指定桶的起始常量迭代器
   * 
   * @param n 桶的索引
   * @return 桶的起始常量局部迭代器
   */
  const_local_iterator begin(size_type n) const noexcept
  { 
    return ht_.begin(n); 
  }
  
  /**
   * @brief 返回指定桶的起始常量迭代器
   * 
   * @param n 桶的索引
   * @return 桶的起始常量局部迭代器
   */
  const_local_iterator cbegin(size_type n) const noexcept
  { 
    return ht_.cbegin(n); 
  }

  /**
   * @brief 返回指定桶的结束迭代器
   * 
   * @param n 桶的索引
   * @return 桶的结束局部迭代器
   */
  local_iterator end(size_type n) noexcept
  { 
    return ht_.end(n); 
  }
  
  /**
   * @brief 返回指定桶的结束常量迭代器
   * 
   * @param n 桶的索引
   * @return 桶的结束常量局部迭代器
   */
  const_local_iterator end(size_type n) const noexcept
  { 
    return ht_.end(n); 
  }
  
  /**
   * @brief 返回指定桶的结束常量迭代器
   * 
   * @param n 桶的索引
   * @return 桶的结束常量局部迭代器
   */
  const_local_iterator cend(size_type n) const noexcept
  { 
    return ht_.cend(n); 
  }

  /**
   * @brief 返回桶的数量
   * 
   * @return 桶的数量
   */
  size_type bucket_count() const noexcept
  { 
    return ht_.bucket_count(); 
  }
  
  /**
   * @brief 返回容器可以有的最大桶数
   * 
   * @return 最大桶数
   */
  size_type max_bucket_count() const noexcept
  { 
    return ht_.max_bucket_count(); 
  }

  /**
   * @brief 返回指定桶中的元素数量
   * 
   * @param n 桶的索引
   * @return 元素数量
   */
  size_type bucket_size(size_type n) const noexcept
  { 
    return ht_.bucket_size(n); 
  }
  
  /**
   * @brief 返回指定键所在的桶的索引
   * 
   * @param key 要查找的键
   * @return 桶的索引
   */
  size_type bucket(const key_type& key) const
  { 
    return ht_.bucket(key); 
  }

  // hash policy

  /**
   * @brief 返回当前的负载因子
   * 
   * @return 负载因子
   */
  float load_factor() const noexcept 
  { 
    return ht_.load_factor(); 
  }

  /**
   * @brief 返回当前的最大负载因子
   * 
   * @return 最大负载因子
   */
  float max_load_factor() const noexcept 
  { 
    return ht_.max_load_factor(); 
  }
  
  /**
   * @brief 设置最大负载因子
   * 
   * @param ml 新的最大负载因子
   */
  void max_load_factor(float ml) 
  { 
    ht_.max_load_factor(ml); 
  }

  /**
   * @brief 重新散列，改变桶的数量
   * 
   * @param count 新的桶数量
   */
  void rehash(size_type count) 
  { 
    ht_.rehash(count); 
  }
  
  /**
   * @brief 预留存储空间
   * 
   * @param count 预期的元素数量
   */
  void reserve(size_type count) 
  { 
    ht_.reserve(count); 
  }

  /**
   * @brief 获取哈希函数
   * 
   * @return 哈希函数
   */
  hasher hash_fcn() const 
  { 
    return ht_.hash_fcn(); 
  }
  
  /**
   * @brief 获取键值比较函数
   * 
   * @return 键值比较函数
   */
  key_equal key_eq() const 
  { 
    return ht_.key_eq(); 
  }

public:
  /**
   * @brief 重载相等运算符
   * 
   * @tparam Key 键值类型
   * @tparam Hash 哈希函数类型
   * @tparam KeyEqual 键值比较函数类型
   * @param lhs 左侧操作数
   * @param rhs 右侧操作数
   * @return 如果两个unordered_set相等，返回true，否则返回false
   */
  friend bool operator==(const unordered_set& lhs, const unordered_set& rhs)
  {
    if (lhs.size() != rhs.size())
      return false;
    
    // 检查每个元素是否都在另一个集合中
    for (auto& elem : lhs)
    {
      if (rhs.find(elem) == rhs.end())
        return false;
    }
    return true;
  }
  
  /**
   * @brief 重载不等运算符
   * 
   * @tparam Key 键值类型
   * @tparam Hash 哈希函数类型
   * @tparam KeyEqual 键值比较函数类型
   * @param lhs 左侧操作数
   * @param rhs 右侧操作数
   * @return 如果两个unordered_set不相等，返回true，否则返回false
   */
  friend bool operator!=(const unordered_set& lhs, const unordered_set& rhs)
  {
    return !(lhs == rhs);
  }
};

// 全局运算符重载

/**
 * @brief 重载相等运算符
 * 
 * @tparam Key 键值类型
 * @tparam Hash 哈希函数类型
 * @tparam KeyEqual 键值比较函数类型
 * @param lhs 左侧操作数
 * @param rhs 右侧操作数
 * @return 如果两个unordered_set相等，返回true，否则返回false
 */
template <class Key, class Hash, class KeyEqual>
bool operator==(const unordered_set<Key, Hash, KeyEqual>& lhs,
                const unordered_set<Key, Hash, KeyEqual>& rhs)
{
  if (lhs.size() != rhs.size())
    return false;
  
  // 检查每个元素是否都在另一个集合中
  for (auto& elem : lhs)
  {
    if (rhs.find(elem) == rhs.end())
      return false;
  }
  return true;
}

/**
 * @brief 重载不等运算符
 * 
 * @tparam Key 键值类型
 * @tparam Hash 哈希函数类型
 * @tparam KeyEqual 键值比较函数类型
 * @param lhs 左侧操作数
 * @param rhs 右侧操作数
 * @return 如果两个unordered_set不相等，返回true，否则返回false
 */
template <class Key, class Hash, class KeyEqual>
bool operator!=(const unordered_set<Key, Hash, KeyEqual>& lhs,
                const unordered_set<Key, Hash, KeyEqual>& rhs)
{
  return !(lhs == rhs);
}

/**
 * @brief 重载mystl的swap
 * 
 * @tparam Key 键值类型
 * @tparam Hash 哈希函数类型
 * @tparam KeyEqual 键值比较函数类型
 * @param lhs 左侧操作数
 * @param rhs 右侧操作数
 */
template <class Key, class Hash, class KeyEqual>
void swap(unordered_set<Key, Hash, KeyEqual>& lhs,
          unordered_set<Key, Hash, KeyEqual>& rhs) noexcept
{
  lhs.swap(rhs);
}

/*****************************************************************************************/

/**
 * @brief 模板类 unordered_multiset，键值允许重复
 * 
 * @tparam Key 键值类型
 * @tparam Hash 哈希函数，缺省使用 std::hash
 * @tparam KeyEqual 键值比较方式，缺省使用 std::equal_to
 */
template <class Key, class Hash, class KeyEqual>
class unordered_multiset
{
private:
  // 使用 hashtable 作为底层机制
  typedef hashtable<Key, Hash, KeyEqual> base_type;
  base_type ht_;

public:
  // 使用 hashtable 的类型定义
  typedef typename base_type::allocator_type       allocator_type;
  typedef typename base_type::key_type             key_type;
  typedef typename base_type::value_type           value_type;
  typedef typename base_type::hasher               hasher;
  typedef typename base_type::key_equal            key_equal;

  typedef typename base_type::size_type            size_type;
  typedef typename base_type::difference_type      difference_type;
  typedef typename base_type::pointer              pointer;
  typedef typename base_type::const_pointer        const_pointer;
  typedef typename base_type::reference            reference;
  typedef typename base_type::const_reference      const_reference;

  typedef typename base_type::const_iterator       iterator;
  typedef typename base_type::const_iterator       const_iterator;
  typedef typename base_type::const_local_iterator local_iterator;
  typedef typename base_type::const_local_iterator const_local_iterator;

  /**
   * @brief 获取分配器实例
   * @return 分配器
   */
  allocator_type get_allocator() const noexcept { return ht_.get_allocator(); }

public:
  // 构造、复制、移动函数

  /**
   * @brief 默认构造函数
   */
  unordered_multiset() noexcept
    : ht_(100, Hash(), KeyEqual())
  {
  }

  /**
   * @brief 指定桶数量的构造函数
   * 
   * @param bucket_count 桶数量
   * @param hash 哈希函数
   * @param equal 键值比较函数
   */
  explicit unordered_multiset(size_type bucket_count,
                              const Hash& hash = Hash(),
                              const KeyEqual& equal = KeyEqual()) noexcept
    : ht_(bucket_count, hash, equal)
  {
  }

  /**
   * @brief 从迭代器范围构造
   * 
   * @tparam InputIterator 输入迭代器类型
   * @param first 范围起始迭代器
   * @param last 范围结束迭代器
   * @param bucket_count 桶数量
   * @param hash 哈希函数
   * @param equal 键值比较函数
   */
  template <class InputIterator>
  unordered_multiset(InputIterator first, InputIterator last,
                     const size_type bucket_count = 100,
                     const Hash& hash = Hash(),
                     const KeyEqual& equal = KeyEqual())
    : ht_(std::max(bucket_count, static_cast<size_type>(std::distance(first, last))), hash, equal)
  {
    for (; first != last; ++first)
      ht_.insert_multi_noresize(*first);
  }

  /**
   * @brief 从初始化列表构造
   * 
   * @param ilist 初始化列表
   * @param bucket_count 桶数量
   * @param hash 哈希函数
   * @param equal 键值比较函数
   */
  unordered_multiset(std::initializer_list<value_type> ilist,
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
   * @param rhs 被拷贝的unordered_multiset
   */
  unordered_multiset(const unordered_multiset& rhs)
    : ht_(rhs.ht_)
  {
  }

  /**
   * @brief 移动构造函数
   * 
   * @param rhs 被移动的unordered_multiset
   */
  unordered_multiset(unordered_multiset&& rhs) noexcept
    : ht_(std::move(rhs.ht_))
  {
  }

  /**
   * @brief 拷贝赋值操作符
   * 
   * @param rhs 被拷贝的unordered_multiset
   * @return 自身引用
   */
  unordered_multiset& operator=(const unordered_multiset& rhs)
  {
    ht_ = rhs.ht_;
    return *this;
  }

  /**
   * @brief 移动赋值操作符
   * 
   * @param rhs 被移动的unordered_multiset
   * @return 自身引用
   */
  unordered_multiset& operator=(unordered_multiset&& rhs) noexcept
  {
    ht_ = std::move(rhs.ht_);
    return *this;
  }

  /**
   * @brief 从初始化列表赋值
   * 
   * @param ilist 初始化列表
   * @return 自身引用
   */
  unordered_multiset& operator=(std::initializer_list<value_type> ilist)
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
  ~unordered_multiset() = default;

  // 迭代器相关

  /**
   * @brief 返回容器的起始迭代器
   * @return 起始迭代器
   */
  iterator       begin()        noexcept { return ht_.begin(); }
  
  /**
   * @brief 返回容器的起始常量迭代器
   * @return 起始常量迭代器
   */
  const_iterator begin()  const noexcept { return ht_.begin(); }
  
  /**
   * @brief 返回容器的结束迭代器
   * @return 结束迭代器
   */
  iterator       end()          noexcept { return ht_.end(); }
  
  /**
   * @brief 返回容器的结束常量迭代器
   * @return 结束常量迭代器
   */
  const_iterator end()    const noexcept { return ht_.end(); }

  /**
   * @brief 返回容器的起始常量迭代器
   * @return 起始常量迭代器
   */
  const_iterator cbegin() const noexcept { return ht_.cbegin(); }
  
  /**
   * @brief 返回容器的结束常量迭代器
   * @return 结束常量迭代器
   */
  const_iterator cend()   const noexcept { return ht_.cend(); }

  // 容量相关

  /**
   * @brief 检查容器是否为空
   * @return 如果容器为空，返回true，否则返回false
   */
  bool      empty()    const noexcept { return ht_.empty(); }
  
  /**
   * @brief 返回容器中的元素数量
   * @return 元素数量
   */
  size_type size()     const noexcept { return ht_.size(); }
  
  /**
   * @brief 返回容器可以容纳的最大元素数量
   * @return 最大元素数量
   */
  size_type max_size() const noexcept { return ht_.max_size(); }

  // 修改容器相关

  // emplace / emplace_hint

  /**
   * @brief 在容器中构造元素
   * 
   * @tparam Args 参数类型包
   * @param args 构造元素的参数
   * @return 指向新元素的迭代器
   */
  template <class ...Args>
  iterator emplace(Args&& ...args)
  { 
    return ht_.emplace_multi(std::forward<Args>(args)...); 
  }

  /**
   * @brief 在指定位置构造元素
   * 
   * @tparam Args 参数类型包
   * @param hint 指定的插入位置
   * @param args 构造元素的参数
   * @return 指向新元素的迭代器
   */
  template <class ...Args>
  iterator emplace_hint(const_iterator hint, Args&& ...args)
  { 
    return ht_.emplace_multi_use_hint(hint, std::forward<Args>(args)...); 
  }

  // insert

  /**
   * @brief 插入元素
   * 
   * @param value 要插入的元素值
   * @return 指向新元素的迭代器
   */
  iterator insert(const value_type& value)
  { 
    return ht_.insert_multi(value); 
  }
  
  /**
   * @brief 插入右值元素
   * 
   * @param value 要插入的右值元素
   * @return 指向新元素的迭代器
   */
  iterator insert(value_type&& value)
  { 
    return ht_.emplace_multi(std::move(value)); 
  }

  /**
   * @brief 在指定位置插入元素
   * 
   * @param hint 插入位置提示
   * @param value 要插入的元素值
   * @return 指向新元素的迭代器
   */
  iterator insert(const_iterator hint, const value_type& value)
  { 
    return ht_.insert_multi_use_hint(hint, value); 
  }
  
  /**
   * @brief 在指定位置插入右值元素
   * 
   * @param hint 插入位置提示
   * @param value 要插入的右值元素
   * @return 指向新元素的迭代器
   */
  iterator insert(const_iterator hint, value_type&& value)
  { 
    return ht_.emplace_multi_use_hint(hint, std::move(value)); 
  }

  /**
   * @brief 插入迭代器范围内的元素
   * 
   * @tparam InputIterator 输入迭代器类型
   * @param first 范围起始迭代器
   * @param last 范围结束迭代器
   */
  template <class InputIterator>
  void insert(InputIterator first, InputIterator last)
  { 
    ht_.insert_multi(first, last); 
  }

  // erase / clear

  /**
   * @brief 删除迭代器指向的元素
   * 
   * @param it 指向要删除元素的迭代器
   */
  void erase(iterator it)
  { 
    ht_.erase(it); 
  }
  
  /**
   * @brief 删除迭代器范围内的元素
   * 
   * @param first 范围起始迭代器
   * @param last 范围结束迭代器
   */
  void erase(iterator first, iterator last)
  { 
    ht_.erase(first, last); 
  }

  /**
   * @brief 删除指定键的所有元素
   * 
   * @param key 要删除的键
   * @return 删除的元素数量
   */
  size_type erase(const key_type& key)
  { 
    return ht_.erase_multi(key); 
  }

  /**
   * @brief 清空容器
   */
  void clear()
  { 
    ht_.clear(); 
  }

  /**
   * @brief 与另一个unordered_multiset交换内容
   * 
   * @param other 要交换的unordered_multiset
   */
  void swap(unordered_multiset& other) noexcept
  { 
    ht_.swap(other.ht_); 
  }

  // 查找相关

  /**
   * @brief 计算指定键的元素数量
   * 
   * @param key 要查找的键
   * @return 元素数量
   */
  size_type count(const key_type& key) const 
  { 
    return ht_.count(key); 
  }

  /**
   * @brief 查找指定键的元素
   * 
   * @param key 要查找的键
   * @return 指向找到元素的迭代器，如果未找到则返回end()
   */
  iterator find(const key_type& key) 
  { 
    return ht_.find(key); 
  }
  
  /**
   * @brief 查找指定键的元素（常量版本）
   * 
   * @param key 要查找的键
   * @return 指向找到元素的常量迭代器，如果未找到则返回end()
   */
  const_iterator find(const key_type& key) const 
  { 
    return ht_.find(key); 
  }

  /**
   * @brief 返回指定键的元素范围
   * 
   * @param key 要查找的键
   * @return 一对迭代器，表示元素范围
   */
  pair<iterator, iterator> equal_range(const key_type& key)
  { 
    return ht_.equal_range_multi(key); 
  }
  
  /**
   * @brief 返回指定键的元素范围（常量版本）
   * 
   * @param key 要查找的键
   * @return 一对常量迭代器，表示元素范围
   */
  pair<const_iterator, const_iterator> equal_range(const key_type& key) const
  { 
    return ht_.equal_range_multi(key); 
  }

  // bucket 接口

  /**
   * @brief 返回指定桶的起始迭代器
   * 
   * @param n 桶的索引
   * @return 桶的起始局部迭代器
   */
  local_iterator begin(size_type n) noexcept
  { 
    return ht_.begin(n); 
  }
  
  /**
   * @brief 返回指定桶的起始常量迭代器
   * 
   * @param n 桶的索引
   * @return 桶的起始常量局部迭代器
   */
  const_local_iterator begin(size_type n) const noexcept
  { 
    return ht_.begin(n); 
  }
  
  /**
   * @brief 返回指定桶的起始常量迭代器
   * 
   * @param n 桶的索引
   * @return 桶的起始常量局部迭代器
   */
  const_local_iterator cbegin(size_type n) const noexcept
  { 
    return ht_.cbegin(n); 
  }

  /**
   * @brief 返回指定桶的结束迭代器
   * 
   * @param n 桶的索引
   * @return 桶的结束局部迭代器
   */
  local_iterator end(size_type n) noexcept
  { 
    return ht_.end(n); 
  }
  
  /**
   * @brief 返回指定桶的结束常量迭代器
   * 
   * @param n 桶的索引
   * @return 桶的结束常量局部迭代器
   */
  const_local_iterator end(size_type n) const noexcept
  { 
    return ht_.end(n); 
  }
  
  /**
   * @brief 返回指定桶的结束常量迭代器
   * 
   * @param n 桶的索引
   * @return 桶的结束常量局部迭代器
   */
  const_local_iterator cend(size_type n) const noexcept
  { 
    return ht_.cend(n); 
  }

  /**
   * @brief 返回桶的数量
   * 
   * @return 桶的数量
   */
  size_type bucket_count() const noexcept
  { 
    return ht_.bucket_count(); 
  }
  
  /**
   * @brief 返回容器可以有的最大桶数
   * 
   * @return 最大桶数
   */
  size_type max_bucket_count() const noexcept
  { 
    return ht_.max_bucket_count(); 
  }

  /**
   * @brief 返回指定桶中的元素数量
   * 
   * @param n 桶的索引
   * @return 元素数量
   */
  size_type bucket_size(size_type n) const noexcept
  { 
    return ht_.bucket_size(n); 
  }
  
  /**
   * @brief 返回指定键所在的桶的索引
   * 
   * @param key 要查找的键
   * @return 桶的索引
   */
  size_type bucket(const key_type& key) const
  { 
    return ht_.bucket(key); 
  }

  // hash policy

  /**
   * @brief 返回当前的负载因子
   * 
   * @return 负载因子
   */
  float load_factor() const noexcept 
  { 
    return ht_.load_factor(); 
  }

  /**
   * @brief 返回当前的最大负载因子
   * 
   * @return 最大负载因子
   */
  float max_load_factor() const noexcept 
  { 
    return ht_.max_load_factor(); 
  }
  
  /**
   * @brief 设置最大负载因子
   * 
   * @param ml 新的最大负载因子
   */
  void max_load_factor(float ml) 
  { 
    ht_.max_load_factor(ml); 
  }

  /**
   * @brief 重新散列，改变桶的数量
   * 
   * @param count 新的桶数量
   */
  void rehash(size_type count) 
  { 
    ht_.rehash(count); 
  }
  
  /**
   * @brief 预留存储空间
   * 
   * @param count 预期的元素数量
   */
  void reserve(size_type count) 
  { 
    ht_.reserve(count); 
  }

  /**
   * @brief 获取哈希函数
   * 
   * @return 哈希函数
   */
  hasher hash_fcn() const 
  { 
    return ht_.hash_fcn(); 
  }
  
  /**
   * @brief 获取键值比较函数
   * 
   * @return 键值比较函数
   */
  key_equal key_eq() const 
  { 
    return ht_.key_eq(); 
  }

public:
  /**
   * @brief 重载相等运算符
   * 
   * @tparam Key 键值类型
   * @tparam Hash 哈希函数类型
   * @tparam KeyEqual 键值比较函数类型
   * @param lhs 左侧操作数
   * @param rhs 右侧操作数
   * @return 如果两个unordered_multiset相等，返回true，否则返回false
   */
  friend bool operator==(const unordered_multiset& lhs, const unordered_multiset& rhs)
  {
    if (lhs.size() != rhs.size())
      return false;
    
    // 对于每个键，检查其在两个容器中的出现次数是否相同
    for (auto& elem : lhs)
    {
      auto count1 = lhs.count(elem);
      auto count2 = rhs.count(elem);
      if (count1 != count2)
        return false;
    }
    return true;
  }
  
  /**
   * @brief 重载不等运算符
   * 
   * @tparam Key 键值类型
   * @tparam Hash 哈希函数类型
   * @tparam KeyEqual 键值比较函数类型
   * @param lhs 左侧操作数
   * @param rhs 右侧操作数
   * @return 如果两个unordered_multiset不相等，返回true，否则返回false
   */
  friend bool operator!=(const unordered_multiset& lhs, const unordered_multiset& rhs)
  {
    return !(lhs == rhs);
  }
};

// 全局运算符重载

/**
 * @brief 重载相等运算符
 * 
 * @tparam Key 键值类型
 * @tparam Hash 哈希函数类型
 * @tparam KeyEqual 键值比较函数类型
 * @param lhs 左侧操作数
 * @param rhs 右侧操作数
 * @return 如果两个unordered_multiset相等，返回true，否则返回false
 */
template <class Key, class Hash, class KeyEqual>
bool operator==(const unordered_multiset<Key, Hash, KeyEqual>& lhs,
                const unordered_multiset<Key, Hash, KeyEqual>& rhs)
{
  if (lhs.size() != rhs.size())
    return false;
  
  // 对于每个键，检查其在两个容器中的出现次数是否相同
  for (auto& elem : lhs)
  {
    auto count1 = lhs.count(elem);
    auto count2 = rhs.count(elem);
    if (count1 != count2)
      return false;
  }
  return true;
}

/**
 * @brief 重载不等运算符
 * 
 * @tparam Key 键值类型
 * @tparam Hash 哈希函数类型
 * @tparam KeyEqual 键值比较函数类型
 * @param lhs 左侧操作数
 * @param rhs 右侧操作数
 * @return 如果两个unordered_multiset不相等，返回true，否则返回false
 */
template <class Key, class Hash, class KeyEqual>
bool operator!=(const unordered_multiset<Key, Hash, KeyEqual>& lhs,
                const unordered_multiset<Key, Hash, KeyEqual>& rhs)
{
  return !(lhs == rhs);
}

/**
 * @brief 重载mystl的swap
 * 
 * @tparam Key 键值类型
 * @tparam Hash 哈希函数类型
 * @tparam KeyEqual 键值比较函数类型
 * @param lhs 左侧操作数
 * @param rhs 右侧操作数
 */
template <class Key, class Hash, class KeyEqual>
void swap(unordered_multiset<Key, Hash, KeyEqual>& lhs,
          unordered_multiset<Key, Hash, KeyEqual>& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace mystl

#endif // MY_UNORDERED_SET_H_ 