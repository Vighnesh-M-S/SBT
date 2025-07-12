#include "DepegPredictor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

double computeDepegRiskScore(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (!file) {
        std::cerr << "❌ Failed to open " << csvPath << "\n";
        return -1.0;
    }

    std::string line;
    std::vector<std::string> lines;

    // Read all lines
    while (std::getline(file, line)) {
        if (!line.empty())
            lines.push_back(line);
    }

    if (lines.size() <= 1) {
        std::cerr << "⚠️ Not enough data in CSV.\n";
        return -1.0;
    }

    std::string lastLine = lines.back();
    std::stringstream ss(lastLine);
    std::string field;
    std::vector<std::string> fields;

    while (std::getline(ss, field, ',')) {
        fields.push_back(field);
    }

    if (fields.size() < 7) {
        std::cerr << "❌ CSV format invalid. Found only " << fields.size() << " fields.\n";
        return -1.0;
    }

    // Extract scores
    double priceTrend = std::stod(fields[1]);
    double tweetScore = std::stod(fields[2]);
    double liquidity = std::stod(fields[3]);
    double redemption = std::stod(fields[4]);
    double bridge = std::stod(fields[5]);
    double whale = std::stod(fields[6]);
    double tvl = std::stod(fields[7]);

    // Apply weights
    double riskScore =
    0.25 * priceTrend +
    0.10 * tweetScore +
    0.20 * liquidity +
    0.10 * redemption +
    0.10 * bridge +
    0.10 * whale +
    0.15 * tvl;

    std::cout << "📉 [DEPEG PREDICTOR] Combined Risk Score = " << riskScore << "\n";

    if (riskScore >= 0.7) {
        std::cout << "🚨 [ALERT] High depeg risk detected!\n";
    }
    else {
        std::cout << "🚨 [ALERT] Low depeg risk detected!\n";
    }

    return riskScore;
}
