#pragma once
#include <map>
#include <deque>
#include <string>
#include <mutex>

struct HistoricalStats {
    double avg;
    double stddev;
    bool trendingDown;
    double riskScore;
};

class HistoricalPriceTracker {
public:
    HistoricalPriceTracker(size_t windowSize = 60); // e.g., 10 mins of data if sampled every 10s

    void addPrice(const std::string& coin, double price);
    HistoricalStats analyze(const std::string& coin);
    void updateRiskCSV(const std::string& coin, const std::string& csvPath);

private:
    size_t windowSize_;
    std::map<std::string, std::deque<double>> history_;
    std::mutex mtx_;
};