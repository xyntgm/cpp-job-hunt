// 极简单元测试（不依赖 GTest，保证第一天就能跑起来）。
// W2 接入 GoogleTest 后，把这个文件替换成 GTest 版本。

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "flightlog/analysis.h"
#include "flightlog/csv.h"

namespace {

int g_failed = 0;
int g_total = 0;

void expect(bool cond, const std::string& what) {
    ++g_total;
    if (!cond) {
        ++g_failed;
        std::cout << "[FAIL] " << what << "\n";
    } else {
        std::cout << "[ ok ] " << what << "\n";
    }
}

bool near(double a, double b, double eps = 1e-6) { return std::fabs(a - b) < eps; }

}  // namespace

int main() {
    using namespace flightlog;

    // 1. CSV 解析
    const auto records = loadCsv("data/sample.csv");
    expect(records.size() == 20, "解析出 20 个采样点");
    expect(near(records.front().time_s, 0.0), "首点时间为 0");
    expect(near(records.front().alt_ft, 500.0), "首点高度为 500 ft");

    // 2. 统计
    const std::vector<double> values{1.0, 2.0, 3.0, 4.0};
    const auto s = computeStats(values.begin(), values.end());
    expect(near(s.min, 1.0) && near(s.max, 4.0) && near(s.mean, 2.5) && s.count == 4,
           "computeStats 极值与均值正确");

    const std::vector<double> empty;
    expect(computeStats(empty.begin(), empty.end()).count == 0, "空区间返回 count == 0");

    // 3. 阶段切分
    const auto phases = segmentPhases(records);
    expect(!phases.empty(), "至少切出 1 个阶段");

    // 4. 超限检测：样例数据中 t=420s 处滚转角 35°
    const auto events = detectExceedances(records);
    bool found_roll = false;
    for (const auto& e : events) {
        if (e.kind == ExceedanceKind::RollExceed && near(e.time_s, 420.0)) {
            found_roll = true;
        }
    }
    expect(found_roll, "检出 t=420s 的滚转角超限事件");

    std::cout << "\n" << (g_total - g_failed) << "/" << g_total << " 通过\n";
    return g_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
