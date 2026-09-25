# 固件控制台集成

这个主机端示例把仓库已有的命令处理框架与轻量任务管理器接入同一个事件循环。输入仍按字节进入环形缓冲区；完整命令由命令表分发；周期 tick 驱动独立任务。

```text
stdin / UART-like bytes
        ↓
CommandEngine → DeviceState
                    ↑
system tick → TaskManager → heartbeat callback
```

## 代码关系

- `practice/src/firmware_console.c`：组合 `CommandEngine` 与 `TaskManager`，注册周期回调。
- `practice/src/main.c`：主机端交互入口，用标准输入模拟串口字节流。
- `tests/test_firmware_console.c`：验证命令更新状态、任务周期和回调对设备状态的影响。
- 命令解析与环形缓冲区来自相邻的[嵌入式命令处理框架](../嵌入式命令处理框架/README.md)。
- 周期任务来自相邻的[轻量任务状态管理](../轻量任务状态管理/README.md)。

`mode 1..3` 使 heartbeat 回调每两个 tick 翻转一次 LED 状态；`mode 0` 保留周期计数但停止翻转。这个行为用于验证命令路径和周期任务共享同一应用状态，不对应特定板卡引脚。

## 构建与测试

在本目录执行：

```powershell
$command = '..\嵌入式命令处理框架\practice'
$tasks = '..\轻量任务状态管理\practice'

gcc -std=c11 -Wall -Wextra -Werror -pedantic `
  practice/src/firmware_console.c practice/src/main.c `
  "$command/src/ring_buffer.c" "$command/src/device.c" "$command/src/command.c" `
  "$tasks/src/task_manager.c" `
  -I practice/include -I "$command/include" -I "$tasks/include" `
  -o firmware-console.exe

gcc -std=c11 -Wall -Wextra -Werror -pedantic `
  practice/src/firmware_console.c tests/test_firmware_console.c `
  "$command/src/ring_buffer.c" "$command/src/device.c" "$command/src/command.c" `
  "$tasks/src/task_manager.c" `
  -I practice/include -I "$command/include" -I "$tasks/include" `
  -o firmware-console-test.exe

.\firmware-console-test.exe
```

该项目位于 `practice/`，集成命令处理框架和轻量任务管理器。
