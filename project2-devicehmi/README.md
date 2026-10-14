# project2-devicehmi

工业设备数据采集上位机（Qt6 / C++17）。**阶段 1 已完成并跑通。**

---

## 阶段 1（已完成 2026-10-14）：串口调试助手

一个能用的设备调试工具：串口配置、收发数据、HEX/文本双模式、时间戳、定时发送、回环自测。

```powershell
cd D:\code\cpp-job-hunt\project2-devicehmi
$env:Path = "E:\Qt\6.5.3\mingw_64\bin;E:\Qt\Tools\mingw1310_64\bin;E:\Qt\Tools\Ninja;" + $env:Path
E:\Qt\Tools\CMake_64\bin\cmake.exe -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_PREFIX_PATH="E:/Qt/6.5.3/mingw_64" `
    -DCMAKE_CXX_COMPILER="E:/Qt/Tools/mingw1310_64/bin/g++.exe"
E:\Qt\Tools\CMake_64\bin\cmake.exe --build build
```

运行：双击 `run.bat`（它会先把 Qt 的 bin 目录加进 PATH）。

### 没有硬件怎么玩

端口下拉框第一项是 **「回环（无硬件自测）」**。选它 → 打开 → 在发送区输入内容 → 点发送，
50ms 后你会在接收区看到自己发出去的数据。勾上 **HEX 显示** 和 **HEX 发送** 可以看到
`hello device` 变成 `68 65 6C 6C 6F 20 64 65 76 69 63 65`。

> 面试话术：「我没有真实硬件，所以实现了一个回环传输，可以在零硬件条件下跑通整条链路做自测。」

---

## 架构（这是本项目最值钱的部分）

```
MainWindow (UI)
    │  只认接口，不认串口/网络
    ▼
ITransport  ← 抽象接口：open/close/write + bytesReceived/errorOccurred/stateChanged
    ├── SerialTransport    QSerialPort 实现（真实设备）
    └── LoopbackTransport  回环实现（无硬件自测）
```

**为什么要这层抽象？** 因为「换一种通信方式不动 UI 一行代码」是设备软件的核心设计诉求 ——
同一套界面要支持 RS232 / RS485 / TCP 三种接入。面试官问"你的架构怎么分层"，答案就是这张图。

## 文件

| 文件 | 职责 |
|---|---|
| `src/ITransport.h` | 传输层抽象接口（**面试重点**） |
| `src/SerialTransport.h/.cpp` | 串口实现：配置、打开、readyRead、错误去重、掉线自动关闭 |
| `src/LoopbackTransport.h/.cpp` | 回环实现：模拟设备 50ms 响应延迟 |
| `src/HexUtil.h/.cpp` | 字节 ↔ HEX 字符串、可打印文本、时间戳 |
| `src/MainWindow.h/.cpp` | 界面：配置区 / 接收区 / 发送区 |

---

## 阶段 2 起的任务（W4-W6，你的活）

### 步骤 1：Modbus RTU 协议栈（W4）
- [ ] `Crc16` 查表法实现（多项式 0xA001），单元测试验证标准帧
- [ ] `ModbusRtuCodec`：帧构建/解析，支持功能码 03（读保持寄存器）/ 06（写单个）/ 10（写多个）
- [ ] 超时重传、异常响应码处理（0x80 + 错误码）
- [ ] **虚拟设备模拟器**：独立可执行程序，通过 com0com 虚拟串口对
      （或直接复用 LoopbackTransport 的升级版）持续产生模拟寄存器数据
- [ ] 用**本工具自己**去连模拟器调试 —— 这就是"我给自己写调试工具"的来源

### 步骤 2：多线程采集 + 实时曲线（W5）
- [ ] 采集放进独立线程（`QThread` + `moveToThread`），经队列与 UI 解耦
- [ ] QtCharts 实时曲线，采样与刷新分离（1kHz 采集 / 10Hz 刷新）
- [ ] 断线自动重连（指数退避）

### 步骤 3：能交付的产品形态（W6）
- [ ] SQLite 落库（批量事务）+ 历史数据查询
- [ ] 报警（阈值上下限）+ 报警列表
- [ ] 配置持久化（QSettings 或 JSON）
- [ ] `windeployqt` 打包成免安装 exe
- [ ] **2 分钟演示录屏**

---

## TODO-学（用标记-回填法：卡住 30 秒就跳过，记进缺口清单）

- `QSerialPort` 为什么作为值成员而不是裸指针？
- 为什么两个不同的 `QObject` 之间用信号槽可以跨线程，而直接调函数不行？
- `readyRead` 的触发时机是什么？为什么读到的可能是半个帧？
- `m_recvView->document()->setMaximumBlockCount(5000)` 解决的是什么问题？
- 为什么 `~SerialTransport()` 里要判断 `isOpen()` 再 `close()`？
- `deleteLater()` 和 `delete` 的区别是什么？（代码里用到了 `deleteLater`）
