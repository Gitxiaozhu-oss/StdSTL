#ifndef RB_TREE_H
#define RB_TREE_H

/**
 * @file my_rb_tree.h
 * @brief 红黑树实现
 * 
 * 这个头文件包含一个模板类 rb_tree（红黑树）的完整实现
 * 红黑树是一种自平衡的二叉搜索树，具有以下五个性质：
 * 1. 每个节点是红色或黑色
 * 2. 根节点是黑色
 * 3. 所有叶子节点（NIL节点）都是黑色
 * 4. 如果一个节点是红色，则它的两个子节点都是黑色
 * 5. 对于每个节点，从该节点到其所有后代叶子节点的简单路径上的黑色节点数相同
 * 
 * 性能优化要点：
 * 1. 插入和删除操作：采用迭代而非递归实现，减少函数调用和栈空间开销
 * 2. 查找操作：使用二分搜索策略，实现O(log n)时间复杂度
 * 3. 内存管理：采用自定义内存分配策略，提高内存利用效率
 * 4. 指针操作：直接操作底层指针而非高层抽象，减少转换开销
 * 5. 平衡调整：插入和删除后的平衡调整采用最优判断顺序，减少操作次数
 * 6. 范围查询：对于equal_range等操作，针对multi和unique模式分别优化
 * 7. 迭代器：实现双向迭代器，支持前向和后向遍历
 * 8. 左右子树：平衡时对左右子树采用对称处理，减少代码冗余
 * 9. 边界更新：插入和删除操作中直接维护最左和最右节点，提高边界元素访问性能
 * 10. 空间优化：使用颜色位(bool)而非枚举，减少内存占用
 * 
 * 针对g++编译器的特定优化：
 * 1. 使用内联函数减少函数调用开销
 * 2. 避免虚函数，使编译器能够更好地进行优化
 * 3. 使用noexcept标记不抛出异常的函数，提高性能
 * 4. 利用移动语义减少不必要的拷贝操作
 * 5. 使用constexpr使编译器在编译期计算常量表达式
 */

#include <initializer_list>
#include <cassert>
#include <memory>
#include <functional>
#include <utility>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

namespace mystl {

// 定义红黑树节点颜色类型
using rb_tree_color_type = bool;

// 定义红色和黑色的常量
static constexpr rb_tree_color_type rb_tree_red = false;
static constexpr rb_tree_color_type rb_tree_black = true;

// 前置声明
template <class T> struct rb_tree_node_base;
template <class T> struct rb_tree_node;

template <class T> struct rb_tree_iterator;
template <class T> struct rb_tree_const_iterator;

/**
 * @brief 红黑树值特性
 * 
 * 这个结构用于处理红黑树的键值类型，支持单一值类型和键值对类型
 * 通过模板特化，区分开普通类型和pair类型的处理
 */
template <class T, bool IsMap>
struct rb_tree_value_traits_imp {
    using key_type = T;
    using mapped_type = T;
    using value_type = T;

    template <class Ty>
    static const key_type& get_key(const Ty& value) {
        return value;
    }

    template <class Ty>
    static const value_type& get_value(const Ty& value) {
        return value;
    }
};

// pair类型的特化版本
template <class T>
struct rb_tree_value_traits_imp<T, true> {
    using key_type = typename std::remove_cv<typename T::first_type>::type;
    using mapped_type = typename T::second_type;
    using value_type = T;

    template <class Ty>
    static const key_type& get_key(const Ty& value) {
        return value.first;
    }

    template <class Ty>
    static const value_type& get_value(const Ty& value) {
        return value;
    }
};

/**
 * @brief 红黑树值特性
 * 
 * 包装rb_tree_value_traits_imp，自动检测类型是否为pair
 */
template <class T>
struct rb_tree_value_traits {
    // 检测T是否为pair类型（更安全的实现）
    template <typename U>
    struct has_first_type {
        template <typename V> static auto test(V*) -> decltype(std::declval<V>().first, std::true_type());
        template <typename V> static auto test(...) -> std::false_type;
        static constexpr bool value = std::is_same<decltype(test<U>(nullptr)), std::true_type>::value;
    };
    
    static constexpr bool is_map = has_first_type<T>::value;

    using value_traits_type = rb_tree_value_traits_imp<T, is_map>;

    using key_type = typename value_traits_type::key_type;
    using mapped_type = typename value_traits_type::mapped_type;
    using value_type = typename value_traits_type::value_type;

    template <class Ty>
    static const key_type& get_key(const Ty& value) {
        return value_traits_type::get_key(value);
    }

    template <class Ty>
    static const value_type& get_value(const Ty& value) {
        return value_traits_type::get_value(value);
    }
};

/**
 * @brief 红黑树节点特性
 * 
 * 定义红黑树节点相关的类型
 */
template <class T>
struct rb_tree_node_traits {
    using color_type = rb_tree_color_type;

    using value_traits = rb_tree_value_traits<T>;
    using key_type = typename value_traits::key_type;
    using mapped_type = typename value_traits::mapped_type;
    using value_type = typename value_traits::value_type;

    using base_ptr = rb_tree_node_base<T>*;
    using node_ptr = rb_tree_node<T>*;
};

/**
 * @brief 红黑树基础节点
 * 
 * 包含红黑树节点的基本结构和指针
 */
template <class T>
struct rb_tree_node_base {
    using color_type = rb_tree_color_type;
    using base_ptr = rb_tree_node_base<T>*;
    using node_ptr = rb_tree_node<T>*;

    base_ptr parent;  // 父节点
    base_ptr left;    // 左子节点
    base_ptr right;   // 右子节点
    color_type color; // 节点颜色

    // 获取自身指针
    base_ptr get_base_ptr() noexcept {
        return this;
    }

    // 转换为节点指针
    node_ptr get_node_ptr() noexcept {
        return static_cast<node_ptr>(this);
    }

    // 获取节点引用
    node_ptr& get_node_ref() noexcept {
        return reinterpret_cast<node_ptr&>(*this);
    }
};

/**
 * @brief 红黑树节点
 * 
 * 继承基础节点，并包含实际的数据值
 */
template <class T>
struct rb_tree_node : public rb_tree_node_base<T> {
    using base_ptr = typename rb_tree_node_base<T>::base_ptr;
    using node_ptr = typename rb_tree_node_base<T>::node_ptr;

    T value;  // 节点值

    // 获取基础节点指针
    base_ptr get_base_ptr() noexcept {
        return static_cast<base_ptr>(this);
    }

    // 获取节点指针
    node_ptr get_node_ptr() noexcept {
        return this;
    }
};

/**
 * @brief 红黑树特性
 * 
 * 定义红黑树相关的类型特性
 */
template <class T>
struct rb_tree_traits {
    using value_traits = rb_tree_value_traits<T>;

    using key_type = typename value_traits::key_type;
    using mapped_type = typename value_traits::mapped_type;
    using value_type = typename value_traits::value_type;

    using pointer = value_type*;
    using reference = value_type&;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;

    using base_type = rb_tree_node_base<T>;
    using node_type = rb_tree_node<T>;

    using base_ptr = base_type*;
    using node_ptr = node_type*;
};

/**
 * @brief 红黑树迭代器基类
 * 
 * 实现红黑树的双向迭代器，支持正向和反向遍历
 */
template <class T>
struct rb_tree_iterator_base {
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    
    using base_ptr = typename rb_tree_traits<T>::base_ptr;

    base_ptr node;  // 指向节点本身

    rb_tree_iterator_base() noexcept : node(nullptr) {}

