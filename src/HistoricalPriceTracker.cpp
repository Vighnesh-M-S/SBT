#include "HistoricalPriceTracker.h"
#include <cmath>
#include <numeric>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <filesystem>


HistoricalPriceTracker::HistoricalPriceTracker(size_t windowSize)
    : windowSize_(windowSize) {}

void HistoricalPriceTracker::addPrice(const std::string& coin, double price) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto& deque = history_[coin];
    deque.push_back(price);
    if (deque.size() > windowSize_) {
        deque.pop_front();
    }
}

HistoricalStats HistoricalPriceTracker::analyze(const std::string& coin) {
    std::lock_guard<std::mutex> lock(mtx_);
    HistoricalStats stats{0.0, 0.0, false, 0.0};
    const auto& prices = history_[coin];
    if (prices.empty()) return stats;

    // Average
    double sum = std::accumulate(prices.begin(), prices.end(), 0.0);
    stats.avg = sum / prices.size();

    // Std deviation
    double sq_sum = 0.0;
    for (double p : prices) {
        sq_sum += (p - stats.avg) * (p - stats.avg);
    }
    stats.stddev = std::sqrt(sq_sum / prices.size());

    // Downward trend
    int downMoves = 0;
    for (size_t i = 1; i < prices.size(); ++i) {
        if (prices[i] < prices[i - 1]) downMoves++;
    }
    stats.trendingDown = (downMoves >= prices.size() / 2);

    // Risk Score: weighted combination (can be tuned)
    stats.riskScore = 0.6 * (stats.trendingDown ? 1.0 : 0.0) + 0.4 * std::min(1.0, stats.stddev);

    return stats;
}

void HistoricalPriceTracker::updateRiskCSV(const std::string& coin, const std::string& csvPath) {
    HistoricalStats stats = analyze(coin);
    double stddevWeight = 0.4;
double trendWeight = 0.6;

// Normalize stddev: scale it to [0, 1] capped at 0.005
double normStdDev = std::min(stats.stddev / 0.005, 1.0);
double stddevScore = normStdDev * stddevWeight;

// Normalize trend: if trendingDown is true, assign full trend weight; else zero
double trendScore = stats.trendingDown ? trendWeight : 0.0;

// Total score
double score = stddevScore + trendScore;
if (score < 1e-6) score = 0.0;
stats.riskScore = score;

    std::cout << "📊 [Debug] avg = " << stats.avg
              << ", stddev = " << stats.stddev
              << ", trendingDown = " << stats.trendingDown
              << ", riskScore = " << score << "\n";

    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream timestamp;
    timestamp << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");

    // Read existing lines
    std::ifstream fileIn(csvPath);
    std::vector<std::string> lines;
    std::string line;

    // Read header + data
    bool updated = false;
    if (fileIn.is_open()) {
        std::getline(fileIn, line); // header
        lines.push_back(line);
        while (std::getline(fileIn, line)) {
            lines.push_back(line);
        }
        fileIn.close();
    }

    // Update last row if exists
    if (lines.size() > 1) {
        std::stringstream ss(lines.back());
        std::string field;
        std::vector<std::string> fields;
        while (std::getline(ss, field, ',')) fields.push_back(field);

        if (fields.size() >= 6) {
            fields[0] = timestamp.str(); // update timestamp
            fields[1] = std::to_string(score); // update trend score

            std::ostringstream updatedLine;
            for (size_t i = 0; i < fields.size(); ++i) {
                updatedLine << fields[i];
                if (i < fields.size() - 1) updatedLine << ",";
            }
            lines.back() = updatedLine.str();
            updated = true;
        }
    }

    // Append if not updated
    if (!updated) {
        std::ostringstream newLine;
        newLine << timestamp.str() << "," << score << ",0.0,0.0,0.0,0.0";
        lines.push_back(newLine.str());
    }

    // Write everything back
    std::ofstream fileOut("/Users/vighneshms/Downloads/SBT/src/model_scores.csv", std::ios::app);
    for (const auto& l : lines) fileOut << l << "\n";

    std::cout << "📈 [Historical] Updated trend risk score = " << score << " at " << timestamp.str() << "\n";
}