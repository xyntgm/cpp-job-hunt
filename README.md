# cpp-job-hunt

求职作品集仓库。三个项目对应 [[求职作战/04-三个项目规格书]]。

| 项目 | 说明 | 状态 |
|---|---|---|
| `project1-flightlog-analyzer` | 飞行数据 CSV 解析 / 阶段切分 / 超限检测 / Markdown 报告（纯 C++17 + CMake + 单测） | 🟡 骨架已跑通，待扩展 |
| `project2-devicehmi` | Qt6 工业设备采集上位机（Modbus RTU + 实时曲线 + SQLite）★ 主打 | ⬜ W4 开工 |
| `project3-collect-gateway` | Linux epoll 多线程采集网关 | ⬜ W7 开工 |

## 每天至少要有一个 commit

```
git add -A
git commit -m "feat: 实现 xxx"
git push
```

## 工具链

- 纯 C++（项目①③）：`g++ 12.2`（MinGW，已在 PATH）
- Qt 项目（项目②）：`E:\Qt\6.5.3\mingw_64` + `E:\Qt\Tools\mingw1310_64` + Qt Creator
- 构建：CMake（`E:\Qt\Tools\CMake_64`）+ Ninja（`E:\Qt\Tools\Ninja`）
