# Embedded-C-Cpp-Learning

嵌入式 C/C++ 程序设计学习与工程实践记录。这里不按课程日期堆放代码，而是从实际练习中挑选能说明问题的案例：C 的数据传递与内存、跨文件接口，以及 C++ 类与对象的组织方式。

当前是 V1。三个案例保留课程原始源码，并由我单独写明文件关系、关键代码和局限；课程代码不标作独立原创。仓库暂不包含单片机固件，也不把尚未写出的状态机、驱动抽象或多态项目列为已完成成果。

## 技术方向

`C 数据与指针 → 函数及模块接口 → C++ 类与对象 → 嵌入式软件结构`

这条路线连接我的 [STC89C52RC 外设实践](https://github.com/REliasCheng/stc89c52-learning)。结构体等数据组织主题在已阅读课程资料中存在，但 V1 尚未将其作为独立案例迁入；后续按实际完成内容更新。

## 当前项目内容

| 案例 | 当前可看的内容 | 入口 |
| --- | --- | --- |
| C 多文件设计 | `main.c` 调用 `calc.h` 声明的接口，`calc.c` 提供实现；同时保留 `A.h`、`B.h` 展示头文件依赖 | [c-module-design](projects/c-module-design/README.md) |
| C 指针与内存 | 对比值传递与地址传递的交换函数；阅读一次 `malloc`、判空、`free` 的完整路径 | [c-pointer-memory](projects/c-pointer-memory/README.md) |
| C++ 类与对象 | `stu.h` 声明类，`stu.cpp` 实现成员函数，`main.cpp` 创建对象并调用 | [cpp-class-object](projects/cpp-class-object/README.md) |

这些是课程学习实践及其代码分析，不是硬件实验，也不是已完成的通用程序库。项目文档明确指出原例中的边界，避免把教学代码直接当作可复用组件。

## 如何查看与构建

每个案例的 README 都给出源码入口、调用关系和单独构建命令。三组源码各有自己的 `main`，不能把整个 `projects/` 一次性链接成一个程序。需要 Windows 上的 GCC/G++，或其他支持对应 C/C++ 标准的编译器；C++ 原文件采用 GBK 编码，构建时须指定输入编码。

## 仓库结构

```text
projects/
  c-module-design/    C 的头文件声明与跨文件调用
  c-pointer-memory/   值传递、地址传递与堆内存
  cpp-class-object/   C++ 类声明、实现和对象调用
docs/
  学习与工程路线.md    三个案例如何连接后续嵌入式软件实践
```

各项目的 `course/` 是精选课程源码，保留原文件字节；项目 README 与 `docs/` 是基于代码的分析。来源与使用边界见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)，前期资产审阅见 [PROJECT_REVIEW.md](PROJECT_REVIEW.md)。

## 下一步

先为现有三个案例补独立验证记录，再选择结构体/文件 I/O 或回调示例做个人改进版本。后续只在有实际代码和验证结果时新增项目，不预先填充空目录。
