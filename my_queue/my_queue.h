#ifndef MY_QUEUE_H_
#define MY_QUEUE_H_

// 这个头文件包含了两个模板类 queue 和 priority_queue
// queue          : 队列，遵循先进先出(FIFO)原则
// priority_queue : 优先队列，根据权值进行排序，权值最高的元素最先出队

#include "../my_deque/my_deque.h"
#include "../my_vector/my_vector.h"
#include <functional>
#include <type_traits>
#include <utility>
#include <initializer_list>

namespace mystl
{

/**
 * @class queue
 * @brief 队列容器，遵循先进先出(FIFO)原则
 * 
 * @tparam T 元素类型
 * @tparam Container 底层容器类型，默认使用 mystl::deque 作为底层容器
 */
template <class T, class Container = mystl::deque<T>>
class queue
{
public:
    // 容器类型别名定义
    using container_type = Container;
    // 使用底层容器的型别
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;

    // 静态断言确保T与容器的value_type一致
    static_assert(std::is_same<T, value_type>::value,
                "queue的类型T必须与底层容器的value_type类型一致");

private:
    container_type c_;  // 用底层容器表现 queue

public:
    /**
     * @brief 默认构造函数
     */
    queue() = default;

    /**
     * @brief 构造指定大小的队列
     * @param n 队列大小
     */
    explicit queue(size_type n) 
        : c_(n) 
    {
    }

    /**
     * @brief 构造指定大小且值为value的队列
     * @param n 队列大小
     * @param value 初始值
     */
    queue(size_type n, const value_type& value)
        : c_(n, value)
    {
    }

    /**
     * @brief 使用迭代器范围构造队列
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <class IIter>
    queue(IIter first, IIter last)
        : c_(first, last) 
    {
    }

    /**
     * @brief 使用初始化列表构造队列
     * @param ilist 初始化列表
     */
    queue(std::initializer_list<T> ilist)
        : c_(ilist.begin(), ilist.end()) 
    {
    }

    /**
     * @brief 使用已有容器构造队列
     * @param c 容器
     */
    queue(const Container& c) 
        : c_(c) 
    {
    }

    /**
     * @brief 移动已有容器构造队列
     * @param c 右值容器引用
     */
    queue(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
        : c_(std::move(c)) 
    {
    }

    /**
     * @brief 拷贝构造函数
     * @param rhs 源队列
     */
    queue(const queue& rhs) 
        : c_(rhs.c_) 
    {
    }

    /**
     * @brief 移动构造函数
     * @param rhs 源队列右值引用
     */
    queue(queue&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
        : c_(std::move(rhs.c_)) 
    {
    }

    /**
     * @brief 拷贝赋值运算符
     * @param rhs 源队列
     * @return 当前队列引用
     */
    queue& operator=(const queue& rhs) 
    {
        c_ = rhs.c_; 
        return *this; 
    }

    /**
     * @brief 移动赋值运算符
     * @param rhs 源队列右值引用
     * @return 当前队列引用
     */
    queue& operator=(queue&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value)
    { 
        c_ = std::move(rhs.c_);
        return *this;
    }

    /**
     * @brief 初始化列表赋值运算符
     * @param ilist 初始化列表
     * @return 当前队列引用
     */
    queue& operator=(std::initializer_list<T> ilist)
    { 
        c_ = ilist; 
        return *this; 
    }

    /**
     * @brief 析构函数
     */
    ~queue() = default;

    // 元素访问相关操作

    /**
     * @brief 获取队首元素
     * @return 队首元素引用
     */
    reference front() { return c_.front(); }

    /**
     * @brief 获取队首元素（常量版本）
     * @return 队首元素常量引用
     */
    const_reference front() const { return c_.front(); }

    /**
     * @brief 获取队尾元素
     * @return 队尾元素引用
     */
    reference back() { return c_.back(); }

    /**
     * @brief 获取队尾元素（常量版本）
     * @return 队尾元素常量引用
     */
    const_reference back() const { return c_.back(); }

    // 容量相关操作

    /**
     * @brief 检查队列是否为空
     * @return 如果队列为空，返回true；否则返回false
     */
    [[nodiscard]] bool empty() const noexcept { return c_.empty(); }

    /**
     * @brief 返回队列中的元素数量
     * @return 元素数量
     */
    size_type size() const noexcept { return c_.size(); }

    // 修改容器相关操作

    /**
     * @brief 原地构造并添加元素到队列尾部
     * @param args 元素构造参数
     */
    template <class ...Args>
    void emplace(Args&& ...args)
    { c_.emplace_back(std::forward<Args>(args)...); }

    /**
     * @brief 添加元素到队列尾部
     * @param value 要添加的元素值
     */
    void push(const value_type& value) 
    { c_.push_back(value); }

    /**
     * @brief 添加元素到队列尾部（移动版本）
     * @param value 要添加的元素右值引用
     */  
    void push(value_type&& value)      
    { c_.emplace_back(std::move(value)); }

    /**
     * @brief 弹出队首元素
     */
    void pop()                         
    { c_.pop_front(); }

    /**
     * @brief 清空队列
     */
    void clear()         
    { 
        while (!empty())
            pop(); 
    }

    /**
     * @brief 与另一个队列交换内容
     * @param rhs 要交换的队列
     */
    void swap(queue& rhs) noexcept(noexcept(std::swap(c_, rhs.c_)))
    { std::swap(c_, rhs.c_); }

public:
    // 友元比较操作符声明
    friend bool operator==(const queue& lhs, const queue& rhs) { return lhs.c_ == rhs.c_; }
    friend bool operator< (const queue& lhs, const queue& rhs) { return lhs.c_ <  rhs.c_; }
};

// 重载比较操作符

/**
 * @brief 判断两个队列是否不相等
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果两个队列不相等，返回true；否则返回false
 */
template <class T, class Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
    return !(lhs == rhs);
}

/**
 * @brief 判断左队列是否大于右队列
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果左队列大于右队列，返回true；否则返回false
 */
template <class T, class Container>
bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
    return rhs < lhs;
}

/**
 * @brief 判断左队列是否小于等于右队列
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果左队列小于等于右队列，返回true；否则返回false
 */
template <class T, class Container>
bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
    return !(rhs < lhs);
}

