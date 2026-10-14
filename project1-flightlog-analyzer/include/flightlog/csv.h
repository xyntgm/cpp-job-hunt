#pragma once

#include <string>
#include <vector>

#include "flightlog/record.h"

namespace flightlog {

/// 读取飞行参数 CSV。
///
/// 表头必须为：time_s,alt_ft,ias_kt,pitch_deg,roll_deg,n1_pct
/// 空行与以 '#' 开头的注释行会被忽略。
///
/// @throws std::runtime_error 文件打不开、表头不匹配、字段数不符或数值非法
std::vector<FlightRecord> loadCsv(const std::string& path);

}  // namespace flightlog
