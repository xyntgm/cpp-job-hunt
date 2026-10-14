// ============================================================================
//  C++ 语法体检 · 全部 12 个练习的实现（现在全是空的，你来填）
//
//  怎么用：
//    1. 先看 tests/test_drills.cpp 里对应的测试 —— 那是需求文档
//    2. 把这里的 throw 换成你的实现
//    3. 重新编译运行，看还有几条 FAIL
//
//  唯一的规则：**卡住超过 30 秒就跳过，记进缺口清单，最后回来补。**
// ============================================================================

#include "drills/drills.h"

namespace drills {

// ---------------------------------------------------------------- D01 指针与引用
void swapByPtr(int* /*a*/, int* /*b*/) {
    throw std::logic_error("TODO: D01 swapByPtr");
}

void swapByRef(int& /*a*/, int& /*b*/) {
    throw std::logic_error("TODO: D01 swapByRef");
}

const int* findMax(const int* /*begin*/, const int* /*end*/) {
    throw std::logic_error("TODO: D01 findMax");
}

// ---------------------------------------------------------------- D02 const 全貌
bool hasPrefix(const std::string& /*text*/, const std::string& /*prefix*/) {
    throw std::logic_error("TODO: D02 hasPrefix");
}

ConstBox::ConstBox(int value) : value_(value) {}

int ConstBox::value() const {
    throw std::logic_error("TODO: D02 ConstBox::value");
}

void ConstBox::setValue(int /*value*/) {
    throw std::logic_error("TODO: D02 ConstBox::setValue");
}

int ConstBox::readCount() const {
    throw std::logic_error("TODO: D02 ConstBox::readCount");
}

// ---------------------------------------------------------------- D03 类与生命周期
int InstanceCounter::alive_   = 0;
int InstanceCounter::next_id_ = 1;

InstanceCounter::InstanceCounter() {
    throw std::logic_error("TODO: D03 InstanceCounter()");
}

InstanceCounter::InstanceCounter(const InstanceCounter& /*other*/) {
    throw std::logic_error("TODO: D03 InstanceCounter(const&)");
}

InstanceCounter& InstanceCounter::operator=(const InstanceCounter& /*other*/) {
    throw std::logic_error("TODO: D03 InstanceCounter::operator=");
}

InstanceCounter::~InstanceCounter() {
    // TODO: D03 —— 析构函数里不能抛异常！这里先留空，避免 terminate
}

int InstanceCounter::alive() {
    throw std::logic_error("TODO: D03 InstanceCounter::alive");
}

int InstanceCounter::id() const {
    throw std::logic_error("TODO: D03 InstanceCounter::id");
}

// ---------------------------------------------------------------- D04 三法则
Buffer::Buffer(std::size_t /*n*/) : data_(nullptr), size_(0) {
    throw std::logic_error("TODO: D04 Buffer(std::size_t)");
}

Buffer::Buffer(const Buffer& /*other*/) : data_(nullptr), size_(0) {
    throw std::logic_error("TODO: D04 Buffer 拷贝构造");
}

Buffer& Buffer::operator=(const Buffer& /*other*/) {
    throw std::logic_error("TODO: D04 Buffer 拷贝赋值（注意自赋值！）");
}

Buffer::~Buffer() {
    // TODO: D04 —— 释放 data_（同样不能抛异常）
}

std::size_t Buffer::size() const { return size_; }

unsigned char* Buffer::data() { return data_; }

const unsigned char* Buffer::data() const { return data_; }

// ---------------------------------------------------------------- D06 智能指针
Device::Device(int /*id*/) {
    throw std::logic_error("TODO: D06 Device::Device");
}

int Device::id() const {
    throw std::logic_error("TODO: D06 Device::id");
}

std::unique_ptr<Device> makeDevice(int /*id*/) {
    throw std::logic_error("TODO: D06 makeDevice");
}

std::size_t useCountOf(const std::shared_ptr<Device>& /*p*/) {
    throw std::logic_error("TODO: D06 useCountOf");
}

// ---------------------------------------------------------------- D07 容器选型
std::unordered_map<std::string, int> countWords(const std::vector<std::string>& /*words*/) {
    throw std::logic_error("TODO: D07 countWords");
}

std::vector<int> topK(std::vector<int> /*values*/, std::size_t /*k*/) {
    throw std::logic_error("TODO: D07 topK");
}

// ---------------------------------------------------------------- D08 迭代器与算法
double mean(const std::vector<double>& /*values*/) {
    throw std::logic_error("TODO: D08 mean");
}

std::size_t countGreater(const std::vector<double>& /*values*/, double /*threshold*/) {
    throw std::logic_error("TODO: D08 countGreater");
}

std::vector<double> scaleAll(const std::vector<double>& /*values*/, double /*factor*/) {
    throw std::logic_error("TODO: D08 scaleAll");
}

// ---------------------------------------------------------------- D09 lambda
std::function<int(int)> makeAdder(int /*n*/) {
    throw std::logic_error("TODO: D09 makeAdder");
}

// ---------------------------------------------------------------- D11 异常
int parseInt(const std::string& /*text*/) {
    throw std::logic_error("TODO: D11 parseInt");
}

// ---------------------------------------------------------------- D12 optional
std::optional<double> readFirstNumber(const std::string& /*path*/) {
    throw std::logic_error("TODO: D12 readFirstNumber");
}

}  // namespace drills