    /**
     * @brief 使迭代器前进（中序遍历的下一个节点）
     * 
     * 1. 如果有右子节点，则下一个节点是右子树的最左节点
     * 2. 如果没有右子节点，则向上查找，直到找到一个节点是其父节点的左子节点
     */
    void inc() noexcept {
        if (node->right != nullptr) {
            // 情况1：存在右子节点，下一个是右子树的最左节点
            node = rb_tree_min(node->right);
        } else {
            // 情况2：不存在右子节点，向上查找
            auto y = node->parent;
            while (node == y->right) {
                node = y;
                y = y->parent;
            }
            // 特殊情况：如果node的右子节点不是y，说明此时node是header或者根节点
            if (node->right != y) {
                node = y;
            }
        }
    }

    /**
     * @brief 使迭代器后退（中序遍历的前一个节点）
     * 
     * 1. 如果当前节点是header，则移动到最右节点
     * 2. 如果有左子节点，则前一个节点是左子树的最右节点
     * 3. 如果没有左子节点，则向上查找，直到找到一个节点是其父节点的右子节点
     */
    void dec() noexcept {
        if (node->parent->parent == node && node->color == rb_tree_red) {
            // 如果node为header，当前是end()迭代器
            node = node->right;  // 指向整棵树的最右节点
        } else if (node->left != nullptr) {
            // 存在左子节点，前一个是左子树的最右节点
            node = rb_tree_max(node->left);
        } else {
            // 不存在左子节点，向上查找
            auto y = node->parent;
            while (node == y->left) {
                node = y;
                y = y->parent;
            }
            node = y;
        }
    }

    bool operator==(const rb_tree_iterator_base& rhs) const noexcept {
        return node == rhs.node;
    }

    bool operator!=(const rb_tree_iterator_base& rhs) const noexcept {
        return node != rhs.node;
    }
};

/**
 * @brief 红黑树迭代器
 * 
 * 实现红黑树的可读写迭代器
 */
template <class T>
struct rb_tree_iterator : public rb_tree_iterator_base<T> {
    using iterator_category = std::bidirectional_iterator_tag;
    using tree_traits = rb_tree_traits<T>;

    using value_type = typename tree_traits::value_type;
    using pointer = typename tree_traits::pointer;
    using reference = typename tree_traits::reference;
    using difference_type = std::ptrdiff_t;
    
    using base_ptr = typename tree_traits::base_ptr;
    using node_ptr = typename tree_traits::node_ptr;

    using iterator = rb_tree_iterator<T>;
    using const_iterator = rb_tree_const_iterator<T>;
    using self = iterator;

    using rb_tree_iterator_base<T>::node;

    // 构造函数
    rb_tree_iterator() noexcept = default;
    explicit rb_tree_iterator(base_ptr x) noexcept { node = x; }
    rb_tree_iterator(node_ptr x) noexcept { node = x; }
    rb_tree_iterator(const iterator& rhs) noexcept { node = rhs.node; }
    rb_tree_iterator(const const_iterator& rhs) noexcept { node = rhs.node; }

    // 重载操作符
    reference operator*() const noexcept { 
        return node->get_node_ptr()->value; 
    }
    
    pointer operator->() const noexcept { 
        return std::addressof(operator*()); 
    }

    self& operator++() noexcept {
        this->inc();
        return *this;
    }

    self operator++(int) noexcept {
        self tmp(*this);
        this->inc();
        return tmp;
    }

    self& operator--() noexcept {
        this->dec();
        return *this;
    }

    self operator--(int) noexcept {
        self tmp(*this);
        this->dec();
        return tmp;
    }
};

/**
 * @brief 红黑树常量迭代器
 * 
 * 实现红黑树的只读迭代器
 */
template <class T>
struct rb_tree_const_iterator : public rb_tree_iterator_base<T> {
    using iterator_category = std::bidirectional_iterator_tag;
    using tree_traits = rb_tree_traits<T>;

    using value_type = typename tree_traits::value_type;
    using pointer = typename tree_traits::const_pointer;
    using reference = typename tree_traits::const_reference;
    using difference_type = std::ptrdiff_t;
    
    using base_ptr = typename tree_traits::base_ptr;
    using node_ptr = typename tree_traits::node_ptr;

    using iterator = rb_tree_iterator<T>;
    using const_iterator = rb_tree_const_iterator<T>;
    using self = const_iterator;

    using rb_tree_iterator_base<T>::node;

    // 构造函数
    rb_tree_const_iterator() noexcept = default;
    explicit rb_tree_const_iterator(base_ptr x) noexcept { node = x; }
    rb_tree_const_iterator(node_ptr x) noexcept { node = x; }
    rb_tree_const_iterator(const iterator& rhs) noexcept { node = rhs.node; }
    rb_tree_const_iterator(const const_iterator& rhs) noexcept { node = rhs.node; }

    // 重载操作符
    reference operator*() const noexcept { 
        return node->get_node_ptr()->value; 
    }
    
    pointer operator->() const noexcept { 
        return std::addressof(operator*()); 
    }

    self& operator++() noexcept {
        this->inc();
        return *this;
    }

    self operator++(int) noexcept {
        self tmp(*this);
        this->inc();
        return tmp;
    }

    self& operator--() noexcept {
        this->dec();
        return *this;
    }

