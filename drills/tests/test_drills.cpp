// ============================================================================
//  C++ 语法体检 · 测试就是需求文档
//
//  ⚠️ 不要修改这个文件。看它 → 猜实现 → 去 src/drills.cpp 写。
// ============================================================================

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "drills/drills.h"

namespace {

int g_pass = 0;
int g_fail = 0;

void check(bool condition, const std::string& what) {
    if (!condition) {
        throw std::runtime_error(what);
    }
}

void checkNear(double a, double b, const std::string& what) {
    if (std::fabs(a - b) > 1e-9) {
        throw std::runtime_error(what + "  （期望 " + std::to_string(b) + "，实际 " +
                                 std::to_string(a) + "）");
    }
}

template <typename Fn>
void run(const char* name, Fn fn) {
    try {
        fn();
        ++g_pass;
        std::cout << "  [PASS] " << name << "\n";
    } catch (const std::exception& e) {
        ++g_fail;
        std::cout << "  [FAIL] " << name << "\n         -> " << e.what() << "\n";
    }
}

// 造一个临时文件，返回路径
std::string makeTempFile(const std::string& content, const std::string& name) {
    const std::string path = name;
    std::ofstream out(path);
    out << content;
    return path;
}

}  // namespace

int main() {
    using namespace drills;

    std::cout << "\n========== C++ 语法体检 ==========\n";

    // ---------------------------------------------------------- D01
    run("D01 swapByPtr 交换两个整数", [] {
        int a = 1, b = 2;
        swapByPtr(&a, &b);
        check(a == 2 && b == 1, "交换结果不正确");
    });
    run("D01 swapByRef 交换两个整数", [] {
        int a = 3, b = 4;
        swapByRef(a, b);
        check(a == 4 && b == 3, "交换结果不正确");
    });
    run("D01 findMax 返回最大元素地址", [] {
        const int  arr[] = {3, 9, 2, 7};
        const int* p     = findMax(arr, arr + 4);
        check(p != nullptr, "不该返回 nullptr");
        check(*p == 9, "返回的不是最大值");
        check(p == arr + 1, "应返回最大元素的地址，而不是别的元素的地址");
    });
    run("D01 findMax 空区间返回 nullptr", [] {
        int        dummy = 1;
        const int* p     = findMax(&dummy, &dummy);
        check(p == nullptr, "空区间必须返回 nullptr");
    });

    // ---------------------------------------------------------- D02
    run("D02 hasPrefix 前缀判断", [] {
        check(hasPrefix("modbus-rtu", "modbus"), "应判定为真");
        check(!hasPrefix("modbus", "modbus-rtu"), "应判定为假");
        check(hasPrefix("abc", ""), "空前缀应判定为真");
        check(!hasPrefix("ab", "abc"), "前缀比原串长应为假");
    });
    run("D02 ConstBox 的 const 成员函数与 mutable", [] {
        ConstBox box(42);
        check(box.value() == 42, "初值不对");
        const ConstBox& ref = box;  // 关键：通过 const 引用访问
        check(ref.value() == 42, "const 对象上应能调用 const 成员函数");
        check(box.readCount() == 2, "readCount 应记录 value() 被调用的次数（mutable）");
        box.setValue(7);
        check(box.value() == 7, "setValue 后取值不对");
        check(box.readCount() == 3, "readCount 累计不对");
    });

    // ---------------------------------------------------------- D03
    run("D03 InstanceCounter 生命周期计数", [] {
        const int base = InstanceCounter::alive();
        {
            InstanceCounter a;
            const int        id_a = a.id();
            check(InstanceCounter::alive() == base + 1, "构造后存活数应为 base+1");
            {
                InstanceCounter b = a;  // 拷贝构造
                check(InstanceCounter::alive() == base + 2, "拷贝构造后应为 base+2");
                check(b.id() == id_a, "拷贝出来的对象应保留同一个 id");
            }
            check(InstanceCounter::alive() == base + 1, "b 析构后应回到 base+1");
        }
        check(InstanceCounter::alive() == base, "全部析构后应回到 base");
    });

    // ---------------------------------------------------------- D04
    run("D04 Buffer 深拷贝（互不影响）", [] {
        Buffer a(4);
        a.data()[0] = 9;
        Buffer b = a;                 // 拷贝构造
        check(b.size() == 4, "拷贝后 size 不对");
        check(b.data() != a.data(), "必须是深拷贝：两块内存地址不能相同");
        b.data()[0] = 1;
        check(a.data()[0] == 9, "改 b 不能影响 a（说明发生了浅拷贝）");
    });
    run("D04 Buffer 拷贝赋值与自赋值", [] {
        Buffer a(2);
        Buffer c(6);
        c = a;
        check(c.size() == 2, "拷贝赋值后 size 应为源对象的 size");
        check(c.data() != a.data(), "拷贝赋值也要深拷贝");
        Buffer& self = c;
        c = self;               // 自赋值不能崩、不能把自己释放掉
        check(c.size() == 2, "自赋值后对象仍应可用");
        check(c.data() != nullptr, "自赋值后 data 不能是野指针");
    });

    // ---------------------------------------------------------- D05
    run("D05 Buffer 移动语义", [] {
        Buffer a(8);
        a.data()[0]     = 5;
        unsigned char* raw = a.data();
        Buffer         b   = std::move(a);   // 移动构造
        check(b.size() == 8, "移动后目标 size 不对");
        check(b.data() == raw, "移动应转移指针所有权，而不是重新分配内存");
        check(a.size() == 0 && a.data() == nullptr, "被移动对象的 size/data 必须置空");
    });

    // ---------------------------------------------------------- D06
    run("D06 unique_ptr 独占所有权", [] {
        auto d = makeDevice(7);
        check(d != nullptr, "makeDevice 不该返回空指针");
        check(d->id() == 7, "id 不对");
    });
    run("D06 shared_ptr 引用计数", [] {
        auto p = std::make_shared<Device>(1);
        check(useCountOf(p) == 1, "此时引用计数应为 1");
        {
            auto q = p;
            check(useCountOf(p) == 2, "拷贝后应为 2");
        }
        check(useCountOf(p) == 1, "离开作用域后应回到 1");
    });

    // ---------------------------------------------------------- D07
    run("D07 countWords 词频统计", [] {
        const std::vector<std::string> words{"a", "b", "a", "c", "a", "b"};
        const auto                     m = countWords(words);
        check(m.size() == 3, "不同单词数应为 3");
        check(m.at("a") == 3 && m.at("b") == 2 && m.at("c") == 1, "词频统计不对");
    });
    run("D07 topK 取前 k 大（降序）", [] {
        const auto r = topK({5, 1, 9, 3, 7}, 2);
        check(r.size() == 2, "应返回 2 个元素");
        check(r[0] == 9 && r[1] == 7, "应为降序的前 2 大");
        const auto all = topK({2, 1}, 5);
        check(all.size() == 2, "k 超过元素个数时应返回全部");
    });

    // ---------------------------------------------------------- D08
    run("D08 mean 平均值（用 STL 算法实现）", [] {
        checkNear(mean({1.0, 2.0, 3.0, 4.0}), 2.5, "平均值不对");
        checkNear(mean({}), 0.0, "空序列应返回 0");
    });
    run("D08 countGreater 计数", [] {
        check(countGreater({1.0, 5.0, 3.0, 8.0}, 3.0) == 2, "统计结果不对");
    });
    run("D08 scaleAll 变换", [] {
        const auto r = scaleAll({1.0, 2.0}, 3.0);
        check(r.size() == 2 && r[0] == 3.0 && r[1] == 6.0, "变换结果不对");
    });

    // ---------------------------------------------------------- D09
    run("D09 makeAdder 闭包", [] {
        const auto add5 = makeAdder(5);
        check(add5(10) == 15, "闭包结果不对");
        const auto add0 = makeAdder(0);
        check(add0(3) == 3, "闭包结果不对");
    });

    // ---------------------------------------------------------- D10
    run("D10 maxOf 函数模板", [] {
        check(maxOf<int>({3, 9, 2}) == 9, "int 版本结果不对");
        checkNear(maxOf<double>({1.5, 0.5}), 1.5, "double 版本结果不对");
        check(maxOf<std::string>({std::string("b"), std::string("z")}) == "z",
              "string 版本结果不对");
    });
    run("D10 maxOf 空序列抛 invalid_argument", [] {
        bool thrown = false;
        try {
            (void)maxOf<int>({});
        } catch (const std::invalid_argument&) {
            thrown = true;
        }
        check(thrown, "空序列应抛 std::invalid_argument");
    });

    // ---------------------------------------------------------- D11
    run("D11 parseInt 正常与异常", [] {
        check(parseInt("123") == 123, "正常解析失败");
        check(parseInt("-45") == -45, "负数解析失败");
        check(parseInt("0") == 0, "0 解析失败");

        int caught = 0;
        for (const std::string& bad : {std::string(""), std::string("12a"), std::string("abc"),
                                      std::string("99999999999999999999")}) {
            try {
                (void)parseInt(bad);
            } catch (const std::invalid_argument&) {
                ++caught;
            }
        }
        check(caught == 4, "4 个非法输入都应抛 std::invalid_argument，实际抛出 " +
                               std::to_string(caught) + " 个");
    });

    // ---------------------------------------------------------- D12
    run("D12 readFirstNumber 读文件", [] {
        const char* path = "tmp_drills_numbers.txt";
        makeTempFile("# 注释行\nhello\n3.14\n2.71\n", path);
        const auto v = readFirstNumber(path);
        check(v.has_value(), "应该读到数字");
        checkNear(*v, 3.14, "读到的数字不对");
        std::remove(path);
    });
    run("D12 readFirstNumber 无数字 / 文件不存在", [] {
        const char* path = "tmp_drills_empty.txt";
        makeTempFile("no numbers here\n", path);
        check(!readFirstNumber(path).has_value(), "没有数字时应返回 nullopt");
        std::remove(path);
        check(!readFirstNumber("definitely_not_exist_12345.txt").has_value(),
              "文件不存在时应返回 nullopt 而不是抛异常");
    });

    // ---------------------------------------------------------- 汇总
    std::cout << "\n==================================\n";
    std::cout << "  通过 " << g_pass << " / " << (g_pass + g_fail) << "\n";
    if (g_fail > 0) {
        std::cout << "  还有 " << g_fail << " 项没通过 —— 每一项就是一个语法缺口。\n";
    } else {
        std::cout << "  全部通过。你的 C++ 语法基本盘可以了，回项目里去。\n";
    }
    std::cout << "==================================\n\n";
    return g_fail == 0 ? 0 : 1;
}
