#include "flightlog/analysis.h"

#include <cmath>

namespace flightlog {

const char* toString(Phase p) {
    switch (p) {
        case Phase::Ground:  return "地面";
        case Phase::Climb:   return "爬升";
        case Phase::Cruise:  return "巡航";
        case Phase::Descent: return "下降";
    }
    return "未知";
}

const char* toString(ExceedanceKind k) {
    switch (k) {
        case ExceedanceKind::RollExceed:  return "滚转角超限";
        case ExceedanceKind::PitchExceed: return "俯仰角超限";
        case ExceedanceKind::Overspeed:   return "空速超限";
        case ExceedanceKind::LowAltitude: return "高度过低";
    }
    return "未知";
}

namespace {

// 阈值集中放这里，方便后续做成配置文件
constexpr double kRollLimitDeg   = 30.0;
constexpr double kPitchLimitDeg  = 20.0;
constexpr double kSpeedLimitKt   = 250.0;
constexpr double kMinAltFt       = 500.0;
constexpr double kGroundedAltFt  = 1000.0;  // 低于该高度视为地面
constexpr double kClimbRateFpm   = 300.0;   // 垂直速度阈值（英尺/分钟）

double verticalSpeedFpm(const FlightRecord& prev, const FlightRecord& cur) {
    const double dt_s = cur.time_s - prev.time_s;
    if (dt_s <= 0.0) {
        return 0.0;
    }
    return (cur.alt_ft - prev.alt_ft) / dt_s * 60.0;
}

}  // namespace

std::vector<PhaseSpan> segmentPhases(const std::vector<FlightRecord>& records) {
    std::vector<PhaseSpan> spans;
    if (records.empty()) {
        return spans;
    }

    std::vector<Phase> phases;
    phases.reserve(records.size());
    phases.push_back(records.front().alt_ft < kGroundedAltFt ? Phase::Ground : Phase::Cruise);

    for (std::size_t i = 1; i < records.size(); ++i) {
        const double vs = verticalSpeedFpm(records[i - 1], records[i]);
        Phase p;
        if (records[i].alt_ft < kGroundedAltFt) {
            p = Phase::Ground;
        } else if (vs > kClimbRateFpm) {
            p = Phase::Climb;
        } else if (vs < -kClimbRateFpm) {
            p = Phase::Descent;
        } else {
            p = Phase::Cruise;
        }
        phases.push_back(p);
    }

    // 合并连续相同阶段（演示点：一遍扫描 + 状态机）
    std::size_t begin = 0;
    for (std::size_t i = 1; i <= phases.size(); ++i) {
        if (i == phases.size() || phases[i] != phases[begin]) {
            spans.push_back(PhaseSpan{phases[begin], begin, i});
            begin = i;
        }
    }
    return spans;
}

std::vector<Exceedance> detectExceedances(const std::vector<FlightRecord>& records) {
    std::vector<Exceedance> events;
    for (const auto& r : records) {
        if (std::fabs(r.roll_deg) > kRollLimitDeg) {
            events.push_back({ExceedanceKind::RollExceed, r.time_s, r.roll_deg, kRollLimitDeg});
        }
        if (std::fabs(r.pitch_deg) > kPitchLimitDeg) {
            events.push_back({ExceedanceKind::PitchExceed, r.time_s, r.pitch_deg, kPitchLimitDeg});
        }
        if (r.ias_kt > kSpeedLimitKt) {
            events.push_back({ExceedanceKind::Overspeed, r.time_s, r.ias_kt, kSpeedLimitKt});
        }
        if (r.alt_ft < kMinAltFt) {
            events.push_back({ExceedanceKind::LowAltitude, r.time_s, r.alt_ft, kMinAltFt});
        }
    }
    return events;
}

std::vector<double> collectField(const std::vector<FlightRecord>& records,
                                 const PhaseSpan& span,
                                 double FlightRecord::*field) {
    std::vector<double> values;
    const std::size_t end = span.end < records.size() ? span.end : records.size();
    for (std::size_t i = span.begin; i < end; ++i) {
        values.push_back(records[i].*field);
    }
    return values;
}

}  // namespace flightlog