/**
 * @brief 判断左队列是否大于等于右队列
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果左队列大于等于右队列，返回true；否则返回false
 */
template <class T, class Container>
bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
    return !(lhs < rhs);
}

/**
 * @brief 交换两个队列的内容
 * @param lhs 左操作数
 * @param rhs 右操作数
 */
template <class T, class Container>
void swap(queue<T, Container>& lhs, queue<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

/*****************************************************************************************/

// 自定义堆操作算法，为priority_queue提供支持

/**
 * @brief 在堆中进行上滤操作
 * @param first 堆起始迭代器
 * @param holeIndex 需要上滤的位置
 * @param topIndex 堆顶位置
 * @param value 需要上滤的值
 * @param comp 比较器
 */
template <class RandomIter, class Distance, class T, class Compare>
void my___adjust_heap(RandomIter first, Distance holeIndex, Distance topIndex, T&& value, Compare comp)
{
    // 获取父节点索引
    Distance parent = (holeIndex - 1) / 2;
    
    // 向上调整，直到达到顶部或找到合适位置
    while (holeIndex > topIndex && comp(*(first + parent), value))
    {
        *(first + holeIndex) = std::move(*(first + parent));
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    
    // 将值放入最终位置
    *(first + holeIndex) = std::forward<T>(value);
}

/**
 * @brief 创建堆
 * @param first 范围起始迭代器
 * @param last 范围结束迭代器
 * @param comp 比较器
 */
template <class RandomIter, class Compare>
void my_make_heap(RandomIter first, RandomIter last, Compare comp)
{
    if (last - first < 2) 
        return;
    
    auto len = last - first;
    auto parent = (len - 2) / 2;
    
    // 从最后一个非叶节点开始向下调整
    while (true)
    {
        auto child = parent * 2 + 1;
        if (child < len)
        {
            // 选择较大的子节点
            if (child + 1 < len && comp(*(first + child), *(first + child + 1)))
                ++child;
            
            // 如果父节点小于子节点，交换并继续向下调整
            if (comp(*(first + parent), *(first + child)))
            {
                auto value = std::move(*(first + parent));
                *(first + parent) = std::move(*(first + child));
                *(first + child) = std::move(value);
                
                parent = child;
                continue;
            }
        }
        
        if (parent == 0)
            return;
        --parent;
    }
}

/**
 * @brief 将元素加入堆
 * @param first 范围起始迭代器
 * @param last 范围结束迭代器
 * @param comp 比较器
 */
template <class RandomIter, class Compare>
void my_push_heap(RandomIter first, RandomIter last, Compare comp)
{
    // 将新元素添加到堆的最后，然后上滤到合适位置
    my___adjust_heap(first, static_cast<typename std::iterator_traits<RandomIter>::difference_type>(last - first - 1), 
                 static_cast<typename std::iterator_traits<RandomIter>::difference_type>(0), 
                 std::move(*(last - 1)), comp);
}

/**
 * @brief 从堆中移除最顶端元素
 * @param first 范围起始迭代器
 * @param last 范围结束迭代器
 * @param comp 比较器
 */
template <class RandomIter, class Compare>
void my_pop_heap(RandomIter first, RandomIter last, Compare comp)
{
    if (last - first <= 1)
        return;
    
    // 将堆顶元素与最后一个元素交换
    std::swap(*first, *(last - 1));
    
    using DistanceType = typename std::iterator_traits<RandomIter>::difference_type;
    DistanceType len = last - first - 1;
    
    // 从堆顶开始向下调整
    DistanceType parent = 0;
    while (true)
    {
        DistanceType child = parent * 2 + 1;
        if (child >= len)
            break;
        
        // 选择较大的子节点
        if (child + 1 < len && comp(*(first + child), *(first + child + 1)))
            ++child;
        
        // 如果父节点小于子节点，交换并继续向下调整
        if (comp(*(first + parent), *(first + child)))
        {
            std::swap(*(first + parent), *(first + child));
            parent = child;
        }
        else
        {
            break;
        }
    }
}

/**
 * @class priority_queue
 * @brief 优先队列容器，元素按照优先级排序
 * 
 * @tparam T 元素类型
 * @tparam Container 底层容器类型，默认使用 mystl::vector 作为底层容器
 * @tparam Compare 比较器类型，默认使用 std::less 比较器
 */
template <class T, class Container = mystl::vector<T>,
          class Compare = std::less<typename Container::value_type>>
class priority_queue
{
public:
    // 容器类型别名定义
    using container_type = Container;
    using value_compare = Compare;
    // 使用底层容器的型别
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;

    // 静态断言确保T与容器的value_type一致
    static_assert(std::is_same<T, value_type>::value,
                "priority_queue的类型T必须与底层容器的value_type类型一致");

private:
    container_type c_;    // 用底层容器来表现 priority_queue
    value_compare comp_;  // 权值比较的标准

public:
    /**
     * @brief 默认构造函数
     */
    priority_queue() = default;

    /**
     * @brief 使用比较器构造优先队列
     * @param c 比较器
     */
    explicit priority_queue(const Compare& c) 
        : c_(), comp_(c) 
    {
    }

    /**
     * @brief 构造指定大小的优先队列
     * @param n 优先队列大小
     */
    explicit priority_queue(size_type n)
        : c_(n)
    {
        my_make_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 构造指定大小且值为value的优先队列
     * @param n 优先队列大小
     * @param value 初始值
     */
    priority_queue(size_type n, const value_type& value) 
        : c_(n, value)
    {
        my_make_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 使用迭代器范围构造优先队列
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <class IIter>
    priority_queue(IIter first, IIter last) 
        : c_(first, last)
    {
        my_make_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 使用初始化列表构造优先队列
     * @param ilist 初始化列表
     */
    priority_queue(std::initializer_list<T> ilist)
        : c_(ilist)
    {
        my_make_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 使用已有容器构造优先队列
     * @param s 容器
     */
    priority_queue(const Container& s)
        : c_(s)
    {
        my_make_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 移动已有容器构造优先队列
     * @param s 右值容器引用
     */
    priority_queue(Container&& s) 
        : c_(std::move(s))
    {
        my_make_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 拷贝构造函数
     * @param rhs 源优先队列
     */
    priority_queue(const priority_queue& rhs)
        : c_(rhs.c_), comp_(rhs.comp_)
    {
        my_make_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 移动构造函数
     * @param rhs 源优先队列右值引用
     */
    priority_queue(priority_queue&& rhs) 
        : c_(std::move(rhs.c_)), comp_(rhs.comp_)
    {
        my_make_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 拷贝赋值运算符
     * @param rhs 源优先队列
     * @return 当前优先队列引用
     */
    priority_queue& operator=(const priority_queue& rhs)
    {
        c_ = rhs.c_;
        comp_ = rhs.comp_;
        my_make_heap(c_.begin(), c_.end(), comp_);
        return *this;
    }

    /**
     * @brief 移动赋值运算符
     * @param rhs 源优先队列右值引用
     * @return 当前优先队列引用
     */
    priority_queue& operator=(priority_queue&& rhs)
    {
        c_ = std::move(rhs.c_);
        comp_ = rhs.comp_;
        my_make_heap(c_.begin(), c_.end(), comp_);
        return *this;
    }

    /**
     * @brief 初始化列表赋值运算符
     * @param ilist 初始化列表
     * @return 当前优先队列引用
     */
    priority_queue& operator=(std::initializer_list<T> ilist)
    {
        c_ = ilist;
        comp_ = value_compare();
        my_make_heap(c_.begin(), c_.end(), comp_);
        return *this;
    }

    /**
     * @brief 析构函数
     */
    ~priority_queue() = default;

public:
    // 元素访问相关操作

    /**
     * @brief 获取优先队列顶部元素（最高优先级元素）
     * @return 顶部元素常量引用
     */
    [[nodiscard]] const_reference top() const { return c_.front(); }

    // 容量相关操作

    /**
     * @brief 检查优先队列是否为空
     * @return 如果优先队列为空，返回true；否则返回false
     */
    [[nodiscard]] bool empty() const noexcept { return c_.empty(); }

    /**
     * @brief 返回优先队列中的元素数量
     * @return 元素数量
     */
    size_type size() const noexcept { return c_.size(); }

    // 修改容器相关操作

    /**
     * @brief 原地构造并添加元素到优先队列
     * @param args 元素构造参数
     */
    template <class... Args>
    void emplace(Args&& ...args)
    {
        c_.emplace_back(std::forward<Args>(args)...);
        my_push_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 添加元素到优先队列
     * @param value 要添加的元素值
     */
    void push(const value_type& value)
    {
        c_.push_back(value);
        my_push_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 添加元素到优先队列（移动版本）
     * @param value 要添加的元素右值引用
     */
    void push(value_type&& value)
    {
        c_.push_back(std::move(value));
        my_push_heap(c_.begin(), c_.end(), comp_);
    }

    /**
     * @brief 弹出优先队列的顶部元素
     */
    void pop()
    {
        my_pop_heap(c_.begin(), c_.end(), comp_);
        c_.pop_back();
    }

    /**
     * @brief 清空优先队列
     */
    void clear()
    {
        while (!empty())
            pop();
    }

    /**
     * @brief 与另一个优先队列交换内容
     * @param rhs 要交换的优先队列
     */
    void swap(priority_queue& rhs) noexcept(noexcept(std::swap(c_, rhs.c_)) &&
                                          noexcept(std::swap(comp_, rhs.comp_)))
    {
        std::swap(c_, rhs.c_);
        std::swap(comp_, rhs.comp_);
    }

public:
    // 友元比较操作符声明
    friend bool operator==(const priority_queue& lhs, const priority_queue& rhs)
    {
        return lhs.c_ == rhs.c_;
    }
    
    friend bool operator!=(const priority_queue& lhs, const priority_queue& rhs)
    {
        return lhs.c_ != rhs.c_;
    }
};

// 重载比较操作符

/**
 * @brief 判断两个优先队列是否相等
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果两个优先队列相等，返回true；否则返回false
 */
template <class T, class Container, class Compare>
bool operator==(const priority_queue<T, Container, Compare>& lhs,
                const priority_queue<T, Container, Compare>& rhs)
{
    return lhs == rhs;
}

/**
 * @brief 判断两个优先队列是否不相等
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果两个优先队列不相等，返回true；否则返回false
 */
template <class T, class Container, class Compare>
bool operator!=(const priority_queue<T, Container, Compare>& lhs,
                const priority_queue<T, Container, Compare>& rhs)
{
    return lhs != rhs;
}

/**
 * @brief 交换两个优先队列的内容
 * @param lhs 左操作数
 * @param rhs 右操作数
 */
template <class T, class Container, class Compare>
void swap(priority_queue<T, Container, Compare>& lhs, 
          priority_queue<T, Container, Compare>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

} // namespace mystl
#endif // !MY_QUEUE_H_ 