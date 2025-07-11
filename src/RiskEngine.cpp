#include "RiskEngine.h"

RiskEngine::RiskEngine(double medium, double high)
    : mediumThreshold(medium), highThreshold(high) {}

RiskLevel RiskEngine::assessRisk(double price) {
    if (price >= mediumThreshold) {
        return RiskLevel::SAFE;
    } else if (price >= highThreshold) {
        return RiskLevel::MEDIUM;
    } else {
        return RiskLevel::HIGH;
    }
}

std::string RiskEngine::riskToString(RiskLevel level) {
    switch (level) {
        case RiskLevel::SAFE: return "🟢 SAFE";
        case RiskLevel::MEDIUM: return "🟡 MEDIUM RISK";
        case RiskLevel::HIGH: return "🔴 HIGH RISK";
        default: return "UNKNOWN";
    }
}

// std::string getCurrentTimestamp() {
//     auto now = std::chrono::system_clock::now();
//     std::time_t t = std::chrono::system_clock::to_time_t(now);
//     std::stringstream ss;
//     ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
//     return ss.str();
// }
