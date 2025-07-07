#include "PriceFeedManager.h"
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <json/json.h> // If using jsoncpp

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void PriceFeedManager::start() {
    std::thread(&PriceFeedManager::fetchPricesLoop, this).detach();
}

void PriceFeedManager::fetchPricesLoop() {
    while (true) {
        CURL* curl = curl_easy_init();
        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.coingecko.com/api/v3/simple/price?ids=usd-coin&vs_currencies=usd");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        Json::Reader reader;
        Json::Value root;
        if (reader.parse(readBuffer, root)) {
            std::lock_guard<std::mutex> lock(mtx_);
            long now = std::time(nullptr);
            prices_["usdc"] = { root["usd-coin"]["usd"].asDouble(), now };
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

PriceData PriceFeedManager::getPrice(const std::string& coin) {
    std::lock_guard<std::mutex> lock(mtx_);
    return prices_[coin];
}