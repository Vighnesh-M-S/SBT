#include "UniswapMonitor.h"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include <cstdlib>
#include<sstream>


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

UniswapStats fetchUniswapStats() {
    const char* apiKey = std::getenv("THEGRAPH_API_KEY");
    if (!apiKey) {
        std::cerr << "❌ Missing THEGRAPH_API_KEY in environment variables.\n";
        return {};
    }

    std::string url = "https://gateway.thegraph.com/api/subgraphs/id/5zvR82QoaXYFyDEKLZ9t6v9adgnptxYpKpSbxtgVENFV";
    std::string query = R"({
        "query": "query Subgraphs { factories(first: 1) { id poolCount txCount totalVolumeUSD } bundles(first: 1) { id ethPriceUSD } }",
        "operationName": "Subgraphs",
        "variables": {}
      })";

    std::string response;
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        std::string bearer = std::string("Authorization: Bearer ") + apiKey;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, bearer.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            std::cerr << "❌ CURL error: " << curl_easy_strerror(res) << std::endl;
            return {};
        }
    }

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(response, root)) {
        std::cerr << "❌ Failed to parse JSON response from TheGraph.\n";
        std::cerr << "Raw Response: \n" << response << std::endl;
        return {};
    }
    std::cout << "Raw TVL JSON: " << response << "\n";

    try {
        const auto& data = root["data"];
    
        const auto& factories = data["factories"];
        if (!factories.isArray() || factories.empty())
            throw std::runtime_error("No factories found");
    
        const auto& factory = factories[0];
        std::string totalVolumeUSD = factory["totalVolumeUSD"].asString();
        std::string poolCount = factory["poolCount"].asString();
        std::string txCount = factory["txCount"].asString();
    
        const auto& bundles = data["bundles"];
        if (!bundles.isArray() || bundles.empty())
            throw std::runtime_error("No bundles found");
    
        std::string ethPriceUSD = bundles[0]["ethPriceUSD"].asString();
    
        std::cout << "✅ TVL Response Parsed:\n"
                  << "Total Volume (USD): " << totalVolumeUSD << "\n"
                  << "Pool Count: " << poolCount << "\n"
                  << "Transaction Count: " << txCount << "\n"
                  << "ETH Price (USD): " << ethPriceUSD << "\n";
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception while parsing TVL fields: " << e.what() << "\n";
    }
}
