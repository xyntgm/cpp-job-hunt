# flightlog-analyzer

飞行参数 CSV 分析工具（C++17）。读取飞行数据记录，自动切分飞行阶段、统计各阶段高度/空速、
检测超限事件，并输出 Markdown 报告。

## 编译

### 方式 A：CMake + Ninja（推荐）

```powershell
cd D:\code\cpp-job-hunt\project1-flightlog-analyzer
$env:Path = "E:\Qt\Tools\Ninja;" + $env:Path
E:\Qt\Tools\CMake_64\bin\cmake.exe -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
E:\Qt\Tools\CMake_64\bin\cmake.exe --build build
```

### 方式 B：一条 g++ 命令（最省事）

```powershell
cd D:\code\cpp-job-hunt\project1-flightlog-analyzer
g++ -std=c++17 -Wall -Wextra -Iinclude src/csv.cpp src/analysis.cpp src/report.cpp src/main.cpp -o flightlog.exe
```

## 运行

```powershell
.\build\flightlog.exe data\sample.csv --out report.md
.\build\flightlog_tests.exe          # 单元测试（需从项目根目录运行）
```

## 输入格式

```csv
time_s,alt_ft,ias_kt,pitch_deg,roll_deg,n1_pct
0,500,0,0.0,0.0,55
```

- 表头必须完全一致
- 空行与 `#` 开头的行会被忽略
- 解析失败会抛出带行号的异常

## 分析规则

| 项目 | 规则 |
|---|---|
| 地面阶段 | 高度 < 1000 ft |
| 爬升 / 下降 | 垂直速度 > +300 / < -300 ft/min |
| 巡航 | 其余情况 |
| 滚转超限 | \|roll\| > 30° |
| 俯仰超限 | \|pitch\| > 20° |
| 空速超限 | IAS > 250 kt |
| 高度过低 | ALT < 500 ft |

## 目录结构

```
├── include/flightlog/   record.h  csv.h  analysis.h  report.h
├── src/                 csv.cpp  analysis.cpp  report.cpp  main.cpp
├── tests/test_main.cpp  极简断言测试（W2 换成 GTest）
├── data/sample.csv      模拟航迹
└── CMakeLists.txt
```

## 我踩的 3 个坑

1. **Windows 换行符**：`std::getline` 读到的行尾带 `\r`，导致表头比较失败。→ 统一 `trim()`。
2. **`std::stod` 静默截断**：输入 `12abc` 会返回 12 而不报错。→ 检查 `consumed` 是否等于字符串长度。
3. **`std::minmax_element` 返回的是迭代器对**（不是值对），空区间解引用会崩。→ 先判断 `count == 0`。

## 待办（W3 目标）

- [ ] 接入 GoogleTest，替换 `tests/test_main.cpp`
- [ ] 阶段切分加入滞回，避免临界点反复抖动
- [ ] 增加终端 ASCII 趋势图
- [ ] 阈值改为从 JSON 配置读取
- [ ] 接入 `clang-format`
