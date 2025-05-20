#ifndef MYSTL_STRING_H_
#define MYSTL_STRING_H_

/**
 * @file string.h
 * @brief 实现C++11标准的string类
 * 
 * 该文件实现了一个完整的string类，与标准库string类功能相同
 */

#include <iostream>
#include <cstring>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <memory>
#include <initializer_list>
#include <stdexcept>
#include <limits>

namespace mystl {

// ------------------------------------------------------------------------------------------
// char_traits类，定义字符特性操作
// ------------------------------------------------------------------------------------------

/**
 * @brief 字符特性模板类
 * 
 * @tparam CharType 字符类型
 */
template <class CharType>
struct char_traits {
    typedef CharType char_type;
    
    /**
     * @brief 计算字符串长度
     * 
     * @param str 字符串
     * @return size_t 字符串长度
     */
    static size_t length(const char_type* str) noexcept {
        size_t len = 0;
        for (; *str != char_type(0); ++str)
            ++len;
        return len;
    }
    
    /**
     * @brief 比较两个字符串
     * 
     * @param s1 第一个字符串
     * @param s2 第二个字符串
     * @param n 比较的长度
     * @return int 比较结果
     */
    static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept {
        for (; n != 0; --n, ++s1, ++s2) {
            if (*s1 < *s2) return -1;
            if (*s2 < *s1) return 1;
        }
        return 0;
    }
    
    /**
     * @brief 拷贝字符串
     * 
     * @param dst 目标地址
     * @param src 源地址
     * @param n 拷贝长度
     * @return char_type* 目标地址
     */
    static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept {
        char_type* r = dst;
        for (; n != 0; --n, ++dst, ++src)
            *dst = *src;
        return r;
    }
    
    /**
     * @brief 移动字符串（解决重叠问题）
     * 
     * @param dst 目标地址
     * @param src 源地址
     * @param n 移动长度
     * @return char_type* 目标地址
     */
    static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept {
        char_type* r = dst;
        if (dst < src) {
            for (; n != 0; --n, ++dst, ++src)
                *dst = *src;
        } else if (src < dst) {
            dst += n;
            src += n;
            for (; n != 0; --n)
                *--dst = *--src;
        }
        return r;
    }
    
    /**
     * @brief 填充字符
     * 
     * @param dst 目标地址
     * @param ch 填充字符
     * @param count 填充数量
     * @return char_type* 目标地址
     */
    static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept {
        char_type* r = dst;
        for (; count > 0; --count, ++dst)
            *dst = ch;
        return r;
    }
    
    /**
     * @brief 查找字符
     * 
     * @param s 字符串
     * @param n 查找范围
     * @param c 查找的字符
     * @return const char_type* 找到的位置
     */
    static const char_type* find(const char_type* s, size_t n, const char_type& c) noexcept {
        for (; n != 0; --n, ++s) {
            if (*s == c)
                return s;
        }
        return nullptr;
    }
};

/**
 * @brief char_traits特化版本 - char
 */
template <>
struct char_traits<char> {
    typedef char char_type;
    
    /**
     * @brief 计算字符串长度
     */
    static size_t length(const char_type* str) noexcept {
        return std::strlen(str);
    }
    
    /**
     * @brief 比较两个字符串
     */
    static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept {
        return std::memcmp(s1, s2, n);
    }
    
    /**
     * @brief 拷贝字符串
     */
    static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept {
        return static_cast<char_type*>(std::memcpy(dst, src, n));
    }
    
    /**
     * @brief 移动字符串
     */
    static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept {
        return static_cast<char_type*>(std::memmove(dst, src, n));
    }
    
    /**
     * @brief 填充字符
     */
    static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept {
        return static_cast<char_type*>(std::memset(dst, ch, count));
    }
    
    /**
     * @brief 查找字符
     */
    static const char_type* find(const char_type* s, size_t n, const char_type& c) noexcept {
        return static_cast<const char_type*>(std::memchr(s, c, n));
    }
};

/**
 * @brief char_traits特化版本 - wchar_t
 */
template <>
struct char_traits<wchar_t> {
    typedef wchar_t char_type;
    
