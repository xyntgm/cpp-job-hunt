#include "flightlog/csv.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace flightlog {
namespace {

/// 去掉首尾空白（含 \r，兼容 Windows 换行）
std::string trim(const std::string& s) {
    const auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return {};
    }
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

std::vector<std::string> split(const std::string& line, char delim) {
    std::vector<std::string> out;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, delim)) {
        out.push_back(trim(item));
    }
    return out;
}

double toDouble(const std::string& text, const std::string& field, std::size_t line_no) {
    try {
        std::size_t consumed = 0;
        const double value = std::stod(text, &consumed);
        if (consumed != text.size()) {
            throw std::invalid_argument("trailing characters");
        }
        return value;
    } catch (const std::exception&) {
        throw std::runtime_error("第 " + std::to_string(line_no) + " 行字段 " + field +
                                 " 不是合法数值: '" + text + "'");
    }
}

constexpr const char* kHeader = "time_s,alt_ft,ias_kt,pitch_deg,roll_deg,n1_pct";

}  // namespace

std::vector<FlightRecord> loadCsv(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("无法打开文件: " + path);
    }

    std::vector<FlightRecord> records;
    std::string line;
    std::size_t line_no = 0;
    bool header_checked = false;

    while (std::getline(in, line)) {
        ++line_no;
        const std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        if (!header_checked) {
            if (trimmed != kHeader) {
                throw std::runtime_error("表头不匹配，期望: " + std::string(kHeader));
            }
            header_checked = true;
            continue;
        }

        const auto fields = split(trimmed, ',');
        if (fields.size() != 6) {
            throw std::runtime_error("第 " + std::to_string(line_no) + " 行字段数应为 6，实际 " +
                                     std::to_string(fields.size()));
        }

        FlightRecord r;
        r.time_s    = toDouble(fields[0], "time_s", line_no);
        r.alt_ft    = toDouble(fields[1], "alt_ft", line_no);
        r.ias_kt    = toDouble(fields[2], "ias_kt", line_no);
        r.pitch_deg = toDouble(fields[3], "pitch_deg", line_no);
        r.roll_deg  = toDouble(fields[4], "roll_deg", line_no);
        r.n1_pct    = toDouble(fields[5], "n1_pct", line_no);
        records.push_back(r);
    }

    if (!header_checked) {
        throw std::runtime_error("文件为空或缺少表头: " + path);
    }
    if (records.empty()) {
        throw std::runtime_error("文件中没有任何数据行: " + path);
    }
    return records;
}

}  // namespace flightlog
