#pragma once

// ============================================================================
//  C++ 语法体检 · 12 个填空练习
//  规则：只需要改 src/drills.cpp（D05 改本文件的两个移动函数）。
//        测试文件 tests/test_drills.cpp 不要改 —— 它就是需求文档。
// ============================================================================

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace drills {

// ---------------------------------------------------------------- D01 指针与引用
// 用指针交换两个整数
void swapByPtr(int* a, int* b);

// 用引用交换两个整数
void swapByRef(int& a, int& b);

// 返回区间内最大元素的地址；区间为空时返回 nullptr
const int* findMax(const int* begin, const int* end);

// ---------------------------------------------------------------- D02 const 全貌
// 参数必须是 const 引用（避免拷贝，且承诺不修改）
bool hasPrefix(const std::string& text, const std::string& prefix);

class ConstBox {
public:
    explicit ConstBox(int value);

    // const 成员函数：允许在 const 对象上调用，内部不能改成员（除非 mutable）
    // 要求：每调用一次 value()，内部的读取计数 +1
    int value() const;

    void setValue(int value);

    int readCount() const;

private:
    int         value_;
    mutable int read_count_ = 0;
};

// ---------------------------------------------------------------- D03 类与生命周期
// 统计当前存活的对象个数
class InstanceCounter {
public:
    InstanceCounter();
    InstanceCounter(const InstanceCounter& other);
    InstanceCounter& operator=(const InstanceCounter& other);
    ~InstanceCounter();

    static int alive();   // 当前存活对象数

    int id() const;

private:
    int        id_;
    static int alive_;
    static int next_id_;
};

// ---------------------------------------------------------------- D04 三法则（深拷贝）
class Buffer {
public:
    explicit Buffer(std::size_t n);            // 分配 n 字节并全部置 0

    Buffer(const Buffer& other);               // 深拷贝（不是共享指针！）
    Buffer& operator=(const Buffer& other);

    ~Buffer();

    // TODO: D05 —— 实现下面两个移动函数（实现后请自行加上 noexcept）
    Buffer(Buffer&& other) noexcept : data_(nullptr), size_(0) { (void)other; }
    Buffer& operator=(Buffer&& other) noexcept { (void)other; return *this; }

    std::size_t          size() const;
    unsigned char*       data();
    const unsigned char* data() const;

private:
    unsigned char* data_;
    std::size_t    size_;
};

class Device {
public:
    explicit Device(int id);
    int id() const;
};

// ---------------------------------------------------------------- D06 智能指针
std::unique_ptr<Device>  makeDevice(int id);
std::size_t              useCountOf(const std::shared_ptr<Device>& p);

// ---------------------------------------------------------------- D07 容器选型
// 统计每个单词出现次数（不需要有序 → 想清楚该用哪个容器）
std::unordered_map<std::string, int> countWords(const std::vector<std::string>& words);

// 返回最大的 k 个值，按降序排列；k 大于元素个数时返回全部
std::vector<int> topK(std::vector<int> values, std::size_t k);

// ---------------------------------------------------------------- D08 迭代器与算法
// 以下三个函数禁止手写 for 循环，必须用 STL 算法 + lambda
double               mean(const std::vector<double>& values);
std::size_t          countGreater(const std::vector<double>& values, double threshold);
std::vector<double>  scaleAll(const std::vector<double>& values, double factor);

// ---------------------------------------------------------------- D09 lambda
// 返回一个 lambda：调用它并传入 x，得到 x + n
std::function<int(int)> makeAdder(int n);

// ---------------------------------------------------------------- D10 模板
// 空 vector 抛 std::invalid_argument；否则返回最大元素
template <typename T>
T maxOf(const std::vector<T>& values) {
    (void)values;
    throw std::logic_error("TODO: D10 实现 maxOf");
    return T{};
}

// ---------------------------------------------------------------- D11 异常
// 把字符串转成 int；输入非法（空、含非数字、溢出）抛 std::invalid_argument
int parseInt(const std::string& text);

// ---------------------------------------------------------------- D12 optional + 文件
// 读取文件里第一个能解析成 double 的数字；文件打不开、没有数字则返回 std::nullopt
std::optional<double> readFirstNumber(const std::string& path);

}  // namespace drills