    /**
     * @brief 计算字符串长度
     */
    static size_t length(const char_type* str) noexcept {
        return std::wcslen(str);
    }
    
    /**
     * @brief 比较两个字符串
     */
    static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept {
        return std::wmemcmp(s1, s2, n);
    }
    
    /**
     * @brief 拷贝字符串
     */
    static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept {
        return std::wmemcpy(dst, src, n);
    }
    
    /**
     * @brief 移动字符串
     */
    static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept {
        return std::wmemmove(dst, src, n);
    }
    
    /**
     * @brief 填充字符
     */
    static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept {
        return std::wmemset(dst, ch, count);
    }
    
    /**
     * @brief 查找字符
     */
    static const char_type* find(const char_type* s, size_t n, const char_type& c) noexcept {
        for (; n != 0; --n, ++s) {
            if (*s == c)
                return s;
        }
        return nullptr;
    }
};

// 字符串默认初始大小（可调整）
constexpr size_t STRING_INIT_SIZE = 16;

// ------------------------------------------------------------------------------------------
// basic_string类模板
// ------------------------------------------------------------------------------------------

/**
 * @brief 字符串类模板
 * 
 * @tparam CharT 字符类型
 * @tparam Traits 字符特性类
 * @tparam Alloc 分配器类型
 */
template <class CharT, class Traits = char_traits<CharT>, class Alloc = std::allocator<CharT>>
class basic_string {
public:
    // 类型定义
    using traits_type = Traits;
    using value_type = CharT;
    using allocator_type = Alloc;
    using size_type = typename std::allocator_traits<Alloc>::size_type;
    using difference_type = typename std::allocator_traits<Alloc>::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    // 特殊值，表示"未找到"或"全部"
    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    // 实现细节
    struct Rep {
        size_type capacity;  // 容量
        size_type size;      // 大小
        
        // 获取字符数组起始位置
        CharT* data() noexcept {
            return reinterpret_cast<CharT*>(this + 1);
        }
        
        const CharT* data() const noexcept {
            return reinterpret_cast<const CharT*>(this + 1);
        }
        
        // 创建新的Rep
        static Rep* create(const Alloc& alloc, size_type capacity) {
            typename std::allocator_traits<Alloc>::template rebind_alloc<Rep> rep_alloc(alloc);
            
            // 计算需要分配的内存大小（Rep结构体 + 字符数组）
            size_type bytes = sizeof(Rep) + sizeof(CharT) * (capacity + 1);
            
            // 分配内存
            void* mem = std::allocator_traits<decltype(rep_alloc)>::allocate(rep_alloc, bytes);
            
            // 构造Rep对象
            Rep* rep = new (mem) Rep;
            rep->capacity = capacity;
            rep->size = 0;
            
            // 末尾添加空字符
            rep->data()[0] = CharT();
            
            return rep;
        }
        
        // 释放Rep
        static void destroy(const Alloc& alloc, Rep* rep) {
            if (rep) {
                typename std::allocator_traits<Alloc>::template rebind_alloc<Rep> rep_alloc(alloc);
                
                // 计算分配的内存大小
                size_type bytes = sizeof(Rep) + sizeof(CharT) * (rep->capacity + 1);
                
                // 调用析构函数
                rep->~Rep();
                
                // 释放内存
                std::allocator_traits<decltype(rep_alloc)>::deallocate(rep_alloc, rep, bytes);
            }
        }
    };
    
    Rep* rep_;          // 指向字符串数据的表示
    Alloc alloc_;       // 分配器

public:
    /**
     * @brief 默认构造函数
     */
    basic_string() noexcept(noexcept(Alloc()))
        : rep_(nullptr), alloc_(Alloc()) {
        rep_ = Rep::create(alloc_, STRING_INIT_SIZE);
    }
    
    /**
     * @brief 带分配器的构造函数
     */
    explicit basic_string(const Alloc& alloc) noexcept
        : rep_(nullptr), alloc_(alloc) {
        rep_ = Rep::create(alloc_, STRING_INIT_SIZE);
    }
    
