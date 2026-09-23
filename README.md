# Embedded-C-Cpp-Learning

C/C++ 语言练习与嵌入式软件结构实验。内容包括数据表示、指针与内存、多文件接口、回调、对象封装和状态机，并包含两个可在主机端独立构建的软件项目。

| 命令处理框架 | 轻量任务管理器 |
| --- | --- |
| ![命令处理框架架构](assets/images/command-framework-architecture.svg) | ![轻量任务管理架构](assets/images/task-manager-architecture.svg) |

## 项目

| 目录 | 内容 |
| --- | --- |
| [01 C 语言基础](projects/01_C语言基础能力/README.md) | 定宽整数、位运算、控制流、数组和字符串 |
| [02 C 语言核心](projects/02_C语言核心能力/README.md) | 指针、动态内存、结构体、枚举和文件 I/O |
| [03 程序设计](projects/03_程序设计能力/README.md) | 头源分离、模块内部状态、接口和函数指针回调 |
| [04 C++ 程序设计](projects/04_C++程序设计/README.md) | 引用、类、构造函数、封装和 vector |
| [05 嵌入式软件结构](projects/05_嵌入式软件思想/README.md) | 分层、接口边界和事件状态机 |
| [06 综合软件实践](projects/06_综合软件实践/README.md) | 命令处理框架与轻量任务管理器 |

### Command framework

RingBuffer 接收逐字节输入，CommandEngine 组装命令行并查询命令表，匹配后调用对应 callback 更新 DeviceState。测试覆盖正常命令、参数错误、未知命令和缓冲区溢出恢复。

    byte input → ring buffer → line parser → command table → callback → device state

[源码与测试](projects/06_综合软件实践/嵌入式命令处理框架/README.md)

### Task manager

固定容量任务表保存周期、下一次运行时间、状态和 callback。每次 tick 只运行到期且处于 RUNNING 的任务；故障任务可单独复位，不影响其他任务。

    tick → task table → state check → callback

[源码与测试](projects/06_综合软件实践/轻量任务状态管理/README.md)

## 构建与测试

个人实践使用 GCC/G++ 16.1.0 在 Windows 上构建，C 与 C++ 示例分别采用 C11 和 C++11，并启用：

    -Wall -Wextra -Werror -pedantic

课程 C++ 文件包含 GBK 编码源码，编译时需要 -finput-charset=GBK。完整命令见[编译环境说明](docs/编译环境说明.md)，测试方法见[调试方法](docs/调试方法.md)。

    projects/
    ├─ 01_C语言基础能力/
    ├─ 02_C语言核心能力/
    ├─ 03_程序设计能力/
    ├─ 04_C++程序设计/
    ├─ 05_嵌入式软件思想/
    └─ 06_综合软件实践/

各主题中的 course/ 保存课程源码，practice/ 保存个人实现，docs/ 记录接口和来源。详细来源见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。