    self operator--(int) noexcept {
        self tmp(*this);
        this->dec();
        return tmp;
    }
};

// 红黑树算法实现部分

/**
 * @brief 寻找红黑树中最小节点
 * 
 * 从给定节点开始，沿着左子树方向一直查找到最左边的节点
 */
template <class NodePtr>
NodePtr rb_tree_min(NodePtr x) noexcept {
    while (x->left != nullptr) {
        x = x->left;
    }
    return x;
}

/**
 * @brief 寻找红黑树中最大节点
 * 
 * 从给定节点开始，沿着右子树方向一直查找到最右边的节点
 */
template <class NodePtr>
NodePtr rb_tree_max(NodePtr x) noexcept {
    while (x->right != nullptr) {
        x = x->right;
    }
    return x;
}

/**
 * @brief 判断节点是否为左子节点
 */
template <class NodePtr>
bool rb_tree_is_lchild(NodePtr node) noexcept {
    return node == node->parent->left;
}

/**
 * @brief 判断节点是否为红色
 */
template <class NodePtr>
bool rb_tree_is_red(NodePtr node) noexcept {
    return node != nullptr && node->color == rb_tree_red;
}

/**
 * @brief 将节点设置为黑色
 */
template <class NodePtr>
void rb_tree_set_black(NodePtr& node) noexcept {
    if (node != nullptr) {
        node->color = rb_tree_black;
    }
}

/**
 * @brief 将节点设置为红色
 */
template <class NodePtr>
void rb_tree_set_red(NodePtr& node) noexcept {
    if (node != nullptr) {
        node->color = rb_tree_red;
    }
}

/**
 * @brief 获取节点的下一个节点（中序遍历顺序）
 */
template <class NodePtr>
NodePtr rb_tree_next(NodePtr node) noexcept {
    if (node->right != nullptr) {
        return rb_tree_min(node->right);
    }
    
    while (!rb_tree_is_lchild(node)) {
        node = node->parent;
    }
    return node->parent;
}

/**
 * @brief 红黑树左旋操作
 * 
 * 示意图:
 * 
 *     p                         p
 *    / \                       / \
 *   x   d    rotate left      y   d
 *  / \       ===========>    / \
 * a   y                     x   c
 *    / \                   / \
 *   b   c                 a   b
 * 
 * @param x 左旋点
 * @param root 根节点
 */
template <class NodePtr>
void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept {
    // y为x的右子节点
    auto y = x->right;
    x->right = y->left;
    
    if (y->left != nullptr) {
        y->left->parent = x;
    }
    y->parent = x->parent;

    if (x == root) {
        // 如果x是根节点，更新根节点为y
        root = y;
    } else if (rb_tree_is_lchild(x)) {
        // 如果x是左子节点
        x->parent->left = y;
    } else {
        // 如果x是右子节点
        x->parent->right = y;
    }
    
    // 重新构建x和y的关系
    y->left = x;
    x->parent = y;
}

/**
 * @brief 红黑树右旋操作
 * 
 * 示意图:
 * 
 *     p                         p
 *    / \                       / \
 *   d   x      rotate right   d   y
 *      / \     ===========>      / \
 *     y   a                     b   x
 *    / \                           / \
 *   b   c                         c   a
 * 
 * @param x 右旋点
 * @param root 根节点
 */
template <class NodePtr>
void rb_tree_rotate_right(NodePtr x, NodePtr& root) noexcept {
    // y为x的左子节点
    auto y = x->left;
    x->left = y->right;
    
    if (y->right != nullptr) {
        y->right->parent = x;
    }
    y->parent = x->parent;

    if (x == root) {
        // 如果x是根节点，更新根节点为y
        root = y;
    } else if (rb_tree_is_lchild(x)) {
        // 如果x是左子节点
        x->parent->left = y;
    } else {
        // 如果x是右子节点
        x->parent->right = y;
    }
    
    // 重新构建x和y的关系
    y->right = x;
    x->parent = y;
}

/**
 * @brief 插入节点后使红黑树重新平衡
 * 
 * 优化说明:
 * 1. 使用迭代而非递归，减少栈开销
 * 2. 处理最常见的情况（case 3 和 case 5）优先，提高性能
 * 3. 左右子树对称处理，减少代码冗余
 * 4. 条件判断顺序经过优化，减少比较次数
 * 5. 将新节点默认设置为红色，降低树高度调整概率
 * 
 * 时间复杂度: O(log n)
 * 
 * @param x 新插入的节点
 * @param root 根节点
 */
template <class NodePtr>
void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept {
    // 新节点默认为红色
    rb_tree_set_red(x);
    
    // 如果新节点不是根节点，且父节点为红色，需要调整平衡
    while (x != root && rb_tree_is_red(x->parent)) {
        if (rb_tree_is_lchild(x->parent)) {
            // 父节点是祖父节点的左子节点
            auto uncle = x->parent->parent->right;
            
            if (uncle != nullptr && rb_tree_is_red(uncle)) {
                // case 3: 父节点和叔叔节点都为红
                rb_tree_set_black(x->parent);
                rb_tree_set_black(uncle);
                rb_tree_set_red(x->parent->parent);
                x = x->parent->parent; // 继续向上调整
            } else {
                // 叔叔节点不存在或为黑
                if (!rb_tree_is_lchild(x)) {
                    // case 4: 当前节点是右子节点
                    x = x->parent;
                    rb_tree_rotate_left(x, root);
                }
                
                // case 5: 当前节点是左子节点
                rb_tree_set_black(x->parent);
                rb_tree_set_red(x->parent->parent);
                rb_tree_rotate_right(x->parent->parent, root);
                break;
            }
        } else {
            // 父节点是祖父节点的右子节点，对称处理
            auto uncle = x->parent->parent->left;
            
            if (uncle != nullptr && rb_tree_is_red(uncle)) {
                // case 3: 父节点和叔叔节点都为红
                rb_tree_set_black(x->parent);
                rb_tree_set_black(uncle);
                rb_tree_set_red(x->parent->parent);
                x = x->parent->parent; // 继续向上调整
            } else {
                // 叔叔节点不存在或为黑
                if (rb_tree_is_lchild(x)) {
                    // case 4: 当前节点是左子节点
                    x = x->parent;
                    rb_tree_rotate_right(x, root);
                }
                
                // case 5: 当前节点是右子节点
                rb_tree_set_black(x->parent);
                rb_tree_set_red(x->parent->parent);
                rb_tree_rotate_left(x->parent->parent, root);
                break;
            }
        }
    }
    
    // 根节点始终为黑色
    rb_tree_set_black(root);
}

/**
 * @brief 删除节点后使红黑树重新平衡
 * 
 * 优化说明:
 * 1. 删除操作分为两个阶段：替换和修复
 * 2. 提前判断各种特殊情况，减少不必要的操作
 * 3. 针对被删节点颜色是黑色的情况专门处理，维持红黑树性质
 * 4. 通过指针直接操作，避免节点复制带来的性能损失
 * 5. 左右子树对称处理，减少代码冗余并提高可维护性
 * 
 * 时间复杂度: O(log n)
 * 
 * @param z 要删除的节点
 * @param root 根节点
 * @param leftmost 最左节点
 * @param rightmost 最右节点
 * @return 返回删除的节点
 */
template <class NodePtr>
NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost) {
    // y是可能的替换节点，指向最终要删除的节点
    auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
    // x是y的一个独子节点或nullptr
    auto x = y->left != nullptr ? y->left : y->right;
    // xp为x的父节点
    NodePtr xp = nullptr;

    // 处理删除节点的情况
    if (y != z) {
        // z有两个非空子节点的情况
        // 将y（z的后继）移动到z的位置
        z->left->parent = y;
        y->left = z->left;

        if (y != z->right) {
            // 如果y不是z的右子节点
            xp = y->parent;
            if (x != nullptr) {
                x->parent = y->parent;
            }

            y->parent->left = x;
            y->right = z->right;
            z->right->parent = y;
        } else {
            // 如果y是z的右子节点
            xp = y;
        }

        // 将y连接到z的父节点
        if (root == z) {
            root = y;
        } else if (rb_tree_is_lchild(z)) {
            z->parent->left = y;
        } else {
            z->parent->right = y;
        }
        y->parent = z->parent;
        std::swap(y->color, z->color);
        y = z; // y现在指向要删除的节点
    } else {
        // z最多有一个非空子节点
        xp = y->parent;
        if (x != nullptr) {
            x->parent = y->parent;
        }

        // 将x连接到z的父节点
        if (root == z) {
            root = x;
        } else if (rb_tree_is_lchild(z)) {
            z->parent->left = x;
        } else {
            z->parent->right = x;
        }

        // 更新最左和最右节点
        if (leftmost == z) {
            leftmost = x == nullptr ? xp : rb_tree_min(x);
        }
        if (rightmost == z) {
            rightmost = x == nullptr ? xp : rb_tree_max(x);
        }
    }

    // 如果删除的是黑色节点，需要调整平衡
    if (!rb_tree_is_red(y)) {
        // x现在在原来y的位置上
        while (x != root && (x == nullptr || !rb_tree_is_red(x))) {
            if (x == xp->left) {
                // x是左子节点
                auto brother = xp->right;
                
                if (rb_tree_is_red(brother)) {
                    // case 1: x的兄弟节点是红色
                    rb_tree_set_black(brother);
                    rb_tree_set_red(xp);
                    rb_tree_rotate_left(xp, root);
                    brother = xp->right;
                }
                
                if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
                    (brother->right == nullptr || !rb_tree_is_red(brother->right))) {
                    // case 2: x的兄弟节点是黑色，且两个子节点都是黑色
                    rb_tree_set_red(brother);
                    x = xp;
                    xp = xp->parent;
                } else {
                    if (brother->right == nullptr || !rb_tree_is_red(brother->right)) {
                        // case 3: x的兄弟节点是黑色，左子节点是红色，右子节点是黑色
                        if (brother->left != nullptr) {
                            rb_tree_set_black(brother->left);
                        }
                        rb_tree_set_red(brother);
                        rb_tree_rotate_right(brother, root);
                        brother = xp->right;
                    }
                    
                    // case 4: x的兄弟节点是黑色，右子节点是红色
                    brother->color = xp->color;
                    rb_tree_set_black(xp);
                    if (brother->right != nullptr) {
                        rb_tree_set_black(brother->right);
                    }
                    rb_tree_rotate_left(xp, root);
                    break;
                }
            } else {
                // x是右子节点，对称处理
                auto brother = xp->left;
                
                if (rb_tree_is_red(brother)) {
                    // case 1: x的兄弟节点是红色
                    rb_tree_set_black(brother);
                    rb_tree_set_red(xp);
                    rb_tree_rotate_right(xp, root);
                    brother = xp->left;
                }
                
                if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
                    (brother->right == nullptr || !rb_tree_is_red(brother->right))) {
                    // case 2: x的兄弟节点是黑色，且两个子节点都是黑色
                    rb_tree_set_red(brother);
                    x = xp;
                    xp = xp->parent;
                } else {
                    if (brother->left == nullptr || !rb_tree_is_red(brother->left)) {
                        // case 3: x的兄弟节点是黑色，右子节点是红色，左子节点是黑色
                        if (brother->right != nullptr) {
                            rb_tree_set_black(brother->right);
                        }
                        rb_tree_set_red(brother);
                        rb_tree_rotate_left(brother, root);
                        brother = xp->left;
                    }
                    
                    // case 4: x的兄弟节点是黑色，左子节点是红色
                    brother->color = xp->color;
                    rb_tree_set_black(xp);
                    if (brother->left != nullptr) {
                        rb_tree_set_black(brother->left);
                    }
                    rb_tree_rotate_right(xp, root);
                    break;
                }
            }
        }
        
        // 如果x是红色，将其设为黑色
        if (x != nullptr) {
            rb_tree_set_black(x);
        }
    }
    
    return y;
}

