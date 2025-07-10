#pragma once
#include <string>
#include <map>

enum class RiskLevel {
    SAFE,
    MEDIUM,
    HIGH
};

class RiskEngine {
public:
    RiskLevel assessRisk(double price);
    std::string riskToString(RiskLevel level);
};
