#pragma once
#include <map>
#include <deque>
#include <string>
#include <mutex>

struct HistoricalStats {
    double avg;
    double stddev;
    bool trendingDown;
};

class HistoricalPriceTracker {
public:
    HistoricalPriceTracker(size_t windowSize = 12); // e.g. last 2 mins if sampled every 10s

    void addPrice(const std::string& coin, double price);
    HistoricalStats analyze(const std::string& coin);

private:
    size_t windowSize_;
    std::map<std::string, std::deque<double>> history_;
    std::mutex mtx_;
};