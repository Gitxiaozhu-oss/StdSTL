#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <memory>  // 用于标准库智能指针
#include "my_smart_pointer.h"  // 我们自己的智能指针实现

/**
 * @brief 用于测试的简单类
 */
class TestClass {
private:
    int value_;
    static int instance_count_;  // 用于跟踪实例数量

public:
    /**
     * @brief 构造函数
     * 
     * @param value 初始值
     */
    TestClass(int value = 0) : value_(value) {
        ++instance_count_;
        std::cout << "TestClass构造: " << value_ << ", 实例数: " << instance_count_ << std::endl;
    }

    /**
     * @brief 析构函数
     */
    ~TestClass() {
        --instance_count_;
        std::cout << "TestClass析构: " << value_ << ", 实例数: " << instance_count_ << std::endl;
    }

    /**
     * @brief 获取值
     * 
     * @return int 当前值
     */
    int getValue() const {
        return value_;
    }

    /**
     * @brief 设置值
     * 
     * @param value 新值
     */
    void setValue(int value) {
        value_ = value;
    }

    /**
     * @brief 获取实例数量
     * 
     * @return int 当前实例数量
     */
    static int getInstanceCount() {
        return instance_count_;
    }
};

// 静态成员初始化
int TestClass::instance_count_ = 0;

/**
 * @brief 自定义删除器
 */
struct CustomDeleter {
    using pointer = TestClass*;  // 为删除器定义指针类型

    void operator()(TestClass* ptr) const {
        std::cout << "自定义删除器被调用" << std::endl;
        delete ptr;
    }
};

/**
 * @brief 测试智能指针基本功能
 */
void test_smart_pointer_basics() {
    std::cout << "\n===== 测试智能指针基本功能 =====" << std::endl;
    
    {
        // 测试 unique_ptr
        std::cout << "\n--- 测试 unique_ptr ---" << std::endl;
        
        std::unique_ptr<TestClass> ptr1(new TestClass(1));
        assert(ptr1->getValue() == 1);
        assert(TestClass::getInstanceCount() == 1);
        
        // 移动构造
        std::unique_ptr<TestClass> ptr2 = std::move(ptr1);
        assert(!ptr1);
        assert(ptr2->getValue() == 1);
        
        // 重置
        ptr2.reset(new TestClass(2));
        assert(ptr2->getValue() == 2);
        assert(TestClass::getInstanceCount() == 1);
        
        // 自定义删除器
        std::unique_ptr<TestClass, CustomDeleter> ptr3(new TestClass(3), CustomDeleter());
        assert(ptr3->getValue() == 3);
        assert(TestClass::getInstanceCount() == 2);
        
        // 访问操作
        ptr3->setValue(30);
        assert((*ptr3).getValue() == 30);
    }
    
    // 测试引用计数和所有权转移
    {
        std::cout << "\n--- 测试 shared_ptr ---" << std::endl;
        
        std::shared_ptr<TestClass> shared1(new TestClass(4));
        assert(shared1.use_count() == 1);
        
        {
            std::shared_ptr<TestClass> shared2 = shared1;
            assert(shared1.use_count() == 2);
            assert(shared2.use_count() == 2);
            
            std::shared_ptr<TestClass> shared3 = std::move(shared1);
            assert(!shared1);
            assert(shared3.use_count() == 2);
        }
        
        // shared2和shared3已销毁，对象应该被释放
        assert(TestClass::getInstanceCount() == 0);
    }
    
    // 测试weak_ptr
    {
        std::cout << "\n--- 测试 weak_ptr ---" << std::endl;
        
        std::weak_ptr<TestClass> weak;
        
        {
            auto shared = std::make_shared<TestClass>(5);
            weak = shared;
            assert(!weak.expired());
            
            std::shared_ptr<TestClass> locked = weak.lock();
            assert(locked);
            assert(shared.use_count() == 2);
        }
        
        // shared销毁后，weak应该expired
        assert(weak.expired());
        assert(weak.lock() == nullptr);
    }
    
    assert(TestClass::getInstanceCount() == 0);
}

/**
 * @brief 测试循环引用问题
 */
void test_circular_reference() {
    std::cout << "\n===== 测试循环引用问题 =====" << std::endl;
    
    struct Node {
        std::shared_ptr<Node> next;
        std::weak_ptr<Node> weak_next;
        int id;
        
        Node(int i) : id(i) {
            std::cout << "创建节点: " << id << std::endl;
        }
        
        ~Node() {
            std::cout << "销毁节点: " << id << std::endl;
        }
    };
    
    // 使用shared_ptr造成循环引用
    std::cout << "\n使用shared_ptr形成循环引用:" << std::endl;
    {
        auto node1 = std::make_shared<Node>(1);
        auto node2 = std::make_shared<Node>(2);
        
        node1->next = node2;
        node2->next = node1;
        
        std::cout << "node1引用计数: " << node1.use_count() << std::endl;
        std::cout << "node2引用计数: " << node2.use_count() << std::endl;
    }
    std::cout << "循环引用场景结束（注意：存在内存泄漏）" << std::endl;
    
    // 使用weak_ptr避免循环引用
    std::cout << "\n使用weak_ptr避免循环引用:" << std::endl;
    {
        auto node3 = std::make_shared<Node>(3);
        auto node4 = std::make_shared<Node>(4);
        
        node3->weak_next = node4;
        node4->weak_next = node3;
        
        std::cout << "node3引用计数: " << node3.use_count() << std::endl;
        std::cout << "node4引用计数: " << node4.use_count() << std::endl;
        
        // 验证weak_ptr功能
        auto locked = node3->weak_next.lock();
        if (locked) {
            std::cout << "通过weak_ptr获取到节点 " << locked->id << std::endl;
        }
    }
    std::cout << "weak_ptr场景结束（节点已正确释放）" << std::endl;
}

