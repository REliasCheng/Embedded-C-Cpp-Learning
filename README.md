# Embedded-C-Cpp-Learning

嵌入式 C/C++ 程序设计学习与工程实践记录。仓库沿着课程的 day01–day13 展开，但导航按能力组织：从 C 的程序入口、数据表示和控制流，走到指针、结构体、资源管理与多文件设计，再进入 C++ 容器、引用和类。每个阶段挑选能说明技术问题的源码，配上我对调用关系、数据边界和工程用途的分析。

这里保留课程实践的来源，不把课堂代码写成独立原创。当前代码均为主机端练习；它们为我的 [STC89C52RC 外设实践](https://github.com/REliasCheng/stc89c52-learning)提供软件基础，但本仓库不冒充硬件驱动项目。

## 技术地图

`C 基础语义 → 指针与数据组织 → 函数接口与模块化 → C++ 对象与容器 → 嵌入式软件结构`

| 能力领域 | 已有代码或分析 | 入口 |
| --- | --- | --- |
| C 语言基础 | 程序入口、定宽整数、位运算和分支 | [day01–03](projects/01_C语言基础能力/day01_程序入口与输出/README.md) |
| C 语言核心 | 指针、数组、字符串、结构体/枚举、文件读写 | [day05–09](projects/02_C语言核心能力/day05_指针与回调/README.md) |
| 程序设计 | 函数签名、头源分离、回调接口 | [day04 与 V1 多文件案例](projects/03_程序设计能力/day04_函数与多文件/README.md) |
| C++ 程序设计 | 标准流、`vector`、引用、类与构造 | [day10–13](projects/04_C++程序设计/day10_基础语法与流/README.md) |
| 嵌入式软件思想 | 从现有代码分析数据宽度、接口、资源和层次边界 | [设计关联](projects/05_嵌入式软件思想/README.md) |

## 课程进阶与代码入口

每天都对应一个技术主题和至少一份精选课程源码；完整讲义、安装程序、视频和重复练习没有批量搬入。

| 阶段 | 技术主题 | 入口 |
| --- | --- | --- |
| day01 | 程序入口与标准输出 | [查看](projects/01_C语言基础能力/day01_程序入口与输出/README.md) |
| day02 | 定宽整数与数据表示 | [查看](projects/01_C语言基础能力/day02_数据类型与可移植性/README.md) |
| day03 | 位运算与分支选择 | [查看](projects/01_C语言基础能力/day03_位运算与分支/README.md) |
| day04 | 函数接口与多文件组织 | [查看](projects/03_程序设计能力/day04_函数与多文件/README.md) |
| day05 | 地址传递与函数回调 | [查看](projects/02_C语言核心能力/day05_指针与回调/README.md) |
| day06 | 数组参数与边界 | [查看](projects/02_C语言核心能力/day06_数组与参数/README.md) |
| day07 | 字符数组与字符指针 | [查看](projects/02_C语言核心能力/day07_字符串存储/README.md) |
| day08 | 结构体传递与枚举 | [查看](projects/02_C语言核心能力/day08_结构体与枚举/README.md) |
| day09 | 资源生命周期与文件 I/O | [查看](projects/02_C语言核心能力/day09_动态内存与文件/README.md) |
| day10 | C++ 标准输出与作用域 | [查看](projects/04_C++程序设计/day10_基础语法与流/README.md) |
| day11 | `vector` 容器操作 | [查看](projects/04_C++程序设计/day11_容器操作/README.md) |
| day12 | 引用与对象别名 | [查看](projects/04_C++程序设计/day12_引用与别名/README.md) |
| day13 | 类对象与构造函数 | [查看](projects/04_C++程序设计/day13_类与构造/README.md) |

## V1 专题案例（保留）

| 案例 | 当前可看的内容 | 入口 |
| --- | --- | --- |
| C 多文件设计 | `main.c` 调用 `calc.h` 声明的接口，`calc.c` 提供实现；同时保留 `A.h`、`B.h` 展示头文件依赖 | [c-module-design](projects/c-module-design/README.md) |
| C 指针与内存 | 对比值传递与地址传递的交换函数；阅读一次 `malloc`、判空、`free` 的完整路径 | [c-pointer-memory](projects/c-pointer-memory/README.md) |
| C++ 类与对象 | `stu.h` 声明类，`stu.cpp` 实现成员函数，`main.cpp` 创建对象并调用 | [cpp-class-object](projects/cpp-class-object/README.md) |

这三个 V1 案例没有被删除或改写。它们把多文件、地址传递和类的头源分离单独展开；新的 day 目录通过链接复用相关分析，不复制同一批源码。

## 如何查看与构建

每个 day 的 README 写明源文件位置、调用关系和本次验证结果。各练习多为独立的 `main`，不能把整个 `projects/` 一次性链接。C++ 课堂文件使用 GBK 编码；本机用 GCC/G++ 16.1.0、`-finput-charset=GBK` 构建 C++ 案例。编译产物放进被忽略的 `.build/`，不要直接提交。

新增的 15 个单文件课程示例分别编译、运行通过；day03 的交互例子输入 `A`，day09 文件读写仅在临时构建目录运行。这个验证范围不等同于穷尽边界测试，也不是硬件验证。具体警告和观察结果在对应 README 中。

## 仓库结构

```text
projects/
  01_C语言基础能力/     day01–03
  02_C语言核心能力/     day05–09
  03_程序设计能力/      day04
  04_C++程序设计/       day10–13
  05_嵌入式软件思想/    跨主题的设计关联
  c-module-design/      V1 多文件专题
  c-pointer-memory/     V1 指针与堆内存专题
  cpp-class-object/     V1 类头源分离专题
docs/
  学习与工程路线.md    课程阶段到工程能力的对应
```

各项目的 `course/` 是精选课程源码，保留原文件字节；项目 README 与 `docs/` 是基于代码的分析。来源与使用边界见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)，前期资产审阅见历史快照 [PROJECT_REVIEW.md](PROJECT_REVIEW.md)。

## 下一步

在现有课程原版旁增加个人改进版本，优先处理数组长度、文件错误路径和头文件中的全局定义；再通过实际代码和测试形成状态机、驱动接口等嵌入式软件案例。尚未完成的主题不列为现有成果。
