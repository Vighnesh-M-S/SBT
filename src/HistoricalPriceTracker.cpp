#include "HistoricalPriceTracker.h"
#include <cmath>
#include <numeric>

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
    HistoricalStats stats{0.0, 0.0, false};
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

    // Downward trend: majority of prices are decreasing
    int downMoves = 0;
    for (size_t i = 1; i < prices.size(); ++i) {
        if (prices[i] < prices[i - 1]) downMoves++;
    }
    stats.trendingDown = (downMoves >= prices.size() / 2);

    return stats;
}