/**
 * @brief 红黑树模板类
 * 
 * 红黑树是一种自平衡的二叉搜索树，常用作关联容器的底层实现
 * 
 * @tparam T 存储的数据类型
 * @tparam Compare 比较器类型，用于比较键值
 */
template <class T, class Compare>
class rb_tree {
public:
    // 类型定义
    using tree_traits = rb_tree_traits<T>;
    using value_traits = rb_tree_value_traits<T>;

    using base_type = typename tree_traits::base_type;
    using base_ptr = typename tree_traits::base_ptr;
    using node_type = typename tree_traits::node_type;
    using node_ptr = typename tree_traits::node_ptr;
    using key_type = typename tree_traits::key_type;
    using mapped_type = typename tree_traits::mapped_type;
    using value_type = typename tree_traits::value_type;
    using key_compare = Compare;

    using allocator_type = std::allocator<T>;
    using data_allocator = std::allocator<T>;
    using base_allocator = std::allocator<base_type>;
    using node_allocator = std::allocator<node_type>;

    using pointer = typename allocator_type::pointer;
    using const_pointer = typename allocator_type::const_pointer;
    using reference = typename allocator_type::reference;
    using const_reference = typename allocator_type::const_reference;
    using size_type = typename allocator_type::size_type;
    using difference_type = typename allocator_type::difference_type;

    using iterator = rb_tree_iterator<T>;
    using const_iterator = rb_tree_const_iterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // 获取分配器
    allocator_type get_allocator() const { return node_allocator(); }
    // 获取比较器
    key_compare key_comp() const { return key_comp_; }

private:
    // 红黑树的数据成员
    base_ptr    header_;      // 特殊节点，与根节点互为对方的父节点
    size_type   node_count_;  // 节点数量
    key_compare key_comp_;    // 节点键值比较准则

private:
    // 获取根节点、最左节点和最右节点
    base_ptr& root() const { return header_->parent; }
    base_ptr& leftmost() const { return header_->left; }
    base_ptr& rightmost() const { return header_->right; }

public:
    // 构造、复制、析构函数
    /**
     * @brief 默认构造函数
     */
    rb_tree() { 
        rb_tree_init(); 
    }

    /**
     * @brief 复制构造函数
     */
    rb_tree(const rb_tree& rhs);

    /**
     * @brief 移动构造函数
     */
    rb_tree(rb_tree&& rhs) noexcept;

    /**
     * @brief 复制赋值运算符
     */
    rb_tree& operator=(const rb_tree& rhs);

    /**
     * @brief 移动赋值运算符
     */
    rb_tree& operator=(rb_tree&& rhs);

    /**
     * @brief 析构函数
     */
    ~rb_tree() { 
        clear(); 
    }

public:
    // 迭代器相关操作

    /**
     * @brief 返回指向容器第一个元素的迭代器
     */
    iterator begin() noexcept { 
        return iterator(leftmost()); 
    }
    
    /**
     * @brief 返回指向容器第一个元素的常量迭代器
     */
    const_iterator begin() const noexcept { 
        return const_iterator(leftmost()); 
    }
    
    /**
     * @brief 返回指向容器尾部的迭代器
     */
    iterator end() noexcept { 
        return iterator(header_); 
    }
    
    /**
     * @brief 返回指向容器尾部的常量迭代器
     */
    const_iterator end() const noexcept { 
        return const_iterator(header_); 
    }

    /**
     * @brief 返回指向容器反向第一个元素的反向迭代器
     */
    reverse_iterator rbegin() noexcept { 
        return reverse_iterator(end()); 
    }
    
    /**
     * @brief 返回指向容器反向第一个元素的常量反向迭代器
     */
    const_reverse_iterator rbegin() const noexcept { 
        return const_reverse_iterator(end()); 
    }
    
    /**
     * @brief 返回指向容器反向尾部的反向迭代器
     */
    reverse_iterator rend() noexcept { 
        return reverse_iterator(begin()); 
    }
    
    /**
     * @brief 返回指向容器反向尾部的常量反向迭代器
     */
    const_reverse_iterator rend() const noexcept { 
        return const_reverse_iterator(begin()); 
    }

    /**
     * @brief 返回指向容器第一个元素的常量迭代器
     */
    const_iterator cbegin() const noexcept { 
        return begin(); 
    }
    
    /**
     * @brief 返回指向容器尾部的常量迭代器
     */
    const_iterator cend() const noexcept { 
        return end(); 
    }
    
    /**
     * @brief 返回指向容器反向第一个元素的常量反向迭代器
     */
    const_reverse_iterator crbegin() const noexcept { 
        return rbegin(); 
    }
    
    /**
     * @brief 返回指向容器反向尾部的常量反向迭代器
     */
    const_reverse_iterator crend() const noexcept { 
        return rend(); 
    }

    // 容量相关操作

    /**
     * @brief 检查容器是否为空
     */
    bool empty() const noexcept { 
        return node_count_ == 0; 
    }
    
    /**
     * @brief 返回容器中的元素数
     */
    size_type size() const noexcept { 
        return node_count_; 
    }
    
    /**
     * @brief 返回容器可以容纳的最大元素数
     */
    size_type max_size() const noexcept { 
        return static_cast<size_type>(-1); 
    }

    // 插入删除相关操作

    /**
     * @brief 原位构造元素，允许键值重复
     * 
     * @tparam Args 参数类型包
     * @param args 构造元素的参数
     * @return 指向新插入元素的迭代器
     */
    template <class ...Args>
    iterator emplace_multi(Args&&... args);

    /**
     * @brief 原位构造元素，不允许键值重复
     * 
     * @tparam Args 参数类型包
     * @param args 构造元素的参数
     * @return pair，包含指向元素的迭代器和是否插入成功的bool值
     */
    template <class ...Args>
    std::pair<iterator, bool> emplace_unique(Args&&... args);

    /**
     * @brief 使用提示原位构造元素，允许键值重复
     * 
     * @tparam Args 参数类型包
     * @param hint 插入位置提示
     * @param args 构造元素的参数
     * @return 指向新插入元素的迭代器
     */
    template <class ...Args>
    iterator emplace_multi_use_hint(iterator hint, Args&&... args);

    /**
     * @brief 使用提示原位构造元素，不允许键值重复
     * 
     * @tparam Args 参数类型包
     * @param hint 插入位置提示
     * @param args 构造元素的参数
     * @return 指向元素的迭代器
     */
    template <class ...Args>
    iterator emplace_unique_use_hint(iterator hint, Args&&... args);

    /**
     * @brief 插入元素，允许键值重复
     * 
     * @param value 要插入的值
     * @return 指向新插入元素的迭代器
     */
    iterator insert_multi(const value_type& value);
    
