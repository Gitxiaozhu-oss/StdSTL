#ifndef MY_HASHTABLE_H
#define MY_HASHTABLE_H

// 这个头文件包含了一个模板类 hashtable
// hashtable : 哈希表，使用开链法处理冲突

#include <initializer_list>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <iterator>  // 添加iterator头文件，提供迭代器标签

#include "../my_vector/my_vector.h"

namespace mystl
{

/**
 * @brief 哈希表节点类
 * 
 * @tparam T 节点存储的数据类型
 */
template <class T>
struct hashtable_node
{
    hashtable_node* next;   // 指向下一个节点
    T               value;  // 存储实值

    hashtable_node() = default;
    
    /**
     * @brief 构造函数，传入值
     * @param n 节点存储的值
     */
    hashtable_node(const T& n) : next(nullptr), value(n) {}

    /**
     * @brief 拷贝构造函数
     * @param node 源节点
     */
    hashtable_node(const hashtable_node& node) : next(node.next), value(node.value) {}
    
    /**
     * @brief 移动构造函数
     * @param node 源节点
     */
    hashtable_node(hashtable_node&& node) : next(node.next), value(std::move(node.value))
    {
        node.next = nullptr;
    }
};

/**
 * @brief value traits 实现类 - 基本类型情况
 * 用于处理值类型和获取键值
 * 
 * @tparam T 值类型
 * @tparam isMap 是否为映射类型（键值对）
 */
template <class T, bool>
struct ht_value_traits_imp
{
    typedef T key_type;      // 键类型
    typedef T mapped_type;   // 映射类型
    typedef T value_type;    // 值类型

    /**
     * @brief 获取键
     * @param value 值
     * @return 键
     */
    template <class Ty>
    static const key_type& get_key(const Ty& value)
    {
        return value;
    }

    /**
     * @brief 获取值
     * @param value 值
     * @return 值
     */
    template <class Ty>
    static const value_type& get_value(const Ty& value)
    {
        return value;
    }
};

/**
 * @brief value traits 实现类 - 键值对类型情况
 * 
 * @tparam T 键值对类型
 */
template <class T>
struct ht_value_traits_imp<T, true>
{
    typedef typename std::remove_cv<typename T::first_type>::type key_type;     // 键类型
    typedef typename T::second_type                               mapped_type;  // 映射类型  
    typedef T                                                     value_type;   // 值类型

    /**
     * @brief 获取键
     * @param value 键值对
     * @return 键
     */
    template <class Ty>
    static const key_type& get_key(const Ty& value)
    {
        return value.first;
    }

    /**
     * @brief 获取值
     * @param value 键值对
     * @return 值
     */
    template <class Ty>
    static const value_type& get_value(const Ty& value)
    {
        return value;
    }
};

/**
 * @brief 是否为键值对类型
 * 
 * @tparam T 要检测的类型
 */
template <class T>
struct is_pair : public std::false_type {};

/**
 * @brief 键值对类型特化
 * 
 * @tparam T1 键类型
 * @tparam T2 值类型
 */
template <class T1, class T2>
struct is_pair<std::pair<T1, T2>> : public std::true_type {};

/**
 * @brief 哈希表值特性类
 * 
 * @tparam T 值类型
 */
template <class T>
struct ht_value_traits
{
    static constexpr bool is_map = is_pair<T>::value;  // 是否为映射类型

    typedef ht_value_traits_imp<T, is_map> value_traits_type;  // 值特性类型

    typedef typename value_traits_type::key_type    key_type;     // 键类型
    typedef typename value_traits_type::mapped_type mapped_type;  // 映射类型
    typedef typename value_traits_type::value_type  value_type;   // 值类型

    /**
     * @brief 获取键
     * @param value 值
     * @return 键
     */
    template <class Ty>
    static const key_type& get_key(const Ty& value)
    {
        return value_traits_type::get_key(value);
    }

    /**
     * @brief 获取值
     * @param value 值
     * @return 值
     */
    template <class Ty>
    static const value_type& get_value(const Ty& value)
    {
        return value_traits_type::get_value(value);
    }
};

// 前向声明
template <class T, class HashFun, class KeyEqual>
class hashtable;

template <class T, class HashFun, class KeyEqual>
struct ht_iterator;

template <class T, class HashFun, class KeyEqual>
struct ht_const_iterator;

template <class T>
struct ht_local_iterator;

template <class T>
struct ht_const_local_iterator;

/**
 * @brief 迭代器标签
 */
using input_iterator_tag = std::input_iterator_tag;
using output_iterator_tag = std::output_iterator_tag;
using forward_iterator_tag = std::forward_iterator_tag;
using bidirectional_iterator_tag = std::bidirectional_iterator_tag;
using random_access_iterator_tag = std::random_access_iterator_tag;

/**
 * @brief 迭代器模板
 */
template <class Category, class T, class Distance = ptrdiff_t,
         class Pointer = T*, class Reference = T&>
struct iterator
{
    typedef Category    iterator_category;  // 迭代器类型
    typedef T           value_type;         // 值类型
    typedef Distance    difference_type;    // 距离类型
    typedef Pointer     pointer;            // 指针类型
    typedef Reference   reference;          // 引用类型
};

/**
 * @brief 哈希表迭代器基类
 * 
 * @tparam T 值类型
 * @tparam Hash 哈希函数类型
 * @tparam KeyEqual 键相等比较函数类型
 */
template <class T, class Hash, class KeyEqual>
struct ht_iterator_base : public iterator<forward_iterator_tag, T>
{
    typedef mystl::hashtable<T, Hash, KeyEqual>         hashtable;
    typedef ht_iterator_base<T, Hash, KeyEqual>         base;
    typedef mystl::ht_iterator<T, Hash, KeyEqual>       iterator;
    typedef mystl::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
    typedef hashtable_node<T>*                          node_ptr;
    typedef hashtable*                                  contain_ptr;
    typedef const node_ptr                              const_node_ptr;
    typedef const contain_ptr                           const_contain_ptr;

    typedef size_t                                      size_type;
    typedef ptrdiff_t                                   difference_type;

    node_ptr    node;  // 迭代器当前所指节点
    contain_ptr ht;    // 保持与容器的连结

    ht_iterator_base() = default;

    /**
     * @brief 相等比较操作符
     * @param rhs 右侧操作数
     * @return 是否相等
     */
    bool operator==(const base& rhs) const { return node == rhs.node; }
    
    /**
     * @brief 不等比较操作符
     * @param rhs 右侧操作数
     * @return 是否不等
     */
    bool operator!=(const base& rhs) const { return node != rhs.node; }
};

/**
 * @brief 哈希表迭代器
 * 
 * @tparam T 值类型
 * @tparam Hash 哈希函数类型
 * @tparam KeyEqual 键相等比较函数类型
 */
template <class T, class Hash, class KeyEqual>
struct ht_iterator : public ht_iterator_base<T, Hash, KeyEqual>
{
    typedef ht_iterator_base<T, Hash, KeyEqual> base;
    typedef typename base::hashtable            hashtable;
    typedef typename base::iterator             iterator;
    typedef typename base::const_iterator       const_iterator;
    typedef typename base::node_ptr             node_ptr;
    typedef typename base::contain_ptr          contain_ptr;

    typedef ht_value_traits<T>                  value_traits;
    typedef T                                   value_type;
    typedef value_type*                         pointer;
    typedef value_type&                         reference;

    using base::node;
    using base::ht;

    ht_iterator() = default;
    
