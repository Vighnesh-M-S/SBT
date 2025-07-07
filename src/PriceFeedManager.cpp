#include "PriceFeedManager.h"

#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <json/json.h>
#include <iostream> // ✅ Needed for std::cout and std::cerr

// Callback function to write the API response into a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Start the fetch loop on a separate thread
void PriceFeedManager::start() {
    std::thread(&PriceFeedManager::fetchPricesLoop, this).detach();
}

// The function that continuously fetches stablecoin prices from CoinGecko
void PriceFeedManager::fetchPricesLoop() {
    while (true) {
        CURL* curl = curl_easy_init();
        std::string readBuffer;

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.coingecko.com/api/v3/simple/price?ids=usd-coin&vs_currencies=usd");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            // Debug log the raw JSON response
            std::cout << "Raw JSON: " << readBuffer << std::endl;

            Json::Reader reader;
            Json::Value root;

            if (reader.parse(readBuffer, root)) {
                if (root.isMember("usd-coin") && root["usd-coin"].isMember("usd")) {
                    std::lock_guard<std::mutex> lock(mtx_);
                    long now = std::time(nullptr);
                    prices_["usdc"] = { root["usd-coin"]["usd"].asDouble(), now };
                } else {
                    std::cerr << "⚠️  Invalid JSON structure or missing 'usd-coin'\n";
                }
            } else {
                std::cerr << "❌ JSON parse error\n";
            }
        } else {
            std::cerr << "❌ CURL error while fetching prices: " << curl_easy_strerror(res) << "\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

// Thread-safe getter for price data
PriceData PriceFeedManager::getPrice(const std::string& coin) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (prices_.count(coin)) {
        return prices_[coin];
    } else {
        return {0.0, 0};  // fallback if coin not available
    }
}