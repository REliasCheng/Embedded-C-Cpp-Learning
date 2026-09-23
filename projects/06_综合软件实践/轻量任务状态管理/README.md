# 轻量任务状态管理

实现一个固定容量的周期任务管理器，用离散 `tick` 推进任务，并显式处理停止、运行和故障状态。程序采用单线程、静态任务表和回调，执行路径对应裸机主循环中的“时间到达—执行回调—更新状态”。

## 调度流程

```text
timer tick
   ↓
task_manager_tick()
   ↓
逐项检查 RUNNING 任务
   ↓
到期 → callback(context) → 重装 remaining
```

`TaskManager`内部保存固定长度任务表；每项包含周期、剩余tick、运行次数、状态、回调与上下文。故障事件只停止对应任务，不影响其他任务继续运行；复位后由调用方决定是否重新启动。

## 实现

- `task_manager_add()`拒绝零周期、空回调和超容量注册。
- `task_manager_start()`重新装载周期，避免沿用停止前的残余计数。
- `task_manager_tick()`只执行`RUNNING`任务，并在回调后累计运行次数。
- `task_manager_fault()`和`task_manager_reset()`构成明确的故障恢复路径。

## 构建与测试

```powershell
gcc -std=c11 -Wall -Wextra -Werror -pedantic `
  practice/src/task_manager.c tests/test_task_manager.c `
  -I practice/include -o task-manager-test.exe
./task-manager-test.exe
```

测试覆盖两个不同周期任务、停止后不执行、单任务故障隔离及复位重启。tick是主机端离散输入；接入MCU时应由定时器ISR只产生tick，回调耗时和共享数据仍需按目标平台评估。

## 接入 MCU

接入 MCU 时由定时器 ISR 产生 tick，主循环调用调度函数。回调仍运行在主循环上下文，任务耗时和共享数据需要按目标平台检查。
