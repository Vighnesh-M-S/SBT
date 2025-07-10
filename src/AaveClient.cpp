// AaveClient.cpp
#include "AaveClient.h"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include <cstdlib>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

void AaveClient::fetchAaveLiquidity() {
    const char* apiKey = std::getenv("THEGRAPH_API_KEY");
    if (!apiKey) {
        std::cerr << "❌ GRAPH_API_KEY not set in environment.\n";
        return;
    }

    const std::string url = "https://gateway.thegraph.com/api/subgraphs/id/GQFbb95cE6d8mV989mL5figjaGaKCQB3xqYrr1bRyXqF";

    std::string query = "{ reserves(first: 5) { id name symbol totalLiquidity availableLiquidity } }";
    std::string requestBody = "{\"query\": \"" + query + "\"}";

    std::string response;
    CURL* curl = curl_easy_init();

    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string authHeader = "Authorization: Bearer " + std::string(apiKey);
        headers = curl_slist_append(headers, authHeader.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            std::cerr << "❌ CURL error: " << curl_easy_strerror(res) << std::endl;
            return;
        }

        // Parse JSON response
        Json::Reader reader;
        Json::Value root;
        if (reader.parse(response, root)) {
            const auto& reserves = root["data"]["reserves"];
            if (!reserves.isArray()) {
                std::cerr << "❌ Invalid format from TheGraph\n";
                std::cerr << "Raw response: " << response << "\n";
                return;
            }

            std::cout << "📊 Aave Liquidity Snapshot:\n";
            for (const auto& reserve : reserves) {
                std::string name = reserve["name"].asString();
                std::string symbol = reserve["symbol"].asString();
                std::string liquidity = reserve["availableLiquidity"].asString();

                std::cout << " - " << symbol << " (" << name << ") → Available: " << liquidity << "\n";
            }
        } else {
            std::cerr << "❌ Failed to parse JSON response from TheGraph.\n";
        }
    }
}
