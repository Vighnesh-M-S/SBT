#include "EtherscanClient.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <curl/curl.h>
#include <json/json.h>

struct Transfer {
    std::string from;
    std::string to;
    std::string value;
    long timestamp;
};

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

void fetchUSDCTransfers() {
    const char* apiKey = std::getenv("ETHERSCAN_API_KEY");
    if (!apiKey) {
        std::cerr << "❌ API key not found! Did you load your .env file?\n";
        return;
    }

    const std::string usdcContract = "0xA0b86991c6218b36c1d19D4a2e9Eb0cE3606eb48";
    std::ostringstream urlStream;
    urlStream << "https://api.etherscan.io/api?module=account&action=tokentx"
              << "&contractaddress=" << usdcContract
              << "&startblock=0&endblock=99999999&sort=desc"
              << "&apikey=" << apiKey;

    std::string url = urlStream.str();
    std::string response;

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "❌ CURL error: " << curl_easy_strerror(res) << std::endl;
            return;
        }

        Json::Value root;
        Json::Reader reader;
        std::vector<Transfer> largeTransfers;

        if (reader.parse(response, root)) {
            if (root["status"].asString() != "1") {
                std::cerr << "⚠️  Etherscan API error: " << root["message"].asString() << std::endl;
                return;
            }

            const Json::Value& txs = root["result"];
            for (const auto& tx : txs) {
                double value = std::stod(tx["value"].asString()) / 1e6; // USDC is 6 decimals

                if (value >= 100000.0) {
                    Transfer t;
                    t.from = tx["from"].asString();
                    t.to = tx["to"].asString();
                    t.value = tx["value"].asString();
                    t.timestamp = std::stol(tx["timeStamp"].asString());
                    largeTransfers.push_back(t);
                }
            }

            std::cout << "💸 Large USDC Transfers: " << largeTransfers.size()
                      << " detected in last fetch.\n";
        } else {
            std::cerr << "❌ Failed to parse Etherscan JSON response.\n";
        }
    }
}