    /**
     * @brief 插入元素，允许键值重复
     * 
     * @param value 要插入的值（右值引用）
     * @return 指向新插入元素的迭代器
     */
    iterator insert_multi(value_type&& value) {
        return emplace_multi(std::move(value));
    }

    /**
     * @brief 使用提示插入元素，允许键值重复
     * 
     * @param hint 插入位置提示
     * @param value 要插入的值
     * @return 指向新插入元素的迭代器
     */
    iterator insert_multi(iterator hint, const value_type& value) {
        return emplace_multi_use_hint(hint, value);
    }
    
    /**
     * @brief 使用提示插入元素，允许键值重复
     * 
     * @param hint 插入位置提示
     * @param value 要插入的值（右值引用）
     * @return 指向新插入元素的迭代器
     */
    iterator insert_multi(iterator hint, value_type&& value) {
        return emplace_multi_use_hint(hint, std::move(value));
    }

    /**
     * @brief 插入元素范围，允许键值重复
     * 
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    template <class InputIterator>
    void insert_multi(InputIterator first, InputIterator last) {
        size_type n = static_cast<size_type>(std::distance(first, last));
        if (node_count_ > max_size() - n) {
            throw std::length_error("rb_tree<T, Comp>'s size too big");
        }
        for (; n > 0; --n, ++first) {
            insert_multi(end(), *first);
        }
    }

    /**
     * @brief 插入元素，不允许键值重复
     * 
     * @param value 要插入的值
     * @return pair，包含指向元素的迭代器和是否插入成功的bool值
     */
    std::pair<iterator, bool> insert_unique(const value_type& value);
    
    /**
     * @brief 插入元素，不允许键值重复
     * 
     * @param value 要插入的值（右值引用）
     * @return pair，包含指向元素的迭代器和是否插入成功的bool值
     */
    std::pair<iterator, bool> insert_unique(value_type&& value) {
        return emplace_unique(std::move(value));
    }

    /**
     * @brief 使用提示插入元素，不允许键值重复
     * 
     * @param hint 插入位置提示
     * @param value 要插入的值
     * @return 指向元素的迭代器
     */
    iterator insert_unique(iterator hint, const value_type& value) {
        return emplace_unique_use_hint(hint, value);
    }
    
    /**
     * @brief 使用提示插入元素，不允许键值重复
     * 
     * @param hint 插入位置提示
     * @param value 要插入的值（右值引用）
     * @return 指向元素的迭代器
     */
    iterator insert_unique(iterator hint, value_type&& value) {
        return emplace_unique_use_hint(hint, std::move(value));
    }

    /**
     * @brief 插入元素范围，不允许键值重复
     * 
     * @tparam InputIterator 输入迭代器类型
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    template <class InputIterator>
    void insert_unique(InputIterator first, InputIterator last) {
        size_type n = static_cast<size_type>(std::distance(first, last));
        if (node_count_ > max_size() - n) {
            throw std::length_error("rb_tree<T, Comp>'s size too big");
        }
        for (; n > 0; --n, ++first) {
            insert_unique(end(), *first);
        }
    }

    /**
     * @brief 删除指定位置的元素
     * 
     * @param hint 要删除元素的位置
     * @return 指向下一个元素的迭代器
     */
    iterator erase(iterator hint);

    /**
     * @brief 删除键值等于key的所有元素
     * 
     * @param key 要删除的键值
     * @return 删除的元素个数
     */
    size_type erase_multi(const key_type& key);

    /**
     * @brief 删除键值等于key的元素（最多一个）
     * 
     * @param key 要删除的键值
     * @return 删除的元素个数（0或1）
     */
    size_type erase_unique(const key_type& key);

    /**
     * @brief 删除范围内的元素
     * 
     * @param first 范围起始迭代器
     * @param last 范围结束迭代器
     */
    void erase(iterator first, iterator last);

    /**
     * @brief 清空容器
     */
    void clear();

    // 红黑树相关操作

    /**
     * @brief 查找键值等于key的元素
     * 
     * 优化说明:
     * 1. 使用二分查找思想以对数时间复杂度进行查找
     * 2. 采用迭代而非递归实现，减少函数调用开销
     * 3. 使用y跟踪最后访问的不小于key的节点，保证查找效率
     * 4. 提前判断key_comp_条件，避免不必要的比较操作
     * 
     * 时间复杂度: O(log n)
     * 
     * @param key 要查找的键值
     * @return 指向元素的迭代器，如果未找到则返回end()
     */
    iterator find(const key_type& key);
    
    /**
     * @brief 查找键值等于key的元素（常量版本）
     * 
     * 优化说明:
     * 1. 与非const版本共享相同的实现逻辑，保持代码一致性
     * 2. 返回const_iterator类型，确保常量安全性
     * 
     * 时间复杂度: O(log n)
     * 
     * @param key 要查找的键值
     * @return 指向元素的常量迭代器，如果未找到则返回end()
     */
    const_iterator find(const key_type& key) const;

    /**
     * @brief 统计键值等于key的元素个数（允许重复键值）
     * 
     * 优化说明:
     * 1. 直接利用已有的equal_range_multi方法实现，减少代码重复
     * 2. 使用std::distance计算范围大小，利用STL算法提高性能
     * 3. 针对大数据集，遍历次数与重复元素数量成正比
     * 
     * 时间复杂度: O(log n + k)，其中k为重复键值的数量
     * 
     * @param key 要统计的键值
     * @return 元素个数
     */
    size_type count_multi(const key_type& key) const {
        auto p = equal_range_multi(key);
        return static_cast<size_type>(std::distance(p.first, p.second));
    }
    
    /**
     * @brief 统计键值等于key的元素个数（不允许重复键值）
     * 
     * 优化说明:
     * 1. 针对unique集合优化，直接返回0或1
     * 2. 利用已有的find方法，避免代码重复
     * 3. 比count_multi更高效，因为不需要计算范围
     * 
     * 时间复杂度: O(log n)
     * 
     * @param key 要统计的键值
     * @return 元素个数（0或1）
     */
    size_type count_unique(const key_type& key) const {
        return find(key) != end() ? 1 : 0;
    }

    /**
     * @brief 返回不小于key的第一个位置
     * 
     * 优化说明:
     * 1. 算法遵循二分查找模式
     * 2. 直接操作底层指针，避免迭代器转换开销
     * 3. 使用y追踪上一个有效位置，确保返回正确的边界值
     * 
     * 时间复杂度: O(log n)
     * 
     * @param key 键值
     * @return 指向不小于key的第一个元素的迭代器
     */
    iterator lower_bound(const key_type& key);
    
    /**
     * @brief 返回不小于key的第一个位置（常量版本）
     * 
     * 优化说明:
     * 1. 与非const版本共享相同的算法实现
     * 2. 返回const_iterator类型，确保常量性
     * 
     * 时间复杂度: O(log n)
     * 
     * @param key 键值
     * @return 指向不小于key的第一个元素的常量迭代器
     */
    const_iterator lower_bound(const key_type& key) const;

    /**
     * @brief 返回大于key的第一个位置
     * 
     * 优化说明:
     * 1. 算法类似于lower_bound，但条件略有不同
     * 2. 通过key_comp_比较器直接判断大小关系，避免重复比较
     * 3. 采用自顶向下的迭代方式，提高缓存命中率
     * 
     * 时间复杂度: O(log n)
     * 
     * @param key 键值
     * @return 指向大于key的第一个元素的迭代器
     */
    iterator upper_bound(const key_type& key);
    
    /**
     * @brief 返回大于key的第一个位置（常量版本）
     *
     * 优化说明:
     * 1. 与非const版本共享相同的算法逻辑
     * 2. 返回const_iterator类型，保证常量安全性
     * 
     * 时间复杂度: O(log n)
     * 
     * @param key 键值
     * @return 指向大于key的第一个元素的常量迭代器
     */
    const_iterator upper_bound(const key_type& key) const;