    /**
     * @brief 构造函数
     * @param n 节点指针
     * @param t 哈希表指针
     */
    ht_iterator(node_ptr n, contain_ptr t)
    {
        node = n;
        ht = t;
    }
    
    /**
     * @brief 拷贝构造函数
     * @param rhs 右侧操作数
     */
    ht_iterator(const iterator& rhs)
    {
        node = rhs.node;
        ht = rhs.ht;
    }
    
    /**
     * @brief 从const_iterator构造
     * @param rhs 右侧操作数
     */
    ht_iterator(const const_iterator& rhs)
    {
        node = rhs.node;
        ht = rhs.ht;
    }
    
    /**
     * @brief 赋值运算符
     * @param rhs 右侧操作数
     * @return 引用
     */
    iterator& operator=(const iterator& rhs)
    {
        if (this != &rhs)
        {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }
    
    /**
     * @brief 从const_iterator赋值
     * @param rhs 右侧操作数
     * @return 引用
     */
    iterator& operator=(const const_iterator& rhs)
    {
        if (this != &rhs)
        {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }

    // 重载操作符
    /**
     * @brief 解引用操作符
     * @return 引用
     */
    reference operator*() const { return node->value; }
    
    /**
     * @brief 成员访问操作符
     * @return 指针
     */
    pointer operator->() const { return &(operator*()); }

    /**
     * @brief 前置递增操作符
     * @return 引用
     */
    iterator& operator++()
    {
        node_ptr old = node;
        node = node->next;
        if (node == nullptr)
        { // 如果下一个位置为空，跳到下一个 bucket 的起始处
            auto index = ht->hash(value_traits::get_key(old->value));
            while (!node && ++index < ht->bucket_size_)
                node = ht->buckets_[index];
        }
        return *this;
    }
    
    /**
     * @brief 后置递增操作符
     * @return 副本
     */
    iterator operator++(int)
    {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
};

/**
 * @brief 哈希表常量迭代器
 * 
 * @tparam T 值类型
 * @tparam Hash 哈希函数类型
 * @tparam KeyEqual 键相等比较函数类型
 */
template <class T, class Hash, class KeyEqual>
struct ht_const_iterator : public ht_iterator_base<T, Hash, KeyEqual>
{
    typedef ht_iterator_base<T, Hash, KeyEqual> base;
    typedef typename base::hashtable            hashtable;
    typedef typename base::iterator             iterator;
    typedef typename base::const_iterator       const_iterator;
    typedef typename base::const_node_ptr       node_ptr;
    typedef typename base::const_contain_ptr    contain_ptr;

    typedef ht_value_traits<T>                  value_traits;
    typedef T                                   value_type;
    typedef const value_type*                   pointer;
    typedef const value_type&                   reference;

    using base::node;
    using base::ht;

    ht_const_iterator() = default;
    
    /**
     * @brief 构造函数
     * @param n 节点指针
     * @param t 哈希表指针
     */
    ht_const_iterator(node_ptr n, contain_ptr t)
    {
        node = n;
        ht = t;
    }
    
    /**
     * @brief 从iterator构造
     * @param rhs 右侧操作数
     */
    ht_const_iterator(const iterator& rhs)
    {
        node = rhs.node;
        ht = rhs.ht;
    }
    
    /**
     * @brief 拷贝构造函数
     * @param rhs 右侧操作数
     */
    ht_const_iterator(const const_iterator& rhs)
    {
        node = rhs.node;
        ht = rhs.ht;
    }
    
    /**
     * @brief 从iterator赋值
     * @param rhs 右侧操作数
     * @return 引用
     */
    const_iterator& operator=(const iterator& rhs)
    {
        if (this != &rhs)
        {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }
    
    /**
     * @brief 赋值运算符
     * @param rhs 右侧操作数
     * @return 引用
     */
    const_iterator& operator=(const const_iterator& rhs)
    {
        if (this != &rhs)
        {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }

    // 重载操作符
    /**
     * @brief 解引用操作符
     * @return 引用
     */
    reference operator*() const { return node->value; }
    
    /**
     * @brief 成员访问操作符
     * @return 指针
     */
    pointer operator->() const { return &(operator*()); }

    /**
     * @brief 前置递增操作符
     * @return 引用
     */
    const_iterator& operator++()
    {
        node_ptr old = node;
        node = node->next;
        if (node == nullptr)
        { // 如果下一个位置为空，跳到下一个 bucket 的起始处
            auto index = ht->hash(value_traits::get_key(old->value));
            while (!node && ++index < ht->bucket_size_)
            {
                node = ht->buckets_[index];
            }
        }
        return *this;
    }
    
    /**
     * @brief 后置递增操作符
     * @return 副本
     */
    const_iterator operator++(int)
    {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }
};

/**
 * @brief 哈希表局部迭代器
 * 仅遍历一个桶中的节点
 * 
 * @tparam T 值类型
 */
template <class T>
struct ht_local_iterator : public iterator<forward_iterator_tag, T>
{
    typedef T                          value_type;
    typedef value_type*                pointer;
    typedef value_type&                reference;
    typedef size_t                     size_type;
    typedef ptrdiff_t                  difference_type;
    typedef hashtable_node<T>*         node_ptr;

    typedef ht_local_iterator<T>       self;
    typedef ht_local_iterator<T>       local_iterator;
    typedef ht_const_local_iterator<T> const_local_iterator;
    
    node_ptr node;  // 指向当前节点

    /**
     * @brief 构造函数
     * @param n 节点指针
     */
    ht_local_iterator(node_ptr n) : node(n) {}
    
    /**
     * @brief 拷贝构造函数
     * @param rhs 右侧操作数
     */
    ht_local_iterator(const local_iterator& rhs) : node(rhs.node) {}
    
    /**
     * @brief 从const_local_iterator构造
     * @param rhs 右侧操作数
     */
    ht_local_iterator(const const_local_iterator& rhs) : node(rhs.node) {}

    /**
     * @brief 解引用操作符
     * @return 引用
     */
    reference operator*() const { return node->value; }
    
    /**
     * @brief 成员访问操作符
     * @return 指针
     */
    pointer operator->() const { return &(operator*()); }

    /**
     * @brief 前置递增操作符
     * @return 引用
     */
    self& operator++()
    {
        node = node->next;
        return *this;
    }
    
    /**
     * @brief 后置递增操作符
     * @return 副本
     */
    self operator++(int)
    {
        self tmp(*this);
        ++*this;
        return tmp;
    }

    /**
     * @brief 相等比较操作符
     * @param other 右侧操作数
     * @return 是否相等
     */
    bool operator==(const self& other) const { return node == other.node; }
    
    /**
     * @brief 不等比较操作符
     * @param other 右侧操作数
     * @return 是否不等
     */
    bool operator!=(const self& other) const { return node != other.node; }
};

/**
 * @brief 哈希表常量局部迭代器
 * 仅遍历一个桶中的节点
 * 
 * @tparam T 值类型
 */
template <class T>
struct ht_const_local_iterator : public iterator<forward_iterator_tag, T>
{
    typedef T                          value_type;
    typedef const value_type*          pointer;
    typedef const value_type&          reference;
    typedef size_t                     size_type;
    typedef ptrdiff_t                  difference_type;
    typedef const hashtable_node<T>*   node_ptr;

    typedef ht_const_local_iterator<T> self;
    typedef ht_local_iterator<T>       local_iterator;
    typedef ht_const_local_iterator<T> const_local_iterator;

    node_ptr node;  // 指向当前节点

    /**
     * @brief 构造函数
     * @param n 节点指针
     */
    ht_const_local_iterator(node_ptr n) : node(n) {}
    
    /**
     * @brief 从local_iterator构造
     * @param rhs 右侧操作数
     */
    ht_const_local_iterator(const local_iterator& rhs) : node(rhs.node) {}
    
    /**
     * @brief 拷贝构造函数
     * @param rhs 右侧操作数
     */
    ht_const_local_iterator(const const_local_iterator& rhs) : node(rhs.node) {}

    /**
     * @brief 解引用操作符
     * @return 引用
     */
    reference operator*() const { return node->value; }
    
    /**
     * @brief 成员访问操作符
     * @return 指针
     */
    pointer operator->() const { return &(operator*()); }

    /**
     * @brief 前置递增操作符
     * @return 引用
     */
    self& operator++()
    {
        node = node->next;
        return *this;
    }

    /**
     * @brief 后置递增操作符
     * @return 副本
     */
    self operator++(int)
    {
        self tmp(*this);
        ++*this;
        return tmp;
    }

    /**
     * @brief 相等比较操作符
     * @param other 右侧操作数
     * @return 是否相等
     */
    bool operator==(const self& other) const { return node == other.node; }
    
    /**
     * @brief 不等比较操作符
     * @param other 右侧操作数
     * @return 是否不等
     */
    bool operator!=(const self& other) const { return node != other.node; }
};

/**
 * @brief 哈希表素数表配置
 * 根据系统位数选择合适的素数表
 */
#if (defined(_WIN64) || defined(__x86_64__) || defined(__amd64) || defined(__aarch64__))
#define SYSTEM_64 1
#else
#define SYSTEM_32 1
#endif

#ifdef SYSTEM_64

#define PRIME_NUM 99

/**
 * @brief 64位系统的素数表
 * 生成规则:
 * 1. 从p = 101开始
 * 2. p = next_prime(p * 1.7)
 * 3. 如果p < (2 << 63)，重复步骤2，否则执行步骤4
 * 4. 以prev_prime(2 << 63 - 1)结束
 */
static constexpr size_t ht_prime_list[] = {
  101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
  4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
  119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
  2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
  23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
  264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
  2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
  15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
  115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull, 
  585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull, 
  2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
  15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
  76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
  384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
  1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
  9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
  49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
  252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
  1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
  6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
};

#else

#define PRIME_NUM 44

/**
 * @brief 32位系统的素数表
 * 生成规则:
 * 1. 从p = 101开始
 * 2. p = next_prime(p * 1.7)
 * 3. 如果p < (2 << 31)，重复步骤2，否则执行步骤4
 * 4. 以prev_prime(2 << 31 - 1)结束
 */
static constexpr size_t ht_prime_list[] = {
  101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u, 
  10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
  402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
  10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
  176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
  2006689337u, 3010034021u, 4294967291u,
};

#endif

/**
 * @brief 查找大于等于n的最小素数
 * 
 * @param n 目标值
 * @return 大于等于n的最小素数
 */
inline size_t ht_next_prime(size_t n)
{
    const size_t* first = ht_prime_list;
    const size_t* last = ht_prime_list + PRIME_NUM;
    const size_t* pos = std::lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

/**
 * @brief 哈希表模板类
 * 
 * @tparam T 值类型
 * @tparam Hash 哈希函数类型
 * @tparam KeyEqual 键值相等判断函数类型
 */
template <class T, class Hash, class KeyEqual>
class hashtable
{
    friend struct mystl::ht_iterator<T, Hash, KeyEqual>;
    friend struct mystl::ht_const_iterator<T, Hash, KeyEqual>;

public:
    // 哈希表的型别定义
    typedef ht_value_traits<T>                          value_traits;
    typedef typename value_traits::key_type             key_type;
    typedef typename value_traits::mapped_type          mapped_type;
    typedef typename value_traits::value_type           value_type;
    typedef Hash                                        hasher;
    typedef KeyEqual                                    key_equal;

    typedef hashtable_node<T>                           node_type;
    typedef node_type*                                  node_ptr;
    typedef mystl::vector<node_ptr>                     bucket_type;

    typedef std::allocator<T>                                allocator_type;
    typedef std::allocator<T>                                data_allocator;
    typedef std::allocator<node_type>                        node_allocator;

    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;
    typedef typename allocator_type::size_type          size_type;
    typedef typename allocator_type::difference_type    difference_type;

    typedef mystl::ht_iterator<T, Hash, KeyEqual>       iterator;
    typedef mystl::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
    typedef mystl::ht_local_iterator<T>                 local_iterator;
    typedef mystl::ht_const_local_iterator<T>           const_local_iterator;

    /**
     * @brief 获取分配器
     * @return 分配器对象
     */
    allocator_type get_allocator() const { return allocator_type(); }

private:
    // 用以下六个参数来表现哈希表
    bucket_type buckets_;     // 桶数组，每个桶是一个链表头指针
    size_type   bucket_size_; // 桶数量
    size_type   size_;        // 元素数量
    float       mlf_;         // 最大负载因子
    hasher      hash_;        // 哈希函数
    key_equal   equal_;       // 判断键值相等的函数

private:
    /**
     * @brief 判断两个键是否相等
     * @param key1 第一个键
     * @param key2 第二个键
     * @return 是否相等
     */
    bool is_equal(const key_type& key1, const key_type& key2)
    {
        return equal_(key1, key2);
    }

    /**
     * @brief 判断两个键是否相等（const版本）
     * @param key1 第一个键
     * @param key2 第二个键
     * @return 是否相等
     */
    bool is_equal(const key_type& key1, const key_type& key2) const
    {
        return equal_(key1, key2);
    }

    /**
     * @brief 获取常量迭代器
     * @param node 节点指针
     * @return 常量迭代器
     */
    const_iterator M_cit(node_ptr node) const noexcept
    {
        return const_iterator(node, const_cast<hashtable*>(this));
    }

    /**
     * @brief 获取哈希表起始位置迭代器
     * @return 迭代器
     */
    iterator M_begin() noexcept
    {
        for (size_type n = 0; n < bucket_size_; ++n)
        {
            if (buckets_[n])  // 找到第一个有节点的桶就返回
                return iterator(buckets_[n], this);
        }
        return iterator(nullptr, this);
    }

    /**
     * @brief 获取哈希表起始位置常量迭代器
     * @return 常量迭代器
     */
    const_iterator M_begin() const noexcept
    {
        for (size_type n = 0; n < bucket_size_; ++n)
        {
            if (buckets_[n])  // 找到第一个有节点的桶就返回
                return M_cit(buckets_[n]);
        }
        return M_cit(nullptr);
    }

public:
    // 构造、复制、移动、析构函数

    /**
     * @brief 构造函数
     * @param bucket_count 桶数量
     * @param hash 哈希函数
     * @param equal 键值相等判断函数
     */
    explicit hashtable(size_type bucket_count,
                     const Hash& hash = Hash(),
                     const KeyEqual& equal = KeyEqual())
        : size_(0), mlf_(1.0f), hash_(hash), equal_(equal)
    {
        init(bucket_count);
    }

    /**
     * @brief 从迭代器范围构造
     * @param first 起始迭代器
     * @param last 结束迭代器
     * @param bucket_count 桶数量
     * @param hash 哈希函数
     * @param equal 键值相等判断函数
     */
    template <class Iter, typename std::enable_if<
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, 
                           input_iterator_tag>::value, int>::type = 0>
    hashtable(Iter first, Iter last,
              size_type bucket_count,
              const Hash& hash = Hash(),
              const KeyEqual& equal = KeyEqual())
        : size_(std::distance(first, last)), mlf_(1.0f), hash_(hash), equal_(equal)
    {
        init(std::max(bucket_count, static_cast<size_type>(std::distance(first, last))));
    }

    /**
     * @brief 拷贝构造函数
     * @param rhs 源哈希表
     */
    hashtable(const hashtable& rhs)
        : hash_(rhs.hash_), equal_(rhs.equal_)
    {
        copy_init(rhs);
    }

    /**
     * @brief 移动构造函数
     * @param rhs 源哈希表
     */
    hashtable(hashtable&& rhs) noexcept
        : bucket_size_(rhs.bucket_size_), 
        size_(rhs.size_),
        mlf_(rhs.mlf_),
        hash_(rhs.hash_),
        equal_(rhs.equal_)
    {
        buckets_ = std::move(rhs.buckets_);
        rhs.bucket_size_ = 0;
        rhs.size_ = 0;
        rhs.mlf_ = 0.0f;
    }

    /**
     * @brief 拷贝赋值运算符
     * @param rhs 源哈希表
     * @return 哈希表引用
     */
    hashtable& operator=(const hashtable& rhs);

    /**
     * @brief 移动赋值运算符
     * @param rhs 源哈希表
     * @return 哈希表引用
     */
    hashtable& operator=(hashtable&& rhs) noexcept;

    /**
     * @brief 析构函数
     */
    ~hashtable() { clear(); }

    // 迭代器相关操作

    /**
     * @brief 获取起始迭代器
     * @return 起始迭代器
     */
    iterator       begin()        noexcept
    { return M_begin(); }

    /**
     * @brief 获取起始常量迭代器
     * @return 起始常量迭代器
     */
    const_iterator begin()  const noexcept
    { return M_begin(); }

    /**
     * @brief 获取结束迭代器
     * @return 结束迭代器
     */
    iterator       end()          noexcept
    { return iterator(nullptr, this); }

    /**
     * @brief 获取结束常量迭代器
     * @return 结束常量迭代器
     */
    const_iterator end()    const noexcept
    { return M_cit(nullptr); }
    
    /**
     * @brief 获取起始常量迭代器
     * @return 起始常量迭代器
     */
    const_iterator cbegin() const noexcept
    { return begin(); }

    /**
     * @brief 获取结束常量迭代器
     * @return 结束常量迭代器
     */
    const_iterator cend()   const noexcept
    { return end(); }

    // 容量相关操作

    /**
     * @brief 判断哈希表是否为空
     * @return 是否为空
     */
    bool      empty()    const noexcept { return size_ == 0; }

    /**
     * @brief 获取元素数量
     * @return 元素数量
     */
    size_type size()     const noexcept { return size_; }

    /**
     * @brief 获取最大元素数量
     * @return 最大元素数量
     */
    size_type max_size() const noexcept { return static_cast<size_type>(-1); }

    // 修改容器相关操作

    // emplace / emplace_hint
    
    /**
     * @brief 就地构造元素，允许重复键值
     * @param args 构造参数
     * @return 指向新元素的迭代器
     */
    template <class ...Args>
    iterator emplace_multi(Args&& ...args);

    /**
     * @brief 就地构造元素，不允许重复键值
     * @param args 构造参数
     * @return 插入结果对，包含迭代器和是否插入成功的标志
     */
    template <class ...Args>
    std::pair<iterator, bool> emplace_unique(Args&& ...args);

    /**
     * @brief 使用提示位置就地构造元素，允许重复键值
     * @param hint 提示位置
     * @param args 构造参数
     * @return 指向新元素的迭代器
     */
    template <class ...Args>
    iterator emplace_multi_use_hint(const_iterator /*hint*/, Args&& ...args)
    { return emplace_multi(std::forward<Args>(args)...); }

    /**
     * @brief 使用提示位置就地构造元素，不允许重复键值
     * @param hint 提示位置
     * @param args 构造参数
     * @return 指向新元素的迭代器
     */
    template <class ...Args>
    iterator emplace_unique_use_hint(const_iterator /*hint*/, Args&& ...args)
    { return emplace_unique(std::forward<Args>(args)...).first; }

    // insert

    /**
     * @brief 插入元素，允许重复键值，不调整大小
     * @param value 要插入的值
     * @return 指向新元素的迭代器
     */
    iterator             insert_multi_noresize(const value_type& value);

    /**
     * @brief 插入元素，不允许重复键值，不调整大小
     * @param value 要插入的值
     * @return 插入结果对，包含迭代器和是否插入成功的标志
     */
    std::pair<iterator, bool> insert_unique_noresize(const value_type& value);

    /**
     * @brief 插入元素，允许重复键值
     * @param value 要插入的值
     * @return 指向新元素的迭代器
     */
    iterator insert_multi(const value_type& value)
    {
        rehash_if_need(1);
        return insert_multi_noresize(value);
    }

    /**
     * @brief 插入元素，允许重复键值（移动版本）
     * @param value 要插入的值
     * @return 指向新元素的迭代器
     */
    iterator insert_multi(value_type&& value)
    { return emplace_multi(std::move(value)); }

    /**
     * @brief 插入元素，不允许重复键值
     * @param value 要插入的值
     * @return 插入结果对，包含迭代器和是否插入成功的标志
     */
    std::pair<iterator, bool> insert_unique(const value_type& value)
    {
        rehash_if_need(1);
        return insert_unique_noresize(value);
    }

    /**
     * @brief 插入元素，不允许重复键值（移动版本）
     * @param value 要插入的值
     * @return 插入结果对，包含迭代器和是否插入成功的标志
     */
    std::pair<iterator, bool> insert_unique(value_type&& value)
    { return emplace_unique(std::move(value)); }

    /**
     * @brief 使用提示位置插入元素，允许重复键值
     * @param hint 提示位置
     * @param value 要插入的值
     * @return 指向新元素的迭代器
     */
    iterator insert_multi_use_hint(const_iterator /*hint*/, const value_type& value)
    { return insert_multi(value); }

    /**
     * @brief 使用提示位置插入元素，允许重复键值（移动版本）
     * @param hint 提示位置
     * @param value 要插入的值
     * @return 指向新元素的迭代器
     */
    iterator insert_multi_use_hint(const_iterator /*hint*/, value_type&& value)
    { return emplace_multi(std::move(value)); }

    /**
     * @brief 使用提示位置插入元素，不允许重复键值
     * @param hint 提示位置
     * @param value 要插入的值
     * @return 指向新元素的迭代器
     */
    iterator insert_unique_use_hint(const_iterator /*hint*/, const value_type& value)
    { return insert_unique(value).first; }

    /**
     * @brief 使用提示位置插入元素，不允许重复键值（移动版本）
     * @param hint 提示位置
     * @param value 要插入的值
     * @return 指向新元素的迭代器
     */
    iterator insert_unique_use_hint(const_iterator /*hint*/, value_type&& value)
    { return emplace_unique(std::move(value)).first; }

    /**
     * @brief 从迭代器范围插入元素，允许重复键值
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <class InputIter>
    void insert_multi(InputIter first, InputIter last)
    { copy_insert_multi(first, last, typename std::iterator_traits<InputIter>::iterator_category()); }

    /**
     * @brief 从迭代器范围插入元素，不允许重复键值
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <class InputIter>
    void insert_unique(InputIter first, InputIter last)
    { copy_insert_unique(first, last, typename std::iterator_traits<InputIter>::iterator_category()); }

    // erase / clear

    /**
     * @brief 删除指定位置的元素
     * @param position 元素位置
     */
    void      erase(const_iterator position);

    /**
     * @brief 删除指定范围的元素
     * @param first 范围起始位置
     * @param last 范围结束位置
     */
    void      erase(const_iterator first, const_iterator last);

    /**
     * @brief 删除指定键的所有元素，允许重复键值
     * @param key 要删除的键
     * @return 删除的元素数量
     */
    size_type erase_multi(const key_type& key);

    /**
     * @brief 删除指定键的元素，不允许重复键值
     * @param key 要删除的键
     * @return 删除的元素数量
     */
    size_type erase_unique(const key_type& key);

    /**
     * @brief 清空哈希表
     */
    void      clear();

    /**
     * @brief 交换两个哈希表
     * @param rhs 要交换的哈希表
     */
    void      swap(hashtable& rhs) noexcept;

    // 查找相关操作

    /**
     * @brief 统计键的出现次数
     * @param key 要查找的键
     * @return 键出现的次数
     */
    size_type count(const key_type& key) const;

    /**
     * @brief 查找键对应的元素
     * @param key 要查找的键
     * @return 指向找到元素的迭代器，如果没找到则返回end()
     */
    iterator find(const key_type& key);

    /**
     * @brief 查找键对应的元素（const版本）
     * @param key 要查找的键
     * @return 指向找到元素的常量迭代器，如果没找到则返回end()
     */
    const_iterator find(const key_type& key) const;

    /**
     * @brief 查找键对应的元素范围，允许重复键值
     * @param key 要查找的键
     * @return 元素范围对，如果没找到则返回{end(),end()}
     */
    std::pair<iterator, iterator> equal_range_multi(const key_type& key);

    /**
     * @brief 查找键对应的元素范围，允许重复键值（const版本）
     * @param key 要查找的键
     * @return 元素范围对，如果没找到则返回{end(),end()}
     */
    std::pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const;

    /**
     * @brief 查找键对应的元素范围，不允许重复键值
     * @param key 要查找的键
     * @return 元素范围对，如果没找到则返回{end(),end()}
     */
    std::pair<iterator, iterator> equal_range_unique(const key_type& key);

    /**
     * @brief 查找键对应的元素范围，不允许重复键值（const版本）
     * @param key 要查找的键
     * @return 元素范围对，如果没找到则返回{end(),end()}
     */
    std::pair<const_iterator, const_iterator> equal_range_unique(const key_type& key) const;

    // bucket interface

    /**
     * @brief 获取指定桶的局部起始迭代器
     * @param n 桶索引
     * @return 局部迭代器
     */
    local_iterator begin(size_type n) noexcept
    { 
        return buckets_[n];
    }

    /**
     * @brief 获取指定桶的局部起始常量迭代器
     * @param n 桶索引
     * @return 局部常量迭代器
     */
    const_local_iterator begin(size_type n) const noexcept
    { 
        return buckets_[n];
    }

    /**
     * @brief 获取指定桶的局部起始常量迭代器
     * @param n 桶索引
     * @return 局部常量迭代器
     */
    const_local_iterator cbegin(size_type n) const noexcept
    { 
        return buckets_[n];
    }

    /**
     * @brief 获取指定桶的局部结束迭代器
     * @param n 桶索引
     * @return 局部迭代器
     */
    local_iterator end(size_type n) noexcept
    { 
        return nullptr; 
    }

    /**
     * @brief 获取指定桶的局部结束常量迭代器
     * @param n 桶索引
     * @return 局部常量迭代器
     */
    const_local_iterator end(size_type n) const noexcept
    { 
        return nullptr; 
    }

    /**
     * @brief 获取指定桶的局部结束常量迭代器
     * @param n 桶索引
     * @return 局部常量迭代器
     */
    const_local_iterator cend(size_type n) const noexcept
    {
        return nullptr; 
    }

    /**
     * @brief 获取桶数量
     * @return 桶数量
     */
    size_type bucket_count() const noexcept
    { return bucket_size_; }

    /**
     * @brief 获取最大桶数量
     * @return 最大桶数量
     */
    size_type max_bucket_count() const noexcept
    { return ht_prime_list[PRIME_NUM - 1]; }

    /**
     * @brief 获取指定桶中的元素数量
     * @param n 桶索引
     * @return 元素数量
     */
    size_type bucket_size(size_type n) const noexcept;

    /**
     * @brief 获取键对应的桶索引
     * @param key 键
     * @return 桶索引
     */
    size_type bucket(const key_type& key) const
    { return hash(key); }

    // hash policy

    /**
     * @brief 获取当前负载因子
     * @return 负载因子
     */
    float load_factor() const noexcept
    { return bucket_size_ != 0 ? (float)size_ / bucket_size_ : 0.0f; }

    /**
     * @brief 获取最大负载因子
     * @return 最大负载因子
     */
    float max_load_factor() const noexcept
    { return mlf_; }

    /**
     * @brief 设置最大负载因子
     * @param ml 最大负载因子
     */
    void max_load_factor(float ml)
    {
        if (ml != ml || ml < 0)
            throw std::out_of_range("invalid hash load factor");
        mlf_ = ml;
    }

    /**
     * @brief 重新哈希表以适应指定数量的桶
     * @param count 新的桶数量
     */
    void rehash(size_type count);

    /**
     * @brief 保留足够的桶以容纳指定数量的元素
     * @param count 元素数量
     */
    void reserve(size_type count)
    { rehash(static_cast<size_type>((float)count / max_load_factor() + 0.5f)); }

    /**
     * @brief 获取哈希函数
     * @return 哈希函数
     */
    hasher hash_fcn() const { return hash_; }

    /**
     * @brief 获取键相等判断函数
     * @return 键相等判断函数
     */
    key_equal key_eq() const { return equal_; }

private:
    // 哈希表成员函数

    // init
    /**
     * @brief 初始化哈希表
     * @param n 初始桶数量
     */
    void init(size_type n);

    /**
     * @brief 从另一个哈希表复制初始化
     * @param ht 源哈希表
     */
    void copy_init(const hashtable& ht);

    // node
    /**
     * @brief 创建节点
     * @param args 构造参数
     * @return 新节点指针
     */
    template <class ...Args>
    node_ptr create_node(Args&& ...args);

    /**
     * @brief 销毁节点
     * @param n 要销毁的节点指针
     */
    void destroy_node(node_ptr n);

    // hash
    /**
     * @brief 获取下一个桶数量
     * @param n 当前元素数量
     * @return 下一个桶数量
     */
    size_type next_size(size_type n) const;

    /**
     * @brief 计算键的哈希值并映射到指定范围
     * @param key 键
     * @param n 范围
     * @return 哈希值
     */
    size_type hash(const key_type& key, size_type n) const;

    /**
     * @brief 计算键的哈希值并映射到当前桶范围
     * @param key 键
     * @return 哈希值
     */
    size_type hash(const key_type& key) const;

    /**
     * @brief 如有必要则重新哈希表
     * @param n 新增元素数量
     */
    void rehash_if_need(size_type n);

    // insert
    /**
     * @brief 从输入迭代器范围插入元素，允许重复键值
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <class InputIter>
    void copy_insert_multi(InputIter first, InputIter last, input_iterator_tag);

    /**
     * @brief 从前向迭代器范围插入元素，允许重复键值
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <class ForwardIter>
    void copy_insert_multi(ForwardIter first, ForwardIter last, forward_iterator_tag);

    /**
     * @brief 从输入迭代器范围插入元素，不允许重复键值
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <class InputIter>
    void copy_insert_unique(InputIter first, InputIter last, input_iterator_tag);

    /**
     * @brief 从前向迭代器范围插入元素，不允许重复键值
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <class ForwardIter>
    void copy_insert_unique(ForwardIter first, ForwardIter last, forward_iterator_tag);

    // insert node
    /**
     * @brief 插入一个节点，不允许重复键值
     * @param np 节点指针
     * @return 插入结果对，包含迭代器和是否插入成功的标志
     */
    std::pair<iterator, bool> insert_node_unique(node_ptr np);

    /**
     * @brief 插入一个节点，允许重复键值
     * @param np 节点指针
     * @return 指向新元素的迭代器
     */
    iterator insert_node_multi(node_ptr np);

    // bucket operator
    /**
     * @brief 替换桶数组
     * @param bucket_count 新桶数量
     */
    void replace_bucket(size_type bucket_count);

    /**
     * @brief 删除指定桶中的元素范围
     * @param n 桶索引
     * @param first 范围起始节点
     * @param last 范围结束节点
     */
    void erase_bucket(size_type n, node_ptr first, node_ptr last);

    /**
     * @brief 删除指定桶中的元素前缀
     * @param n 桶索引
     * @param last 范围结束节点
     */
    void erase_bucket(size_type n, node_ptr last);

    // compare
    /**
     * @brief 比较两个哈希表是否相等，允许重复键值
     * @param other 另一个哈希表
     * @return 是否相等
     */
    bool equal_to_multi(const hashtable& other);

    /**
     * @brief 比较两个哈希表是否相等，不允许重复键值
     * @param other 另一个哈希表
     * @return 是否相等
     */
    bool equal_to_unique(const hashtable& other);
};

/*****************************************************************************************/
// 哈希表模板类的实现

/**
 * @brief 拷贝赋值运算符
 */
template <class T, class Hash, class KeyEqual>
hashtable<T, Hash, KeyEqual>& 
hashtable<T, Hash, KeyEqual>::operator=(const hashtable& rhs)
{
    if (this != &rhs)
    {
        hashtable tmp(rhs);
        swap(tmp);
    }
    return *this;
}

/**
 * @brief 移动赋值运算符
 */
template <class T, class Hash, class KeyEqual>
hashtable<T, Hash, KeyEqual>& 
hashtable<T, Hash, KeyEqual>::operator=(hashtable&& rhs) noexcept
{
    hashtable tmp(std::move(rhs));
    swap(tmp);
    return *this;
}

/**
 * @brief 就地构造元素，允许重复键值
 * 强异常安全保证
 */
template <class T, class Hash, class KeyEqual>
template <class ...Args>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::emplace_multi(Args&& ...args)
{
    auto np = create_node(std::forward<Args>(args)...);
    try
    {
        if ((float)(size_ + 1) > (float)bucket_size_ * max_load_factor())
            rehash(size_ + 1);
    }
    catch (...)
    {
        destroy_node(np);
        throw;
    }
    return insert_node_multi(np);
}

/**
 * @brief 就地构造元素，不允许重复键值
 * 强异常安全保证
 */
template <class T, class Hash, class KeyEqual>
template <class ...Args>
std::pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool> 
hashtable<T, Hash, KeyEqual>::emplace_unique(Args&& ...args)
{
    auto np = create_node(std::forward<Args>(args)...);
    try
    {
        if ((float)(size_ + 1) > (float)bucket_size_ * max_load_factor())
            rehash(size_ + 1);
    }
    catch (...)
    {
        destroy_node(np);
        throw;
    }
    return insert_node_unique(np);
}

/**
 * @brief 在不需要重建表格的情况下插入新节点，键值不允许重复
 */
template <class T, class Hash, class KeyEqual>
std::pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool>
hashtable<T, Hash, KeyEqual>::insert_unique_noresize(const value_type& value)
{
    const auto n = hash(value_traits::get_key(value));
    auto first = buckets_[n];
    for (auto cur = first; cur; cur = cur->next)
    {
        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(value)))
            return std::make_pair(iterator(cur, this), false);
    }
    // 让新节点成为链表的第一个节点
    auto tmp = create_node(value);  
    tmp->next = first;
    buckets_[n] = tmp;
    ++size_;
    return std::make_pair(iterator(tmp, this), true);
}

/**
 * @brief 在不需要重建表格的情况下插入新节点，键值允许重复
 */
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::insert_multi_noresize(const value_type& value)
{
    const auto n = hash(value_traits::get_key(value));
    auto first = buckets_[n];
    auto tmp = create_node(value);
    for (auto cur = first; cur; cur = cur->next)
    {
        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(value)))
        { // 如果链表中存在相同键值的节点就马上插入，然后返回
            tmp->next = cur->next;
            cur->next = tmp;
            ++size_;
            return iterator(tmp, this);
        }
    }
    // 否则插入在链表头部
    tmp->next = first;
    buckets_[n] = tmp;
    ++size_;
    return iterator(tmp, this);
}

/**
 * @brief 初始化哈希表
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::init(size_type n)
{
    const auto bucket_nums = next_size(n);
    try
    {
        buckets_.reserve(bucket_nums);
        buckets_.assign(bucket_nums, nullptr);
    }
    catch (...)
    {
        bucket_size_ = 0;
        size_ = 0;
        throw;
    }
    bucket_size_ = buckets_.size();
}

/**
 * @brief 从另一个哈希表复制初始化
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::copy_init(const hashtable& ht)
{
    bucket_size_ = 0;
    buckets_.reserve(ht.bucket_size_);
    buckets_.assign(ht.bucket_size_, nullptr);
    try
    {
        for (size_type i = 0; i < ht.bucket_size_; ++i)
        {
            node_ptr cur = ht.buckets_[i];
            if (cur)
            { // 如果某桶存在链表
                auto copy = create_node(cur->value);
                buckets_[i] = copy;
                for (auto next = cur->next; next; cur = next, next = cur->next)
                {  //复制链表
                    copy->next = create_node(next->value);
                    copy = copy->next;
                }
                copy->next = nullptr;
            }
        }
        bucket_size_ = ht.bucket_size_;
        mlf_ = ht.mlf_;
        size_ = ht.size_;
    }
    catch (...)
    {
        clear();
    }
}

/**
 * @brief 创建节点对象
 * @param args 传递给构造函数的参数
 * @return 节点指针
 */
template <class T, class Hash, class KeyEqual>
template <class ...Args>
typename hashtable<T, Hash, KeyEqual>::node_ptr
hashtable<T, Hash, KeyEqual>::create_node(Args&& ...args)
{
    node_allocator alloc;
    node_ptr tmp = alloc.allocate(1);
    try
    {
        data_allocator d_alloc;
        d_alloc.construct(std::addressof(tmp->value), std::forward<Args>(args)...);
        tmp->next = nullptr;
    }
    catch (...)
    {
        node_allocator n_alloc;
        n_alloc.deallocate(tmp, 1);
        throw;
    }
    return tmp;
}

/**
 * @brief 销毁节点对象
 * @param node 节点指针
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::destroy_node(node_ptr node)
{
    data_allocator d_alloc;
    d_alloc.destroy(std::addressof(node->value));
    node_allocator n_alloc;
    n_alloc.deallocate(node, 1);
}

/**
 * @brief 获取下一个桶数量
 */
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::next_size(size_type n) const
{
    return ht_next_prime(n);
}

/**
 * @brief 计算键的哈希值并映射到指定范围
 */
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::hash(const key_type& key, size_type n) const
{
    return hash_(key) % n;
}

/**
 * @brief 计算键的哈希值并映射到当前桶范围
 */
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::hash(const key_type& key) const
{
    return hash_(key) % bucket_size_;
}

/**
 * @brief 如有必要则重新哈希表
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::rehash_if_need(size_type n)
{
    if (static_cast<float>(size_ + n) > (float)bucket_size_ * max_load_factor())
        rehash(size_ + n);
}

/**
 * @brief 从输入迭代器范围插入元素，允许重复键值
 */
template <class T, class Hash, class KeyEqual>
template <class InputIter>
void hashtable<T, Hash, KeyEqual>::
copy_insert_multi(InputIter first, InputIter last, input_iterator_tag)
{
    rehash_if_need(std::distance(first, last));
    for (; first != last; ++first)
        insert_multi_noresize(*first);
}

/**
 * @brief 从前向迭代器范围插入元素，允许重复键值
 */
template <class T, class Hash, class KeyEqual>
template <class ForwardIter>
void hashtable<T, Hash, KeyEqual>::
copy_insert_multi(ForwardIter first, ForwardIter last, forward_iterator_tag)
{
    size_type n = std::distance(first, last);
    rehash_if_need(n);
    for (; n > 0; --n, ++first)
        insert_multi_noresize(*first);
}

/**
 * @brief 从输入迭代器范围插入元素，不允许重复键值
 */
template <class T, class Hash, class KeyEqual>
template <class InputIter>
void hashtable<T, Hash, KeyEqual>::
copy_insert_unique(InputIter first, InputIter last, input_iterator_tag)
{
    rehash_if_need(std::distance(first, last));
    for (; first != last; ++first)
        insert_unique_noresize(*first);
}

/**
 * @brief 从前向迭代器范围插入元素，不允许重复键值
 */
template <class T, class Hash, class KeyEqual>
template <class ForwardIter>
void hashtable<T, Hash, KeyEqual>::
copy_insert_unique(ForwardIter first, ForwardIter last, forward_iterator_tag)
{
    size_type n = std::distance(first, last);
    rehash_if_need(n);
    for (; n > 0; --n, ++first)
        insert_unique_noresize(*first);
}

/**
 * @brief 插入一个节点，允许重复键值
 */
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::insert_node_multi(node_ptr np)
{
    const auto n = hash(value_traits::get_key(np->value));
    auto cur = buckets_[n];
    if (cur == nullptr)
    {
        buckets_[n] = np;
        ++size_;
        return iterator(np, this);
    }
    for (; cur; cur = cur->next)
    {
        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(np->value)))
        {
            np->next = cur->next;
            cur->next = np;
            ++size_;
            return iterator(np, this);
        }
    }
    np->next = buckets_[n];
    buckets_[n] = np;
    ++size_;
    return iterator(np, this);
}

/**
 * @brief 插入一个节点，不允许重复键值
 */
template <class T, class Hash, class KeyEqual>
std::pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool>
hashtable<T, Hash, KeyEqual>::insert_node_unique(node_ptr np)
{
    const auto n = hash(value_traits::get_key(np->value));
    auto cur = buckets_[n];
    if (cur == nullptr)
    {
        buckets_[n] = np;
        ++size_;
        return std::make_pair(iterator(np, this), true);
    }
    for (; cur; cur = cur->next)
    {
        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(np->value)))
        {
            return std::make_pair(iterator(cur, this), false);
        }
    }
    np->next = buckets_[n];
    buckets_[n] = np;
    ++size_;
    return std::make_pair(iterator(np, this), true);
}

