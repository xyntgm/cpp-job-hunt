#pragma once

#include <string>
#include <vector>

#include "flightlog/record.h"

namespace flightlog {

/// 生成 Markdown 分析报告。
///
/// @throws std::runtime_error 文件无法写入
void writeMarkdownReport(const std::string& path,
                         const std::vector<FlightRecord>& records,
                         const std::vector<PhaseSpan>& phases,
                         const std::vector<Exceedance>& exceedances);

/// 在终端打印一份简要摘要
void printSummary(const std::vector<FlightRecord>& records,
                  const std::vector<PhaseSpan>& phases,
                  const std::vector<Exceedance>& exceedances);

}  // namespace flightlog