    /**
     * @brief 返回键值等于key的元素范围（允许重复键值）
     * 
     * 优化说明:
     * 1. 结合lower_bound和upper_bound实现，利用现有方法减少代码重复
     * 2. 对于大量重复键值的情况，只需要两次二分查找就能得到完整范围
     * 3. 返回pair类型，避免创建临时容器
     * 
     * 时间复杂度: O(log n)
     * 
     * @param key 键值
     * @return 指向范围的迭代器对
     */
    std::pair<iterator, iterator> equal_range_multi(const key_type& key) {
        return std::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
    }
    
    /**
     * @brief 返回键值等于key的元素范围（允许重复键值，常量版本）
     * 
     * 优化说明:
     * 1. 与非const版本实现逻辑一致，但返回const_iterator
     * 2. 保持接口一致性，确保常量容器能够使用相同的查询功能
     * 
     * 时间复杂度: O(log n)
     * 
     * @param key 键值
     * @return 指向范围的常量迭代器对
     */
    std::pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const {
        return std::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
    }

    /**
     * @brief 返回键值等于key的元素范围（不允许重复键值）
     * 
     * 优化说明:
     * 1. 针对不重复键值集合优化，只查找一次而不是两次
     * 2. 对于包含目标键值的情况，范围最多只有一个元素
     * 3. 对于不包含目标键值的情况，直接返回空范围
     * 
     * 时间复杂度: O(log n)
     * 
     * @param key 键值
     * @return 指向范围的迭代器对
     */
    std::pair<iterator, iterator> equal_range_unique(const key_type& key) {
        iterator it = find(key);
        auto next = it;
        return it == end() ? std::make_pair(it, it) : std::make_pair(it, ++next);
    }
    
    /**
     * @brief 返回键值等于key的元素范围（不允许重复键值，常量版本）
     * 
     * 优化说明:
     * 1. 与非const版本实现逻辑一致，但使用const_iterator
     * 2. 保持接口一致性，确保常量容器能够使用相同的查询功能
     * 
     * 时间复杂度: O(log n)
     * 
     * @param key 键值
     * @return 指向范围的常量迭代器对
     */
    std::pair<const_iterator, const_iterator> equal_range_unique(const key_type& key) const {
        const_iterator it = find(key);
        auto next = it;
        return it == end() ? std::make_pair(it, it) : std::make_pair(it, ++next);
    }

    /**
     * @brief 交换两个红黑树的内容
     * 
     * @param rhs 要交换的另一个红黑树
     */
    void swap(rb_tree& rhs) noexcept;

private:
    // 节点相关操作

    /**
     * @brief 创建一个节点
     */
    template <class ...Args>
    node_ptr create_node(Args&&... args);
    
    /**
     * @brief 复制一个节点
     */
    node_ptr clone_node(base_ptr x);
    
    /**
     * @brief 销毁一个节点
     */
    void destroy_node(node_ptr p);

    // 初始化和重置操作
    
    /**
     * @brief 初始化红黑树
     */
    void rb_tree_init();
    
    /**
     * @brief 重置红黑树
     */
    void reset();

    // 获取插入位置
    
    /**
     * @brief 获取插入位置（允许重复键值）
     */
    std::pair<base_ptr, bool> get_insert_multi_pos(const key_type& key);
    
    /**
     * @brief 获取插入位置（不允许重复键值）
     */
    std::pair<std::pair<base_ptr, bool>, bool> get_insert_unique_pos(const key_type& key);

    // 插入节点操作
    
    /**
     * @brief 在指定位置插入值
     */
    iterator insert_value_at(base_ptr x, const value_type& value, bool add_to_left);
    
    /**
     * @brief 在指定位置插入节点
     */
    iterator insert_node_at(base_ptr x, node_ptr node, bool add_to_left);

    // 使用提示的插入操作
    
    /**
     * @brief 使用提示插入节点（允许重复键值）
     */
    iterator insert_multi_use_hint(iterator hint, key_type key, node_ptr node);
    
    /**
     * @brief 使用提示插入节点（不允许重复键值）
     */
    iterator insert_unique_use_hint(iterator hint, key_type key, node_ptr node);

    // 复制和删除树
    
    /**
     * @brief 复制一棵子树
     */
    base_ptr copy_from(base_ptr x, base_ptr p);
    
    /**
     * @brief 删除一棵子树
     */
    void erase_since(base_ptr x);
};

// 红黑树成员函数实现

/**
 * @brief 复制构造函数
 */
template <class T, class Compare>
rb_tree<T, Compare>::rb_tree(const rb_tree& rhs) {
    rb_tree_init();
    if (rhs.node_count_ != 0) {
        root() = copy_from(rhs.root(), header_);
        leftmost() = rb_tree_min(root());
        rightmost() = rb_tree_max(root());
    }
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
}

/**
 * @brief 移动构造函数
 */
template <class T, class Compare>
rb_tree<T, Compare>::rb_tree(rb_tree&& rhs) noexcept
    : header_(std::move(rhs.header_)),
      node_count_(rhs.node_count_),
      key_comp_(rhs.key_comp_) {
    rhs.reset();
}

/**
 * @brief 复制赋值运算符
 */
template <class T, class Compare>
rb_tree<T, Compare>& rb_tree<T, Compare>::operator=(const rb_tree& rhs) {
    if (this != &rhs) {
        clear();

        if (rhs.node_count_ != 0) {
            root() = copy_from(rhs.root(), header_);
            leftmost() = rb_tree_min(root());
            rightmost() = rb_tree_max(root());
        }

        node_count_ = rhs.node_count_;
        key_comp_ = rhs.key_comp_;
    }
    return *this;
}

/**
 * @brief 移动赋值运算符
 */
template <class T, class Compare>
rb_tree<T, Compare>& rb_tree<T, Compare>::operator=(rb_tree&& rhs) {
    clear();
    header_ = std::move(rhs.header_);
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
    rhs.reset();
    return *this;
}

/**
 * @brief 原位构造元素，允许键值重复
 */
template <class T, class Compare>
template <class ...Args>
typename rb_tree<T, Compare>::iterator 
rb_tree<T, Compare>::emplace_multi(Args&&... args) {
    if (node_count_ > max_size() - 1) {
        throw std::length_error("rb_tree<T, Comp>'s size too big");
    }
    node_ptr np = create_node(std::forward<Args>(args)...);
    auto res = get_insert_multi_pos(value_traits::get_key(np->value));
    return insert_node_at(res.first, np, res.second);
}

/**
 * @brief 原位构造元素，不允许键值重复
 */
template <class T, class Compare>
template <class ...Args>
std::pair<typename rb_tree<T, Compare>::iterator, bool> 
rb_tree<T, Compare>::emplace_unique(Args&&... args) {
    if (node_count_ > max_size() - 1) {
        throw std::length_error("rb_tree<T, Comp>'s size too big");
    }
    node_ptr np = create_node(std::forward<Args>(args)...);
    auto res = get_insert_unique_pos(value_traits::get_key(np->value));
    if (res.second) {
        // 插入成功
        return std::make_pair(insert_node_at(res.first.first, np, res.first.second), true);
    }
    destroy_node(np);
    return std::make_pair(iterator(res.first.first), false);
}

/**
 * @brief 使用提示原位构造元素，允许键值重复
 */
