# StdSTL

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)

使用 **C++11** 仿写 STL 常见容器，帮助初学者深入理解标准库底层机制，快速提升 C++ 能力，是学习 STL 源码和底层数据结构的绝佳项目。

---

## 项目简介

本项目旨在通过 C++11 代码还原 STL 常用容器和相关底层数据结构，带有详细注释和结构化代码，适合：

- C++ 初学者进阶
- 算法/数据结构学习者
- 对 STL 实现机制感兴趣的开发者

---

## 使用环境

- **操作系统**：支持 Windows、Linux、macOS
- **编译器**：支持 C++11 标准的主流编译器（如 GCC 4.8 及以上、Clang 3.3 及以上、MSVC 2015 及以上）
- **依赖库**：无额外第三方依赖，仅依赖标准 C++11 库

---

## 适合人群

- 希望深入理解 STL 底层原理的 C++ 学习者
- 需要学习和借鉴数据结构实现的大学生、研究生
- 致力于提高代码能力、准备面试的开发者
- 对底层算法与容器实现细节有兴趣的 C++ 工程师

---

## 目录结构
| 目录/文件              | 说明                                        |
|------------------------|---------------------------------------------|
| my_deque/              | 双端队列（deque）实现                       |
| my_hashtable/          | 哈希表（hashtable）实现，unordered 容器基础 |
| my_list/               | 链表（list）实现，基础节点与迭代器          |
| my_map/                | 映射（map）实现，底层基于红黑树             |
| my_queue/              | 队列（queue）实现，适配器模式               |
| my_rb_tree/            | 红黑树（rb_tree）实现，map/set 底层         |
| my_set/                | 集合（set）实现，底层同 map                 |
| my_smart_pointer/      | 智能指针（unique_ptr、shared_ptr等）实现    |
| my_stack/              | 栈（stack）实现，适配器模式                 |
| my_string/             | 字符串（string）实现                        |
| my_unordered_map/      | 无序映射（unordered_map）实现               |
| my_unordered_set/      | 无序集合（unordered_set）实现               |
| my_vector/             | 动态数组（vector）实现                      |
| README.md              | 项目说明文档                                |

### 各模块简介
- **my_vector**：模仿 `std::vector`，包含动态扩容、下标访问、迭代器等功能。
- **my_list**：双向链表，支持节点插入、删除、迭代遍历。
- **my_deque**：分段数组实现，支持两端插入删除。
- **my_stack/my_queue**：容器适配器，底层基于 `vector` 或 `list`。
- **my_map/my_set**：基于红黑树，支持有序查找、插入和删除。
- **my_rb_tree**：红黑树独立实现，可学习平衡树原理。
- **my_hashtable/my_unordered_map/my_unordered_set**：哈希表底层实现，支持高效查找与插入。
- **my_string**：基本字符串功能实现，含深拷贝、移动语义等特性。
- **my_smart_pointer**：模拟 `unique_ptr`、`shared_ptr` 等智能指针，掌握资源管理原理。

**说明**：各模块通常包含源码（`.h`）和单元测试或使用示例（`testxxx.cpp`）。

### 使用方法
1. **克隆项目**
```bash
git clone https://github.com/Gitxiaozhu-oss/StdSTL.git
cd StdSTL/容器文件夹
make
```

2. **编译与运行示例（以 my_vector 为例）**
```bash
cd StdSTL/my_vector
// 编译
make

// 运行测试文件
./vector_test
```

3. **阅读源码与注释**：每个目录下的 `.h/.cpp` 文件均附有详细注释，便于理解实现原理,还有流程图。

### 学习建议
1. 建议按照容器复杂度由简到难学习：`vector` → `list` → `deque` → `stack/queue` → `map/set` → `unordered_map/unordered_set`。
2. 多对比 STL 官方实现和本项目代码，理解设计思路与优化方式。
3. 如遇疑惑，可阅读注释或提交 Issue 交流讨论。

### 贡献指南
- 欢迎贡献代码、完善文档、提出建议和反馈 Bug！
- 提交 Pull Request 需遵守统一代码风格。
- 建议每次修改配套增加测试用例。
- 文档和注释同样欢迎完善/翻译。

### 常见问题
**Q：如何选择模块学习？**  
A：推荐从 `my_vector` 等简单序列容器学起，逐步深入到 `map/set`、哈希表和红黑树。

**Q：代码能否直接用于生产？**  
A：本项目主要用于学习交流，未进行完整测试与优化，生产环境建议使用 STL 官方实现。

**Q：项目适用 C++ 标准？**  
A：全部使用 C++11 标准，便于理解现代 C++ 语法特性。

### License
本项目基于 **MIT 协议**开源，欢迎自由学习、修改和分发。

**查看更多内容与源码**：[StdSTL 仓库主页](https://github.com/Gitxiaozhu-oss/StdSTL.git)

如对实现细节、数据结构或 C++ 技巧有疑问，欢迎提 Issue 讨论！
