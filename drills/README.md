# C++ 语法体检 · 12 个填空练习

> **目的不是"学 C++"，是让你能解释自己项目里的每一行。**
> 这 12 个练习覆盖了 `project1-flightlog-analyzer` 里用到的全部 C++ 特性。

---

## 怎么用（重要：顺序不能反）

```
1. 打开 tests/test_drills.cpp，读对应那一段测试 —— 那就是需求文档
2. 猜一下实现该怎么写（写不出来正常）
3. 去 src/drills.cpp 把 throw 换成你的实现
4. 编译运行，看还有几条 FAIL
5. FAIL 的那条 = 一个真实缺口 → 记进 Obsidian 的缺口清单
6. 30 秒内能查到答案就查；超过 30 秒就跳过，先做下一个
```

**唯一的规则：卡住超过 30 秒就跳过。** 全做完一遍再回来补，不要在第一题上耗两小时。

---

## 编译运行

```powershell
cd D:\code\cpp-job-hunt\drills
$env:Path = "E:\Qt\Tools\Ninja;" + $env:Path
E:\Qt\Tools\CMake_64\bin\cmake.exe -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
E:\Qt\Tools\CMake_64\bin\cmake.exe --build build
.\build\drills_check.exe
```

---

## 12 个练习覆盖什么

| 编号 | 主题 | 对应项目① 里的哪一行代码 |
|---|---|---|
| D01 | 指针、引用、指针运算 | `const int* findMax(...)`、`records[i].*field` |
| D02 | `const` 全貌、`mutable`、const 成员函数 | `statsOf(..., double FlightRecord::*field)` |
| D03 | 构造/析构/初始化列表/静态成员 | `FlightRecord` 的默认成员初值 |
| D04 | 三法则、深拷贝、自赋值 | （项目② 会大量用） |
| D05 | 移动语义、`std::move` | `collectField` 返回 `std::vector` |
| D06 | `unique_ptr` / `shared_ptr` / 引用计数 | （项目③ 会大量用） |
| D07 | 容器选型、`unordered_map`、`topK` | `countWords` ↔ `records` 聚合 |
| D08 | 迭代器、STL 算法、lambda | `std::minmax_element` / `std::accumulate` |
| D09 | lambda 捕获、返回闭包 | `count_if` 里的 lambda |
| D10 | 函数模板、`iterator_traits` | `computeStats<Iter>` |
| D11 | 异常、`std::invalid_argument` | `csv.cpp` 的 `toDouble` |
| D12 | `std::optional`、文件流、RAII | `readFirstNumber` ↔ `loadCsv` |

---

## 参考：D01 的做法（只给这一个，剩下的自己想）

```cpp
void swapByPtr(int* a, int* b) {
    if (a == nullptr || b == nullptr) {
        return;                 // 或抛异常，但要先想清楚契约
    }
    const int tmp = *a;
    *a = *b;
    *b = tmp;
}

const int* findMax(const int* begin, const int* end) {
    if (begin == nullptr || begin == end) {
        return nullptr;
    }
    const int* max_ptr = begin;
    for (const int* p = begin + 1; p != end; ++p) {   // 指针运算 + 迭代器式遍历
        if (*p > *max_ptr) {
            max_ptr = p;
        }
    }
    return max_ptr;             // 返回的是"地址"，不是值 —— 这就是本题的考点
}
```

注意 `findMax` 的测试里有一句 `check(p == arr + 1, ...)`：**必须返回最大元素的地址**。
返回 `&arr[0]` 或者返回一个新的临时变量地址都是错的 —— 这就是"指针 vs 值"的差别。

---

## 全部通过之后

不要继续刷题。**回到 `project1-flightlog-analyzer` 去加功能**（见它的 README 待办清单）。
语法只在"要用"的时候才学得牢。
