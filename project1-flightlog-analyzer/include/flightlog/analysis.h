#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <vector>

#include "flightlog/record.h"

namespace flightlog {

/// 通用统计结果（极值 + 均值 + 样本数）
template <typename T>
struct Stats {
    T      min{};
    T      max{};
    double mean = 0.0;
    std::size_t count = 0;
};

/// 对任意迭代器区间做统计。
///
/// 演示点：模板 + STL 算法 + 迭代器，而不是手写 for 循环。
/// 区间为空时返回 count == 0 的默认值。
template <typename Iter>
Stats<typename std::iterator_traits<Iter>::value_type> computeStats(Iter first, Iter last) {
    using T = typename std::iterator_traits<Iter>::value_type;
    Stats<T> s;
    s.count = static_cast<std::size_t>(std::distance(first, last));
    if (s.count == 0) {
        return s;
    }
    const auto minmax = std::minmax_element(first, last);
    s.min = *minmax.first;
    s.max = *minmax.second;
    const double sum = std::accumulate(first, last, 0.0);
    s.mean = sum / static_cast<double>(s.count);
    return s;
}

/// 按高度变化率把航迹切分为飞行阶段
std::vector<PhaseSpan> segmentPhases(const std::vector<FlightRecord>& records);

/// 超限检测：滚转 ±30°、俯仰 ±20°、空速 > 250 kt、巡航高度低于 500 ft
std::vector<Exceedance> detectExceedances(const std::vector<FlightRecord>& records);

/// 取某阶段区间内某个字段的序列（配合 computeStats 使用）
std::vector<double> collectField(const std::vector<FlightRecord>& records,
                                 const PhaseSpan& span,
                                 double FlightRecord::*field);

}  // namespace flightlog
