#include "RiskEngine.h"

RiskLevel RiskEngine::assessRisk(double price) {
    if (price >= 0.995) {
        return RiskLevel::SAFE;
    } else if (price >= 0.990) {
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
