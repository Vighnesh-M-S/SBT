#pragma once
#include <string>
#include <map>

enum class RiskLevel {
    SAFE,
    MEDIUM,
    HIGH
};

class RiskEngine {
private:
    double mediumThreshold;
    double highThreshold;
public:
    RiskEngine(double medium = 0.995, double high = 0.990);
    RiskLevel assessRisk(double price);
    std::string riskToString(RiskLevel level);
    std::string getCurrentTimestamp();
};