/**
 * @brief 删除迭代器所指的节点
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase(const_iterator position)
{
    auto p = position.node;
    if (p)
    {
        const auto n = hash(value_traits::get_key(p->value));
        auto cur = buckets_[n];
        if (cur == p)
        { // p 位于链表头部
            buckets_[n] = cur->next;
            destroy_node(cur);
            --size_;
        }
        else
        {
            auto next = cur->next;
            while (next)
            {
                if (next == p)
                {
                    cur->next = next->next;
                    destroy_node(next);
                    --size_;
                    break;
                }
                else
                {
                    cur = next;
                    next = cur->next;
                }
            }
        }
    }
}

/**
 * @brief 删除[first, last)内的节点
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase(const_iterator first, const_iterator last)
{
    if (first.node == last.node)
        return;
    auto first_bucket = first.node 
        ? hash(value_traits::get_key(first.node->value)) 
        : bucket_size_;
    auto last_bucket = last.node 
        ? hash(value_traits::get_key(last.node->value))
        : bucket_size_;
    if (first_bucket == last_bucket)
    { // 如果在同一个桶
        erase_bucket(first_bucket, first.node, last.node);
    }
    else
    {
        erase_bucket(first_bucket, first.node, nullptr);
        for (auto n = first_bucket + 1; n < last_bucket; ++n)
        {
            if(buckets_[n] != nullptr)
                erase_bucket(n, nullptr);
        }
        if (last_bucket != bucket_size_)
        {
            erase_bucket(last_bucket, last.node);
        }
    }
}

/**
 * @brief 删除键值为key的节点
 */
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
erase_multi(const key_type& key)
{
    auto p = equal_range_multi(key);
    if (p.first.node != nullptr)
    {
        erase(p.first, p.second);
        return std::distance(p.first, p.second);
    }
    return 0;
}