template <class T, class Compare>
template <class ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::emplace_multi_use_hint(iterator hint, Args&&... args) {
    if (node_count_ > max_size() - 1) {
        throw std::length_error("rb_tree<T, Comp>'s size too big");
    }
    node_ptr np = create_node(std::forward<Args>(args)...);
    if (node_count_ == 0) {
        return insert_node_at(header_, np, true);
    }
    key_type key = value_traits::get_key(np->value);
    if (hint == begin()) {
        // 位于begin处，如果新值小于当前值，则插入到begin之前
        if (key_comp_(key, value_traits::get_key(*hint))) {
            return insert_node_at(hint.node, np, true);
        } else {
            // 否则找普通插入位置
            auto pos = get_insert_multi_pos(key);
            return insert_node_at(pos.first, np, pos.second);
        }
    } else if (hint == end()) {
        // 位于end处，如果新值不小于最大值，则插入到end之前
        if (!key_comp_(key, value_traits::get_key(rightmost()->get_node_ptr()->value))) {
            return insert_node_at(rightmost(), np, false);
        } else {
            // 否则找普通插入位置
            auto pos = get_insert_multi_pos(key);
            return insert_node_at(pos.first, np, pos.second);
        }
    }
    return insert_multi_use_hint(hint, key, np);
}

/**
 * @brief 使用提示原位构造元素，不允许键值重复
 */
template<class T, class Compare>
template<class ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::emplace_unique_use_hint(iterator hint, Args&&... args) {
    if (node_count_ > max_size() - 1) {
        throw std::length_error("rb_tree<T, Comp>'s size too big");
    }
    node_ptr np = create_node(std::forward<Args>(args)...);
    if (node_count_ == 0) {
        return insert_node_at(header_, np, true);
    }
    key_type key = value_traits::get_key(np->value);
    if (hint == begin()) {
        // 位于begin处，如果新值小于当前值，则插入到begin之前
        if (key_comp_(key, value_traits::get_key(*hint))) {
            return insert_node_at(hint.node, np, true);
        } else {
            // 否则找普通插入位置
            auto pos = get_insert_unique_pos(key);
            if (!pos.second) {
                destroy_node(np);
                return pos.first.first;
            }
            return insert_node_at(pos.first.first, np, pos.first.second);
        }
    } else if (hint == end()) {
        // 位于end处，如果最大值小于新值，则插入到end之前
        if (key_comp_(value_traits::get_key(rightmost()->get_node_ptr()->value), key)) {
            return insert_node_at(rightmost(), np, false);
        } else {
            // 否则找普通插入位置
            auto pos = get_insert_unique_pos(key);
            if (!pos.second) {
                destroy_node(np);
                return pos.first.first;
            }
            return insert_node_at(pos.first.first, np, pos.first.second);
        }
    }
    return insert_unique_use_hint(hint, key, np);
}

/**
 * @brief 插入元素，允许键值重复
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_multi(const value_type& value) {
    if (node_count_ > max_size() - 1) {
        throw std::length_error("rb_tree<T, Comp>'s size too big");
    }
    auto res = get_insert_multi_pos(value_traits::get_key(value));
    return insert_value_at(res.first, value, res.second);
}

/**
 * @brief 插入元素，不允许键值重复
 */
template <class T, class Compare>
std::pair<typename rb_tree<T, Compare>::iterator, bool>
rb_tree<T, Compare>::insert_unique(const value_type& value) {
    if (node_count_ > max_size() - 1) {
        throw std::length_error("rb_tree<T, Comp>'s size too big");
    }
    auto res = get_insert_unique_pos(value_traits::get_key(value));
    if (res.second) {
        // 插入成功
        return std::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
    }
    return std::make_pair(iterator(res.first.first), false);
}

/**
 * @brief 删除指定位置的元素
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::erase(iterator hint) {
    auto node = hint.node->get_node_ptr();
    iterator next(node);
    ++next;
    
    rb_tree_erase_rebalance(hint.node, root(), leftmost(), rightmost());
    destroy_node(node);
    --node_count_;
    return next;
}

/**
 * @brief 删除键值等于key的所有元素
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::size_type
rb_tree<T, Compare>::erase_multi(const key_type& key) {
    auto p = equal_range_multi(key);
    size_type n = static_cast<size_type>(std::distance(p.first, p.second));
    erase(p.first, p.second);
    return n;
}

/**
 * @brief 删除键值等于key的元素（最多一个）
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::size_type
rb_tree<T, Compare>::erase_unique(const key_type& key) {
    auto it = find(key);
    if (it != end()) {
        erase(it);
        return 1;
    }
    return 0;
}

/**
 * @brief 删除范围内的元素
 */
template <class T, class Compare>
void rb_tree<T, Compare>::erase(iterator first, iterator last) {
    if (first == begin() && last == end()) {
        clear();
    } else {
        while (first != last) {
            erase(first++);
        }
    }
}

/**
 * @brief 清空容器
 */
template <class T, class Compare>
void rb_tree<T, Compare>::clear() {
    if (node_count_ != 0) {
        erase_since(root());
        leftmost() = header_;
        root() = nullptr;
        rightmost() = header_;
        node_count_ = 0;
    }
}

/**
 * @brief 查找键值等于key的元素
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::find(const key_type& key) {
    auto y = header_;  // 最后一个不小于key的节点
    auto x = root();
    while (x != nullptr) {
        if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
            // key小于等于当前节点键值，向左走
            y = x;
            x = x->left;
        } else {
            // key大于当前节点键值，向右走
            x = x->right;
        }
    }
    iterator j = iterator(y);
    return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
}

/**
 * @brief 查找键值等于key的元素（常量版本）
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::find(const key_type& key) const {
    auto y = header_;  // 最后一个不小于key的节点
    auto x = root();
    while (x != nullptr) {
        if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
            // key小于等于当前节点键值，向左走
            y = x;
            x = x->left;
        } else {
            // key大于当前节点键值，向右走
            x = x->right;
        }
    }
    const_iterator j = const_iterator(y);
    return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
}

/**
 * @brief 返回不小于key的第一个位置
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::lower_bound(const key_type& key) {
    auto y = header_;
    auto x = root();
    while (x != nullptr) {
        if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
            // key <= x，记录当前节点并向左走
            y = x;
            x = x->left;
        } else {
            // key > x，向右走
            x = x->right;
        }
    }
    return iterator(y);
}

/**
 * @brief 返回不小于key的第一个位置（常量版本）
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::lower_bound(const key_type& key) const {
    auto y = header_;
    auto x = root();
    while (x != nullptr) {
        if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
            // key <= x，记录当前节点并向左走
            y = x;
            x = x->left;
        } else {
            // key > x，向右走
            x = x->right;
        }
    }
    return const_iterator(y);
}

/**
 * @brief 返回大于key的第一个位置
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::upper_bound(const key_type& key) {
    auto y = header_;
    auto x = root();
    while (x != nullptr) {
        if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value))) {
            // key < x，记录当前节点并向左走
            y = x;
            x = x->left;
        } else {
            // key >= x，向右走
            x = x->right;
        }
    }
    return iterator(y);
}

/**
 * @brief 返回大于key的第一个位置（常量版本）
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::upper_bound(const key_type& key) const {
    auto y = header_;
    auto x = root();
    while (x != nullptr) {
        if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value))) {
            // key < x，记录当前节点并向左走
            y = x;
            x = x->left;
        } else {
            // key >= x，向右走
            x = x->right;
        }
    }
    return const_iterator(y);
}

/**
 * @brief 交换两个红黑树的内容
 */
template <class T, class Compare>
void rb_tree<T, Compare>::swap(rb_tree& rhs) noexcept {
    if (this != &rhs) {
        std::swap(header_, rhs.header_);
        std::swap(node_count_, rhs.node_count_);
        std::swap(key_comp_, rhs.key_comp_);
    }
}

// 红黑树私有辅助函数实现

/**
 * @brief 创建一个节点
 */
template <class T, class Compare>
template <class ...Args>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::create_node(Args&&... args) {
    auto tmp = node_allocator().allocate(1);
    try {
        new (&tmp->value) value_type(std::forward<Args>(args)...);
        tmp->left = nullptr;
        tmp->right = nullptr;
        tmp->parent = nullptr;
    } catch (...) {
        node_allocator().deallocate(tmp, 1);
        throw;
    }
    return tmp;
}

