#include "flightlog/report.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "flightlog/analysis.h"

namespace flightlog {
namespace {

std::string fixed(double value, int precision = 1) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(precision) << value;
    return os.str();
}

/// 取某阶段某字段的统计量。
/// 注意：必须先落地 vector 再取迭代器 —— 直接写
/// computeStats(collect(...).begin(), collect(...).end()) 会让 begin/end
/// 来自两个不同的临时对象，是未定义行为。
Stats<double> statsOf(const std::vector<FlightRecord>& records,
                      const PhaseSpan& span,
                      double FlightRecord::*field) {
    const std::vector<double> values = collectField(records, span, field);
    return computeStats(values.begin(), values.end());
}

}  // namespace

void writeMarkdownReport(const std::string& path,
                         const std::vector<FlightRecord>& records,
                         const std::vector<PhaseSpan>& phases,
                         const std::vector<Exceedance>& exceedances) {
    // std::ofstream 本身就是 RAII：离开作用域自动关闭
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("无法写入报告文件: " + path);
    }

    out << "# 飞行数据分析报告\n\n";
    out << "- 采样点总数：" << records.size() << "\n";
    out << "- 时间跨度：" << fixed(records.front().time_s, 0) << " s ~ "
        << fixed(records.back().time_s, 0) << " s\n";
    out << "- 阶段数：" << phases.size() << "\n";
    out << "- 超限事件：" << exceedances.size() << " 次\n\n";

    out << "## 各阶段统计\n\n";
    out << "| 阶段 | 采样点 | 高度均值(ft) | 高度范围(ft) | 空速均值(kt) | 空速最大(kt) |\n";
    out << "|---|---|---|---|---|---|\n";
    for (const auto& span : phases) {
        const auto alt = statsOf(records, span, &FlightRecord::alt_ft);
        const auto ias = statsOf(records, span, &FlightRecord::ias_kt);
        out << "| " << toString(span.phase) << " | " << span.size() << " | " << fixed(alt.mean, 0)
            << " | " << fixed(alt.min, 0) << " ~ " << fixed(alt.max, 0) << " | "
            << fixed(ias.mean, 0) << " | " << fixed(ias.max, 0) << " |\n";
    }

    out << "\n## 超限事件明细\n\n";
    if (exceedances.empty()) {
        out << "无。\n";
    } else {
        out << "| 时间(s) | 类型 | 数值 | 阈值 |\n|---|---|---|---|\n";
        for (const auto& e : exceedances) {
            out << "| " << fixed(e.time_s, 0) << " | " << toString(e.kind) << " | "
                << fixed(e.value, 1) << " | " << fixed(e.limit, 1) << " |\n";
        }
    }
    out << "\n---\n\n> 由 flightlog-analyzer 生成。\n";
}

void printSummary(const std::vector<FlightRecord>& records,
                  const std::vector<PhaseSpan>& phases,
                  const std::vector<Exceedance>& exceedances) {
    std::cout << "采样点: " << records.size() << "  阶段数: " << phases.size()
              << "  超限事件: " << exceedances.size() << "\n";

    const PhaseSpan whole{Phase::Cruise, 0, records.size()};
    const auto alt = statsOf(records, whole, &FlightRecord::alt_ft);
    std::cout << "全程高度: " << fixed(alt.min, 0) << " ~ " << fixed(alt.max, 0) << " ft, 均值 "
              << fixed(alt.mean, 0) << " ft\n";

    for (const auto& span : phases) {
        std::cout << "  - [" << fixed(records[span.begin].time_s, 0) << "s ~ "
                  << fixed(records[span.end - 1].time_s, 0) << "s] " << toString(span.phase)
                  << " (" << span.size() << " 点)\n";
    }

    for (const auto& e : exceedances) {
        std::cout << "  ! t=" << fixed(e.time_s, 0) << "s " << toString(e.kind) << " 值 "
                  << fixed(e.value, 1) << " 阈值 " << fixed(e.limit, 1) << "\n";
    }
}

}  // namespace flightlog
