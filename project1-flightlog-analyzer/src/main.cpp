#include <exception>
#include <iostream>
#include <string>

#include "flightlog/analysis.h"
#include "flightlog/csv.h"
#include "flightlog/report.h"

namespace {

void printUsage(const char* program) {
    std::cout << "用法: " << program << " <input.csv> [--out report.md]\n"
              << "示例: " << program << " data/sample.csv --out report.md\n";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    const std::string input = argv[1];
    std::string output = "report.md";
    for (int i = 2; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == "--out") {
            output = argv[i + 1];
            ++i;
        }
    }

    try {
        const auto records = flightlog::loadCsv(input);
        const auto phases = flightlog::segmentPhases(records);
        const auto exceedances = flightlog::detectExceedances(records);

        flightlog::printSummary(records, phases, exceedances);
        flightlog::writeMarkdownReport(output, records, phases, exceedances);
        std::cout << "\n报告已写入: " << output << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "错误: " << ex.what() << "\n";
        return 2;
    }
    return 0;
}
