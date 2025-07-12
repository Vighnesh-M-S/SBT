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

    // Normalize stddev
    double normStdDev = std::min(stats.stddev / 0.005, 1.0);
    double stddevScore = normStdDev * stddevWeight;

    // Trend score
    double trendScore = stats.trendingDown ? trendWeight : 0.0;

    // Final risk score
    double score = stddevScore + trendScore;
    if (score < 1e-6) score = 0.0;
    stats.riskScore = score;

    // Timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream timestamp;
    timestamp << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");

    // Check if file exists
    bool fileExists = std::ifstream(csvPath).good();

    // Open in append mode
    std::ofstream fileOut(csvPath, std::ios::app);
    if (!fileOut) {
        std::cerr << "❌ Failed to open CSV for writing: " << csvPath << "\n";
        return;
    }

    // Write header if file is new
    if (!fileExists) {
        fileOut << "timestamp,trendScore,tweetScore,liquidityRisk,redemptionRisk,bridgeRisk,whaleRisk,uniswapRisk\n";
    }

    // Append new row
    fileOut << timestamp.str() << "," << score
            << ",0.0,0.0,0.0,0.0,0.0,0.0\n";

    std::cout << "📈 [Historical] Appended trend risk score = " << score << " at " << timestamp.str() << "\n";
}