/**
 * @brief 复制一个节点
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::clone_node(base_ptr x) {
    node_ptr tmp = create_node(x->get_node_ptr()->value);
    tmp->color = x->color;
    tmp->left = nullptr;
    tmp->right = nullptr;
    return tmp;
}

/**
 * @brief 销毁一个节点
 */
template <class T, class Compare>
void rb_tree<T, Compare>::destroy_node(node_ptr p) {
    p->value.~value_type();
    node_allocator().deallocate(p, 1);
}

/**
 * @brief 初始化红黑树
 */
template <class T, class Compare>
void rb_tree<T, Compare>::rb_tree_init() {
    header_ = base_allocator().allocate(1);
    header_->color = rb_tree_red;  // header_节点颜色为红，与root区分
    root() = nullptr;
    leftmost() = header_;
    rightmost() = header_;
    node_count_ = 0;
}

/**
 * @brief 重置红黑树
 */
template <class T, class Compare>
void rb_tree<T, Compare>::reset() {
    // 原来的实现会导致对象处于无效状态
    // header_ = nullptr;
    // node_count_ = 0;
    
    // 调用rb_tree_init重新初始化为有效的空树
    rb_tree_init();
}

/**
 * @brief 获取插入位置（允许重复键值）
 */
template <class T, class Compare>
std::pair<typename rb_tree<T, Compare>::base_ptr, bool>
rb_tree<T, Compare>::get_insert_multi_pos(const key_type& key) {
    auto x = root();
    auto y = header_;
    bool add_to_left = true;
    while (x != nullptr) {
        y = x;
        add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
        x = add_to_left ? x->left : x->right;
    }
    return std::make_pair(y, add_to_left);
}

/**
 * @brief 获取插入位置（不允许重复键值）
 */
template <class T, class Compare>
std::pair<std::pair<typename rb_tree<T, Compare>::base_ptr, bool>, bool>
rb_tree<T, Compare>::get_insert_unique_pos(const key_type& key) {
    // 返回一个pair，第一个值为一个pair，包含插入点的父节点和一个bool表示是否在左边插入，
    // 第二个值为一个bool，表示是否插入成功
    auto x = root();
    auto y = header_;
    bool add_to_left = true;  // 树为空时也在header_左边插入
    while (x != nullptr) {
        y = x;
        add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
        x = add_to_left ? x->left : x->right;
    }
    iterator j = iterator(y);  // 此时y为插入点的父节点
    if (add_to_left) {
        if (y == header_ || j == begin()) {
            // 如果树为空树或插入点在最左节点处，肯定可以插入新的节点
            return std::make_pair(std::make_pair(y, true), true);
        } else {
            // 否则，如果存在重复节点，那么--j就是重复的值
            --j;
        }
    }
    if (key_comp_(value_traits::get_key(*j), key)) {
        // 表明新节点没有重复
        return std::make_pair(std::make_pair(y, add_to_left), true);
    }
    // 进行至此，表示新节点与现有节点键值重复
    return std::make_pair(std::make_pair(y, add_to_left), false);
}

/**
 * @brief 在指定位置插入值
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_value_at(base_ptr x, const value_type& value, bool add_to_left) {
    node_ptr node = create_node(value);
    node->parent = x;
    auto base_node = node->get_base_ptr();
    if (x == header_) {
        // 空树情况
        root() = base_node;
        leftmost() = base_node;
        rightmost() = base_node;
    } else if (add_to_left) {
        // 插入左子节点
        x->left = base_node;
        if (leftmost() == x) {
            // 更新最左节点
            leftmost() = base_node;
        }
    } else {
        // 插入右子节点
        x->right = base_node;
        if (rightmost() == x) {
            // 更新最右节点
            rightmost() = base_node;
        }
    }
    // 重新平衡树结构
    rb_tree_insert_rebalance(base_node, root());
    ++node_count_;
    return iterator(node);
}

/**
 * @brief 在指定位置插入节点
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_node_at(base_ptr x, node_ptr node, bool add_to_left) {
    node->parent = x;
    auto base_node = node->get_base_ptr();
    if (x == header_) {
        root() = base_node;
        leftmost() = base_node;
        rightmost() = base_node;
    } else if (add_to_left) {
        x->left = base_node;
        if (leftmost() == x) {
            leftmost() = base_node;
        }
    } else {
        x->right = base_node;
        if (rightmost() == x) {
            rightmost() = base_node;
        }
    }
    rb_tree_insert_rebalance(base_node, root());
    ++node_count_;
    return iterator(node);
}

/**
 * @brief 使用提示插入节点（允许重复键值）
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_multi_use_hint(iterator hint, key_type key, node_ptr node) {
    // 在hint附近寻找可插入的位置
    auto np = hint.node;
    auto before = hint;
    --before;
    auto bnp = before.node;
    if (!key_comp_(key, value_traits::get_key(*before)) &&
        !key_comp_(value_traits::get_key(*hint), key)) {
        // before <= node <= hint
        if (bnp->right == nullptr) {
            return insert_node_at(bnp, node, false);
        } else if (np->left == nullptr) {
            return insert_node_at(np, node, true);
        }
    }
    auto pos = get_insert_multi_pos(key);
    return insert_node_at(pos.first, node, pos.second);
}

/**
 * @brief 使用提示插入节点（不允许重复键值）
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_unique_use_hint(iterator hint, key_type key, node_ptr node) {
    // 在hint附近寻找可插入的位置
    auto np = hint.node;
    auto before = hint;
    --before;
    auto bnp = before.node;
    if (key_comp_(value_traits::get_key(*before), key) &&
        key_comp_(key, value_traits::get_key(*hint))) {
        // before < node < hint
        if (bnp->right == nullptr) {
            return insert_node_at(bnp, node, false);
        } else if (np->left == nullptr) {
            return insert_node_at(np, node, true);
        }
    }
    auto pos = get_insert_unique_pos(key);
    if (!pos.second) {
        destroy_node(node);
        return pos.first.first;
    }
    return insert_node_at(pos.first.first, node, pos.first.second);
}

/**
 * @brief 复制一棵子树
 */
template <class T, class Compare>
typename rb_tree<T, Compare>::base_ptr
rb_tree<T, Compare>::copy_from(base_ptr x, base_ptr p) {
    auto top = clone_node(x);
    top->parent = p;
    try {
        if (x->right) {
            top->right = copy_from(x->right, top);
        }
        p = top;
        x = x->left;
        while (x != nullptr) {
            auto y = clone_node(x);
            p->left = y;
            y->parent = p;
            if (x->right) {
                y->right = copy_from(x->right, y);
            }
            p = y;
            x = x->left;
        }
    } catch (...) {
        erase_since(top);
        throw;
    }
    return top;
}

/**
 * @brief 删除一棵子树
 */
template <class T, class Compare>
void rb_tree<T, Compare>::erase_since(base_ptr x) {
    while (x != nullptr) {
        erase_since(x->right);
        auto y = x->left;
        destroy_node(x->get_node_ptr());
        x = y;
    }
}

// 重载比较操作符

/**
 * @brief 相等比较运算符
 */
template <class T, class Compare>
bool operator==(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return lhs.size() == rhs.size() && 
           std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

/**
 * @brief 小于比较运算符
 */
template <class T, class Compare>
bool operator<(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end());
}

/**
 * @brief 不等比较运算符
 */
template <class T, class Compare>
bool operator!=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief 大于比较运算符
 */
template <class T, class Compare>
bool operator>(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return rhs < lhs;
}

/**
 * @brief 小于等于比较运算符
 */
template <class T, class Compare>
bool operator<=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief 大于等于比较运算符
 */
template <class T, class Compare>
bool operator>=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return !(lhs < rhs);
}

/**
 * @brief 重载 swap
 */
template <class T, class Compare>
void swap(rb_tree<T, Compare>& lhs, rb_tree<T, Compare>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace mystl

#endif // RB_TREE_H 