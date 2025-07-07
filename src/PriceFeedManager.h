#pragma once
#include <string>
#include <unordered_map>
#include <mutex>

struct PriceData {
    double price;
    long timestamp;
};

class PriceFeedManager {
public:
    void start();
    PriceData getPrice(const std::string& coin);

private:
    std::unordered_map<std::string, PriceData> prices_;
    std::mutex mtx_;
    void fetchPricesLoop();
};
