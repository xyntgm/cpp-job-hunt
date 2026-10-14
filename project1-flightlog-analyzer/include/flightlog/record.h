#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace flightlog {

/// 一个采样点的飞行参数
struct FlightRecord {
    double time_s    = 0.0;  ///< 相对时间（秒）
    double alt_ft    = 0.0;  ///< 气压高度（英尺）
    double ias_kt    = 0.0;  ///< 指示空速（节）
    double pitch_deg = 0.0;  ///< 俯仰角（度）
    double roll_deg  = 0.0;  ///< 滚转角（度）
    double n1_pct    = 0.0;  ///< 发动机 N1 转速（百分比）
};

/// 飞行阶段
enum class Phase { Ground, Climb, Cruise, Descent };

const char* toString(Phase p);

/// 一个飞行阶段覆盖的记录区间 [begin, end)
struct PhaseSpan {
    Phase  phase;
    size_t begin = 0;
    size_t end   = 0;

    size_t size() const { return end - begin; }
};

/// 超限事件类型
enum class ExceedanceKind {
    RollExceed,
    PitchExceed,
    Overspeed,
    LowAltitude
};

const char* toString(ExceedanceKind k);

/// 一条超限事件
struct Exceedance {
    ExceedanceKind kind;
    double         time_s = 0.0;
    double         value  = 0.0;
    double         limit  = 0.0;
};

}  // namespace flightlog