/**
 * @brief 删除键值为key的节点
 */
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
erase_unique(const key_type& key)
{
    const auto n = hash(key);
    auto first = buckets_[n];
    if (first)
    {
        if (is_equal(value_traits::get_key(first->value), key))
        {
            buckets_[n] = first->next;
            destroy_node(first);
            --size_;
            return 1;
        }
        else
        {
            auto next = first->next;
            while (next)
            {
                if (is_equal(value_traits::get_key(next->value), key))
                {
                    first->next = next->next;
                    destroy_node(next);
                    --size_;
                    return 1;
                }
                first = next;
                next = first->next;
            }
        }
    }
    return 0;
}

/**
 * @brief 清空哈希表
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
clear()
{
    if (size_ != 0)
    {
        for (size_type i = 0; i < bucket_size_; ++i)
        {
            node_ptr cur = buckets_[i];
            while (cur != nullptr)
            {
                node_ptr next = cur->next;
                destroy_node(cur);
                cur = next;
            }
            buckets_[i] = nullptr;
        }
        size_ = 0;
    }
}

/**
 * @brief 查找键值为key的节点，返回其迭代器
 */
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::
find(const key_type& key)
{
    const auto n = hash(key);
    node_ptr first = buckets_[n];
    for (; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {}
    return iterator(first, this);
}

/**
 * @brief 查找键值为key的节点，返回其迭代器
 */
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::const_iterator
hashtable<T, Hash, KeyEqual>::
find(const key_type& key) const
{
    const auto n = hash(key);
    node_ptr first = buckets_[n];
    for (; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {}
    return M_cit(first);
}

/**
 * @brief 查找键值为key出现的次数
 */
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
count(const key_type& key) const
{
    const auto n = hash(key);
    size_type result = 0;
    for (node_ptr cur = buckets_[n]; cur; cur = cur->next)
    {
        if (is_equal(value_traits::get_key(cur->value), key))
            ++result;
    }
    return result;
}

/**
 * @brief 查找与键值key相等的区间，返回一个pair，指向相等区间的首尾
 */
template <class T, class Hash, class KeyEqual>
std::pair<typename hashtable<T, Hash, KeyEqual>::iterator,
         typename hashtable<T, Hash, KeyEqual>::iterator>
hashtable<T, Hash, KeyEqual>::
equal_range_multi(const key_type& key)
{
    const auto n = hash(key);
    for (node_ptr first = buckets_[n]; first; first = first->next)
    {
        if (is_equal(value_traits::get_key(first->value), key))
        { // 如果出现相等的键值
            for (node_ptr second = first->next; second; second = second->next)
            {
                if (!is_equal(value_traits::get_key(second->value), key))
                    return std::make_pair(iterator(first, this), iterator(second, this));
            }
            for (auto m = n + 1; m < bucket_size_; ++m)
            { // 整个链表都相等，查找下一个链表出现的位置
                if (buckets_[m])
                    return std::make_pair(iterator(first, this), iterator(buckets_[m], this));
            }
            return std::make_pair(iterator(first, this), end());
        }
    }
    return std::make_pair(end(), end());
}

/**
 * @brief 查找与键值key相等的区间，返回一个pair，指向相等区间的首尾
 */
template <class T, class Hash, class KeyEqual>
std::pair<typename hashtable<T, Hash, KeyEqual>::const_iterator,
         typename hashtable<T, Hash, KeyEqual>::const_iterator>
hashtable<T, Hash, KeyEqual>::
equal_range_multi(const key_type& key) const
{
    const auto n = hash(key);
    for (node_ptr first = buckets_[n]; first; first = first->next)
    {
        if (is_equal(value_traits::get_key(first->value), key))
        {
            for (node_ptr second = first->next; second; second = second->next)
            {
                if (!is_equal(value_traits::get_key(second->value), key))
                    return std::make_pair(M_cit(first), M_cit(second));
            }
            for (auto m = n + 1; m < bucket_size_; ++m)
            { // 整个链表都相等，查找下一个链表出现的位置
                if (buckets_[m])
                    return std::make_pair(M_cit(first), M_cit(buckets_[m]));
            }
            return std::make_pair(M_cit(first), cend());
        }
    }
    return std::make_pair(cend(), cend());
}

/**
 * @brief 查找与键值key相等的区间，返回一个pair，指向相等区间的首尾
 */
template <class T, class Hash, class KeyEqual>
std::pair<typename hashtable<T, Hash, KeyEqual>::iterator,
         typename hashtable<T, Hash, KeyEqual>::iterator>
hashtable<T, Hash, KeyEqual>::
equal_range_unique(const key_type& key)
{
    const auto n = hash(key);
    for (node_ptr first = buckets_[n]; first; first = first->next)
    {
        if (is_equal(value_traits::get_key(first->value), key))
        {
            if (first->next)
                return std::make_pair(iterator(first, this), iterator(first->next, this));
            for (auto m = n + 1; m < bucket_size_; ++m)
            { // 整个链表都相等，查找下一个链表出现的位置
                if (buckets_[m])
                    return std::make_pair(iterator(first, this), iterator(buckets_[m], this));
            }
            return std::make_pair(iterator(first, this), end());
        }
    }
    return std::make_pair(end(), end());
}

/**
 * @brief 查找与键值key相等的区间，返回一个pair，指向相等区间的首尾
 */
template <class T, class Hash, class KeyEqual>
std::pair<typename hashtable<T, Hash, KeyEqual>::const_iterator,
         typename hashtable<T, Hash, KeyEqual>::const_iterator>
hashtable<T, Hash, KeyEqual>::
equal_range_unique(const key_type& key) const
{
    const auto n = hash(key);
    for (node_ptr first = buckets_[n]; first; first = first->next)
    {
        if (is_equal(value_traits::get_key(first->value), key))
        {
            if (first->next)
                return std::make_pair(M_cit(first), M_cit(first->next));
            for (auto m = n + 1; m < bucket_size_; ++m)
            { // 整个链表都相等，查找下一个链表出现的位置
                if (buckets_[m])
                    return std::make_pair(M_cit(first), M_cit(buckets_[m]));
            }
            return std::make_pair(M_cit(first), cend());
        }
    }
    return std::make_pair(cend(), cend());
}

/**
 * @brief 重新对元素进行一遍哈希，插入到新的位置
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
rehash(size_type count)
{
    auto n = ht_next_prime(count);
    if (n > bucket_size_)
    {
        replace_bucket(n);
    }
    else
    {
        if ((float)size_ / (float)n < max_load_factor() - 0.25f &&
            (float)n < (float)bucket_size_ * 0.75)  // 值得重新哈希
        {
            replace_bucket(n);
        }
    }
}

/**
 * @brief 替换桶
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
replace_bucket(size_type bucket_count)
{
    bucket_type bucket(bucket_count);
    if (size_ != 0)
    {
        for (size_type i = 0; i < bucket_size_; ++i)
        {
            for (auto first = buckets_[i]; first; first = first->next)
            {
                auto tmp = create_node(first->value);
                const auto n = hash(value_traits::get_key(first->value), bucket_count);
                auto f = bucket[n];
                bool is_inserted = false;
                for (auto cur = f; cur; cur = cur->next)
                {
                    if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(first->value)))
                    {
                        tmp->next = cur->next;
                        cur->next = tmp;
                        is_inserted = true;
                        break;
                    }
                }
                if (!is_inserted)
                {
                    tmp->next = f;
                    bucket[n] = tmp;
                }
            }
        }
    }
    buckets_.swap(bucket);
    bucket_size_ = buckets_.size();
}

/**
 * @brief 在某个桶节点的个数
 */
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
bucket_size(size_type n) const noexcept
{
    size_type result = 0;
    for (auto cur = buckets_[n]; cur; cur = cur->next)
    {
        ++result;
    }
    return result;
}

/**
 * @brief 删除指定桶中 [first, last) 的节点
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase_bucket(size_type n, node_ptr first, node_ptr last)
{
    auto cur = buckets_[n];
    if (cur == first)
    {
        erase_bucket(n, last);
    }
    else
    {
        node_ptr next = cur->next;
        for (; next != first; cur = next, next = cur->next) {}
        while (next != last)
        {
            cur->next = next->next;
            destroy_node(next);
            next = cur->next;
            --size_;
        }
    }
}

/**
 * @brief 删除指定桶中 [buckets_[n], last) 的节点
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase_bucket(size_type n, node_ptr last)
{
    auto cur = buckets_[n];
    while (cur != last)
    {
        auto next = cur->next;
        destroy_node(cur);
        cur = next;
        --size_;
    }
    buckets_[n] = last;
}

/**
 * @brief 交换两个hashtable
 */
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
swap(hashtable& rhs) noexcept
{
    if (this != &rhs)
    {
        buckets_.swap(rhs.buckets_);
        std::swap(bucket_size_, rhs.bucket_size_);
        std::swap(size_, rhs.size_);
        std::swap(mlf_, rhs.mlf_);
        std::swap(hash_, rhs.hash_);
        std::swap(equal_, rhs.equal_);
    }
}

/**
 * @brief 全局swap
 */
template <class T, class Hash, class KeyEqual>
void swap(hashtable<T, Hash, KeyEqual>& lhs,
          hashtable<T, Hash, KeyEqual>& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace mystl

#endif // MY_HASHTABLE_H 