    /**
     * @brief 从C风格字符串构造
     */
    basic_string(const CharT* s, const Alloc& alloc = Alloc())
        : rep_(nullptr), alloc_(alloc) {
        const size_type len = traits_type::length(s);
        rep_ = Rep::create(alloc_, len);
        traits_type::copy(rep_->data(), s, len);
        rep_->size = len;
        rep_->data()[len] = CharT();
    }
    
    /**
     * @brief 从C风格字符串构造指定长度
     */
    basic_string(const CharT* s, size_type n, const Alloc& alloc = Alloc())
        : rep_(nullptr), alloc_(alloc) {
        rep_ = Rep::create(alloc_, n);
        traits_type::copy(rep_->data(), s, n);
        rep_->size = n;
        rep_->data()[n] = CharT();
    }
    
    /**
     * @brief 填充构造函数
     */
    basic_string(size_type n, CharT c, const Alloc& alloc = Alloc())
        : rep_(nullptr), alloc_(alloc) {
        rep_ = Rep::create(alloc_, n);
        traits_type::fill(rep_->data(), c, n);
        rep_->size = n;
        rep_->data()[n] = CharT();
    }
    
    /**
     * @brief 范围构造函数
     */
    template <class InputIt, typename = typename std::enable_if<
        std::is_convertible<typename std::iterator_traits<InputIt>::iterator_category, 
                           std::input_iterator_tag>::value>::type>
    basic_string(InputIt first, InputIt last, const Alloc& alloc = Alloc())
        : rep_(nullptr), alloc_(alloc) {
        const size_type len = std::distance(first, last);
        rep_ = Rep::create(alloc_, len);
        
        size_type i = 0;
        for (auto it = first; it != last; ++it, ++i) {
            rep_->data()[i] = *it;
        }
        
        rep_->size = len;
        rep_->data()[len] = CharT();
    }
    
    /**
     * @brief 复制构造函数
     */
    basic_string(const basic_string& other)
        : rep_(nullptr), alloc_(std::allocator_traits<Alloc>::select_on_container_copy_construction(other.alloc_)) {
        rep_ = Rep::create(alloc_, other.size());
        traits_type::copy(rep_->data(), other.data(), other.size());
        rep_->size = other.size();
        rep_->data()[rep_->size] = CharT();
    }
    
    /**
     * @brief 带分配器的复制构造函数
     */
    basic_string(const basic_string& other, const Alloc& alloc)
        : rep_(nullptr), alloc_(alloc) {
        rep_ = Rep::create(alloc_, other.size());
        traits_type::copy(rep_->data(), other.data(), other.size());
        rep_->size = other.size();
        rep_->data()[rep_->size] = CharT();
    }
    
    /**
     * @brief 移动构造函数
     */
    basic_string(basic_string&& other) noexcept
        : rep_(other.rep_), alloc_(std::move(other.alloc_)) {
        other.rep_ = nullptr;
    }
    
    /**
     * @brief 带分配器的移动构造函数
     */
    basic_string(basic_string&& other, const Alloc& alloc) noexcept
        : rep_(nullptr), alloc_(alloc) {
        if (alloc_ == other.alloc_) {
            // 分配器相同，直接移动
            rep_ = other.rep_;
            other.rep_ = nullptr;
        } else {
            // 分配器不同，需要重新分配
            rep_ = Rep::create(alloc_, other.size());
            traits_type::copy(rep_->data(), other.data(), other.size());
            rep_->size = other.size();
            rep_->data()[rep_->size] = CharT();
        }
    }
    
    /**
     * @brief 从初始化列表构造
     */
    basic_string(std::initializer_list<CharT> ilist, const Alloc& alloc = Alloc())
        : basic_string(ilist.begin(), ilist.end(), alloc) {
    }
    
    /**
     * @brief 析构函数
     */
    ~basic_string() {
        Rep::destroy(alloc_, rep_);
    }