/**
 * @brief 测试我们自己实现的智能指针
 */
void test_mystl_smart_pointers() {
    std::cout << "\n===== 测试 mystl 命名空间智能指针 =====" << std::endl;
    
    // 测试 unique_ptr
    {
        std::cout << "\n--- 测试 mystl::unique_ptr 基础功能 ---" << std::endl;
        
        // 基础功能测试
        mystl::unique_ptr<TestClass> ptr1(new TestClass(100));
        std::cout << "创建 mystl::unique_ptr 成功！" << std::endl;
        std::cout << "值: " << ptr1->getValue() << std::endl;
        
        // 测试移动所有权
        mystl::unique_ptr<TestClass> ptr2 = std::move(ptr1);
        std::cout << "移动后 ptr1 是否为空: " << (ptr1 ? "否" : "是") << std::endl;
        std::cout << "移动后 ptr2 的值: " << ptr2->getValue() << std::endl;
        
        // 测试重置功能
        ptr2.reset(new TestClass(101));
        std::cout << "重置后的值: " << ptr2->getValue() << std::endl;
        
        // 测试空指针重置
        ptr2.reset();
        std::cout << "重置为空后，ptr2 是否为空: " << (ptr2 ? "否" : "是") << std::endl;
        
        // 测试自定义删除器
        mystl::unique_ptr<TestClass, CustomDeleter> ptr3(new TestClass(102), CustomDeleter());
        std::cout << "带删除器的 unique_ptr 值: " << ptr3->getValue() << std::endl;
    }
    
    // 测试 unique_ptr 数组特化
    {
        std::cout << "\n--- 测试 mystl::unique_ptr 数组特化 ---" << std::endl;
        
        // 创建管理int数组的unique_ptr
        mystl::unique_ptr<int[]> arr(new int[5]);
        
        // 给数组元素赋值
        for (int i = 0; i < 5; ++i) {
            arr[i] = i * 10;
        }
        
        // 验证数组元素值
        for (int i = 0; i < 5; ++i) {
            std::cout << "arr[" << i << "] = " << arr[i] << std::endl;
        }
        
        // 测试重置
        arr.reset(new int[3]);
        
        // 给新数组元素赋值
        for (int i = 0; i < 3; ++i) {
            arr[i] = i * 100;
        }
        
        // 验证新数组元素值
        for (int i = 0; i < 3; ++i) {
            std::cout << "新 arr[" << i << "] = " << arr[i] << std::endl;
        }
        
        // 测试释放
        int* raw_arr = arr.release();
        std::cout << "释放后 arr 是否为空: " << (arr.get() == nullptr ? "是" : "否") << std::endl;
        
        // 手动删除释放的数组
        delete[] raw_arr;
        std::cout << "数组特化测试完成" << std::endl;
    }
    
    // 测试 shared_ptr
    {
        std::cout << "\n--- 测试 mystl::shared_ptr 基础功能 ---" << std::endl;
        
        // 创建和引用计数测试
        mystl::shared_ptr<TestClass> ptr1(new TestClass(200));
        std::cout << "创建 mystl::shared_ptr 成功！" << std::endl;
        std::cout << "初始引用计数: " << ptr1.use_count() << std::endl;
        
        // 复制和引用计数测试
        {
            mystl::shared_ptr<TestClass> ptr2 = ptr1;
            std::cout << "复制后引用计数: " << ptr1.use_count() << std::endl;
            
            // 移动构造测试
            mystl::shared_ptr<TestClass> ptr3 = std::move(ptr1);
            std::cout << "移动后 ptr1 是否为空: " << (ptr1 ? "否" : "是") << std::endl;
            std::cout << "ptr3 引用计数: " << ptr3.use_count() << std::endl;
            
            // reset测试
            ptr3.reset();
            std::cout << "重置后 ptr3 是否为空: " << (ptr3 ? "否" : "是") << std::endl;
            std::cout << "ptr2 引用计数: " << ptr2.use_count() << std::endl;
            
            // make_shared 测试
            auto ptr4 = mystl::make_shared<TestClass>(201);
            std::cout << "使用 make_shared 创建，值: " << ptr4->getValue() << std::endl;
            std::cout << "ptr4 引用计数: " << ptr4.use_count() << std::endl;
        }
        
        std::cout << "作用域结束后，TestClass 实例数: " << TestClass::getInstanceCount() << std::endl;
    }
    
    // 测试 weak_ptr
    {
        std::cout << "\n--- 测试 mystl::weak_ptr 基础功能 ---" << std::endl;
        
        mystl::weak_ptr<TestClass> weak;
        
        {
            // 创建 shared_ptr 并从中创建 weak_ptr
            auto shared = mystl::make_shared<TestClass>(300);
            weak = shared;
            std::cout << "创建 mystl::weak_ptr 成功！" << std::endl;
            std::cout << "weak_ptr 是否已过期: " << (weak.expired() ? "是" : "否") << std::endl;
            std::cout << "weak_ptr 引用计数: " << weak.use_count() << std::endl;
            
            // 从 weak_ptr 获取 shared_ptr
            mystl::shared_ptr<TestClass> locked = weak.lock();
            if (locked) {
                std::cout << "从 weak_ptr 获取的 shared_ptr 值: " << locked->getValue() << std::endl;
                std::cout << "获取后 shared_ptr 引用计数: " << shared.use_count() << std::endl;
            }
        }
        
        // shared_ptr 已销毁，测试 weak_ptr 状态
        std::cout << "shared_ptr 离开作用域后，weak_ptr 是否已过期: " << (weak.expired() ? "是" : "否") << std::endl;
        mystl::shared_ptr<TestClass> locked = weak.lock();
        std::cout << "从过期的 weak_ptr 获取 shared_ptr " << (locked ? "成功" : "失败") << std::endl;
    }
    
    // 测试循环引用问题
    {
        std::cout << "\n--- 测试 mystl 智能指针的循环引用问题 ---" << std::endl;
        
        struct Node {
            mystl::shared_ptr<Node> next;
            mystl::weak_ptr<Node> weak_next;
            int id;
            
            Node(int i) : id(i) {
                std::cout << "创建 mystl 节点: " << id << std::endl;
            }
            
            ~Node() {
                std::cout << "销毁 mystl 节点: " << id << std::endl;
            }
        };
        
        // 测试 shared_ptr 循环引用
        std::cout << "使用 mystl::shared_ptr 形成循环引用:" << std::endl;
        {
            auto node1 = mystl::make_shared<Node>(1);
            auto node2 = mystl::make_shared<Node>(2);
            
            node1->next = node2;
            node2->next = node1;
            
            std::cout << "node1 引用计数: " << node1.use_count() << std::endl;
            std::cout << "node2 引用计数: " << node2.use_count() << std::endl;
        }
        std::cout << "mystl::shared_ptr 循环引用作用域结束，应该有内存泄漏" << std::endl;
        
        // 测试 weak_ptr 避免循环引用
        std::cout << "\n使用 mystl::weak_ptr 避免循环引用:" << std::endl;
        {
            auto node3 = mystl::make_shared<Node>(3);
            auto node4 = mystl::make_shared<Node>(4);
            
            node3->weak_next = node4;
            node4->weak_next = node3;
            
            std::cout << "node3 引用计数: " << node3.use_count() << std::endl;
            std::cout << "node4 引用计数: " << node4.use_count() << std::endl;
            
            // 验证 weak_ptr 功能
            auto node4_lock = node3->weak_next.lock();
            if (node4_lock) {
                std::cout << "通过 weak_ptr 获取到节点 " << node4_lock->id << std::endl;
            }
        }
        std::cout << "mystl::weak_ptr 避免循环引用作用域结束，应该正确释放内存" << std::endl;
    }
    
    // 测试智能指针的比较运算
    {
        std::cout << "\n--- 测试 mystl 智能指针的比较运算 ---" << std::endl;
        
        // 注意：不要将同一个指针传递给多个shared_ptr
        mystl::shared_ptr<TestClass> sp1(new TestClass(400));
        mystl::shared_ptr<TestClass> sp2 = sp1;  // 共享同一个指针，而不是创建多个指针指向同一对象
        mystl::shared_ptr<TestClass> sp3(new TestClass(401));
        mystl::shared_ptr<TestClass> sp4;
        
        std::cout << "sp1 == sp2: " << (sp1 == sp2 ? "true" : "false") << std::endl;
        std::cout << "sp1 != sp3: " << (sp1 != sp3 ? "true" : "false") << std::endl;
        std::cout << "sp4 == nullptr: " << (sp4 == nullptr ? "true" : "false") << std::endl;
        std::cout << "nullptr == sp4: " << (nullptr == sp4 ? "true" : "false") << std::endl;
    }
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "开始测试智能指针基本功能..." << std::endl;
    
    test_smart_pointer_basics();
    test_circular_reference();
    
    // 测试我们自己的智能指针实现
    try {
        test_mystl_smart_pointers();
    } catch (const std::exception& e) {
        std::cout << "测试失败: " << e.what() << std::endl;
    }
    
    std::cout << "\n所有测试完成！" << std::endl;
    return 0;
} 