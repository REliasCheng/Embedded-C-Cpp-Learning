# 嵌入式命令处理框架

实现一个面向字节流的轻量命令处理框架。输入先进入固定容量环形缓冲区，主循环逐字节组装一行命令，再通过命令表和回调函数分发到设备状态。示例支持 `led on/off`、`mode 0..3` 和 `status`。

## 数据流

```text
字节输入
  ↓
RingBuffer                 中断与主循环之间的固定容量队列
  ↓
CommandEngine              按换行符组帧、处理超长输入
  ↓
CommandEntry[]             命令名与处理函数的映射表
  ↓
DeviceState                LED、模式和命令计数
```

- `ring_buffer.c/.h`：保存字节并显式报告满队列。
- `command.c/.h`：组帧、拆分参数、查表和回调。
- `device.c/.h`：集中保存被命令修改的状态。
- `main.c`：模拟UART输入并打印响应。
- `tests/test_command_framework.c`：覆盖正常命令、参数错误、未知命令和超长帧恢复。

## 实现

`command_engine_feed()` 只负责接收字节；`command_engine_process()` 只在主循环上下文中解析完整命令。二者通过环形缓冲区解耦，UART ISR 可调用前者，字符串比较和格式化保留在主循环。

命令表将字符串与函数指针绑定。新增命令时增加一项映射并实现对应处理函数，不需要扩展一长串嵌套判断。所有处理函数统一返回`CommandResult`，上层可以区分空命令、未知命令、参数错误、输入溢出和响应缓冲区不足。

## 构建与测试

```powershell
gcc -std=c11 -Wall -Wextra -Werror -pedantic `
  practice/src/ring_buffer.c practice/src/device.c practice/src/command.c `
  tests/test_command_framework.c -I practice/include -o command-test.exe
./command-test.exe
```

Tested with GCC 16.1.0 on Windows. 测试覆盖软件边界和状态变化；UART 寄存器与 ISR 不在这个主机端程序中。

## 接入 MCU

接入 MCU 时可由 UART ISR 调用 `command_engine_feed()`，主循环继续执行 `command_engine_process()`。这样字符串比较、格式化和 callback 不进入中断上下文。