    /**
     * @brief 复制赋值操作符
     */
    basic_string& operator=(const basic_string& other) {
        if (this != &other) {
            // 判断是否需要更新分配器
            if (std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value) {
                if (alloc_ != other.alloc_) {
                    // 分配器不同，释放旧内存，使用新分配器
                    Rep::destroy(alloc_, rep_);
                    alloc_ = other.alloc_;
                    rep_ = Rep::create(alloc_, other.size());
                } else if (rep_->capacity < other.size()) {
                    // 容量不足，重新分配
                    Rep::destroy(alloc_, rep_);
                    rep_ = Rep::create(alloc_, other.size());
                }
            } else if (rep_->capacity < other.size()) {
                // 容量不足，重新分配
                Rep::destroy(alloc_, rep_);
                rep_ = Rep::create(alloc_, other.size());
            }
            
            // 复制数据
            traits_type::copy(rep_->data(), other.data(), other.size());
            rep_->size = other.size();
            rep_->data()[rep_->size] = CharT();
        }
        return *this;
    }
    
    /**
     * @brief 移动赋值操作符
     */
    basic_string& operator=(basic_string&& other) noexcept(
        std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value ||
        std::allocator_traits<Alloc>::is_always_equal::value) {
        if (this != &other) {
            // 判断是否可以直接移动
            if (alloc_ == other.alloc_ || 
                std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value) {
                // 可以直接移动
                Rep::destroy(alloc_, rep_);
                
                if (std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value) {
                    alloc_ = std::move(other.alloc_);
                }
                
                rep_ = other.rep_;
                other.rep_ = nullptr;
            } else {
                // 需要复制
                if (rep_->capacity < other.size()) {
                    Rep::destroy(alloc_, rep_);
                    rep_ = Rep::create(alloc_, other.size());
                }
                
                traits_type::copy(rep_->data(), other.data(), other.size());
                rep_->size = other.size();
                rep_->data()[rep_->size] = CharT();
            }
        }
        return *this;
    }
    
    /**
     * @brief 从C风格字符串赋值
     */
    basic_string& operator=(const CharT* s) {
        const size_type len = traits_type::length(s);
        
        if (rep_->capacity < len) {
            Rep::destroy(alloc_, rep_);
            rep_ = Rep::create(alloc_, len);
        }
        
        traits_type::copy(rep_->data(), s, len);
        rep_->size = len;
        rep_->data()[len] = CharT();
        
        return *this;
    }
    
    /**
     * @brief 从字符赋值
     */
    basic_string& operator=(CharT c) {
        if (rep_->capacity < 1) {
            Rep::destroy(alloc_, rep_);
            rep_ = Rep::create(alloc_, 1);
        }
        
        rep_->data()[0] = c;
        rep_->size = 1;
        rep_->data()[1] = CharT();
        
        return *this;
    }
    
    /**
     * @brief 从初始化列表赋值
     */
    basic_string& operator=(std::initializer_list<CharT> ilist) {
        return assign(ilist.begin(), ilist.end());
    }
    
    /**
     * @brief 从字符串赋值
     */
    basic_string& assign(const basic_string& str) {
        return *this = str;
    }
    
    /**
     * @brief 从子字符串赋值
     */
    basic_string& assign(const basic_string& str, size_type pos, size_type count = npos) {
        if (pos > str.size()) {
            throw std::out_of_range("basic_string::assign: pos out of range");
        }
        
        size_type len = std::min(count, str.size() - pos);
        
        if (rep_->capacity < len) {
            Rep::destroy(alloc_, rep_);
            rep_ = Rep::create(alloc_, len);
        }
        
        traits_type::copy(rep_->data(), str.data() + pos, len);
        rep_->size = len;
        rep_->data()[len] = CharT();
        
        return *this;
    }
    
    /**
     * @brief 从C风格字符串赋值
     */
    basic_string& assign(const CharT* s, size_type count) {
        if (rep_->capacity < count) {
            Rep::destroy(alloc_, rep_);
            rep_ = Rep::create(alloc_, count);
        }
        
        traits_type::copy(rep_->data(), s, count);
        rep_->size = count;
        rep_->data()[count] = CharT();
        
        return *this;
    }
    
    /**
     * @brief 从C风格字符串赋值
     */
    basic_string& assign(const CharT* s) {
        return *this = s;
    }
    
    /**
     * @brief 填充赋值
     */
    basic_string& assign(size_type count, CharT ch) {
        if (rep_->capacity < count) {
            Rep::destroy(alloc_, rep_);
            rep_ = Rep::create(alloc_, count);
        }
        
        traits_type::fill(rep_->data(), ch, count);
        rep_->size = count;
        rep_->data()[count] = CharT();
        
        return *this;
    }
    
    /**
     * @brief 范围赋值
     */
    template <class InputIt, typename = typename std::enable_if<
        std::is_convertible<typename std::iterator_traits<InputIt>::iterator_category, 
                           std::input_iterator_tag>::value>::type>
    basic_string& assign(InputIt first, InputIt last) {
        const size_type len = std::distance(first, last);
        
        if (rep_->capacity < len) {
            Rep::destroy(alloc_, rep_);
            rep_ = Rep::create(alloc_, len);
        }
        
        size_type i = 0;
        for (auto it = first; it != last; ++it, ++i) {
            rep_->data()[i] = *it;
        }
        
        rep_->size = len;
        rep_->data()[len] = CharT();
        
        return *this;
    }
    
    /**
     * @brief 从初始化列表赋值
     */
    basic_string& assign(std::initializer_list<CharT> ilist) {
        return assign(ilist.begin(), ilist.end());
    }
    
    // 迭代器函数
    
    /**
     * @brief 返回开始位置迭代器
     */
    iterator begin() noexcept {
        return rep_->data();
    }
    
    /**
     * @brief 返回开始位置常量迭代器
     */
    const_iterator begin() const noexcept {
        return rep_->data();
    }
    
    /**
     * @brief 返回结束位置迭代器
     */
    iterator end() noexcept {
        return rep_->data() + rep_->size;
    }
    
    /**
     * @brief 返回结束位置常量迭代器
     */
    const_iterator end() const noexcept {
        return rep_->data() + rep_->size;
    }
    
    /**
     * @brief 返回反向开始位置迭代器
     */
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    
    /**
     * @brief 返回反向开始位置常量迭代器
     */
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    
    /**
     * @brief 返回反向结束位置迭代器
     */
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    
    /**
     * @brief 返回反向结束位置常量迭代器
     */
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    
    /**
     * @brief 返回常量开始位置迭代器
     */
    const_iterator cbegin() const noexcept {
        return begin();
    }
    
    /**
     * @brief 返回常量结束位置迭代器
     */
    const_iterator cend() const noexcept {
        return end();
    }
    
    /**
     * @brief 返回常量反向开始位置迭代器
     */
    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }
    
    /**
     * @brief 返回常量反向结束位置迭代器
     */
    const_reverse_iterator crend() const noexcept {
        return rend();
    }
    
    // 容量相关方法
    
    /**
     * @brief 返回字符串大小
     */
    size_type size() const noexcept {
        return rep_->size;
    }
    
    /**
     * @brief 返回字符串长度（与size相同）
     */
    size_type length() const noexcept {
        return rep_->size;
    }
    
    /**
     * @brief 返回最大可能大小
     */
    size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(CharT) - 1;
    }
    
    /**
     * @brief 调整字符串大小
     */
    void resize(size_type n, CharT c = CharT()) {
        if (n > rep_->capacity) {
            // 需要重新分配空间
            size_type new_capacity = std::max(n, rep_->capacity * 2);
            Rep* new_rep = Rep::create(alloc_, new_capacity);
            
            // 复制原有数据
            traits_type::copy(new_rep->data(), rep_->data(), rep_->size);
            
            // 填充新部分
            if (n > rep_->size) {
                traits_type::fill(new_rep->data() + rep_->size, c, n - rep_->size);
            }
            
            new_rep->size = n;
            new_rep->data()[n] = CharT();
            
            // 释放旧内存
            Rep::destroy(alloc_, rep_);
            rep_ = new_rep;
        } else {
            // 不需要重新分配
            if (n > rep_->size) {
                // 需要填充新部分
                traits_type::fill(rep_->data() + rep_->size, c, n - rep_->size);
            }
            
            rep_->size = n;
            rep_->data()[n] = CharT();
        }
    }
    
    /**
     * @brief 返回当前容量
     */
    size_type capacity() const noexcept {
        return rep_->capacity;
    }
    
    /**
     * @brief 保留预留空间
     */
    void reserve(size_type n = 0) {
        if (n > rep_->capacity) {
            // 需要重新分配
            Rep* new_rep = Rep::create(alloc_, n);
            
            // 复制原有数据
            traits_type::copy(new_rep->data(), rep_->data(), rep_->size);
            new_rep->size = rep_->size;
            new_rep->data()[new_rep->size] = CharT();
            
            // 释放旧内存
            Rep::destroy(alloc_, rep_);
            rep_ = new_rep;
        }
    }
    
    /**
     * @brief 减小容量以适应当前大小
     */
    void shrink_to_fit() {
        if (rep_->size < rep_->capacity) {
            // 创建合适大小的新内存
            Rep* new_rep = Rep::create(alloc_, rep_->size);
            
            // 复制数据
            traits_type::copy(new_rep->data(), rep_->data(), rep_->size);
            new_rep->size = rep_->size;
            new_rep->data()[new_rep->size] = CharT();
            
            // 释放旧内存
            Rep::destroy(alloc_, rep_);
            rep_ = new_rep;
        }
    }
    
    /**
     * @brief 检查字符串是否为空
     */
    bool empty() const noexcept {
        return rep_->size == 0;
    }
    
    /**
     * @brief 清空字符串
     */
    void clear() noexcept {
        rep_->size = 0;
        rep_->data()[0] = CharT();
    }
    
    // 元素访问
    
    /**
     * @brief 访问指定位置的字符（无边界检查）
     * 
     * @param pos 字符位置
     * @return reference 字符引用
     */
    reference operator[](size_type pos) {
        return rep_->data()[pos];
    }
    
    /**
     * @brief 访问指定位置的字符（无边界检查，常量版本）
     * 
     * @param pos 字符位置
     * @return const_reference 字符常量引用
     */
    const_reference operator[](size_type pos) const {
        return rep_->data()[pos];
    }
    
    /**
     * @brief 访问指定位置的字符（有边界检查）
     * 
     * @param pos 字符位置
     * @return reference 字符引用
     * @throw std::out_of_range 如果pos >= size()
     */
    reference at(size_type pos) {
        if (pos >= rep_->size) {
            throw std::out_of_range("basic_string::at: pos out of range");
        }
        return rep_->data()[pos];
    }
    
    /**
     * @brief 访问指定位置的字符（有边界检查，常量版本）
     * 
     * @param pos 字符位置
     * @return const_reference 字符常量引用
     * @throw std::out_of_range 如果pos >= size()
     */
    const_reference at(size_type pos) const {
        if (pos >= rep_->size) {
            throw std::out_of_range("basic_string::at: pos out of range");
        }
        return rep_->data()[pos];
    }
    
    /**
     * @brief 访问第一个字符
     * 
     * @return reference 第一个字符的引用
     */
    reference front() {
        return rep_->data()[0];
    }
    
    /**
     * @brief 访问第一个字符（常量版本）
     * 
     * @return const_reference 第一个字符的常量引用
     */
    const_reference front() const {
        return rep_->data()[0];
    }
    
    /**
     * @brief 访问最后一个字符
     * 
     * @return reference 最后一个字符的引用
     */
    reference back() {
        return rep_->data()[rep_->size - 1];
    }
    
    /**
     * @brief 访问最后一个字符（常量版本）
     * 
     * @return const_reference 最后一个字符的常量引用
     */
    const_reference back() const {
        return rep_->data()[rep_->size - 1];
    }
    
    /**
     * @brief 获取C风格字符数组
     * 
     * @return const_pointer C风格字符数组指针
     */
    const_pointer c_str() const noexcept {
        return rep_->data();
    }
    
    /**
     * @brief 获取字符数组
     * 
     * @return const_pointer 字符数组指针
     */
    const_pointer data() const noexcept {
        return rep_->data();
    }
    
    /**
     * @brief 获取字符数组（C++17，非常量版本）
     * 
     * @return pointer 字符数组指针
     */
    pointer data() noexcept {
        return rep_->data();
    }
};

// 字符串类型别名
using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

} // namespace mystl

#endif // MYSTL_STRING_H_ 