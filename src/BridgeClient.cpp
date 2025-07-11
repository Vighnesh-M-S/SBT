#include "BridgeClient.h"
#include <iostream>
#include <cstdlib>
#include <curl/curl.h>
#include <json/json.h>
#include <unordered_map>
#include <set>
#include <iomanip>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}


void BridgeClient::fetchBridgeEvents() {
    const char* apiKey = std::getenv("THEGRAPH_API_KEY");
    if (!apiKey) {
        std::cerr << "❌ Missing THEGRAPH_API_KEY\n";
        return;
    }

    const std::string url = "https://gateway.thegraph.com/api/subgraphs/id/E8ihHMv3ehsjNpopsTAsA8n4tKcNLLhcKUgomRn2cKmF";

    const std::string queryPayload = R"({
        "query": "{ addInboundProofLibraryForChains(first: 5) { id chainId lib blockNumber } appConfigUpdateds(first: 5) { id userApplication configType newConfig } }"
    })";

    std::string response;
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        std::string authHeader = std::string("Authorization: Bearer ") + apiKey;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, authHeader.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, queryPayload.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            std::cerr << "❌ cURL error: " << curl_easy_strerror(res) << "\n";
            return;
        }

        if (response.find("<!DOCTYPE html>") != std::string::npos) {
            std::cerr << "❌ Received HTML instead of JSON. Check the endpoint or API key.\n";
            return;
        }

        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(response, root)) {
            std::cerr << "❌ JSON parse error from LayerZero: " << reader.getFormattedErrorMessages() << "\n";
            return;
        }
        BridgeClient::analyzeBridgeEvents(root["data"]);

        // std::cout << "✅ Bridge Events Response Parsed:\n";
        // std::cout << root.toStyledString() << "\n";
    }
}

void BridgeClient::analyzeBridgeEvents(const Json::Value& data) {
    const auto& inbound = data["addInboundProofLibraryForChains"];
    const auto& configUpdates = data["appConfigUpdateds"];

    std::unordered_map<int, int> chainEventCount;
    std::unordered_map<std::string, int> configTypeCount;
    std::set<std::string> uniqueUsers;

    // Analyze inbound proof libraries
    for (const auto& entry : inbound) {
        int chainId = entry["chainId"].asInt();
        chainEventCount[chainId]++;
    }

    // Analyze config updates
    for (const auto& entry : configUpdates) {
        std::string configType = entry["configType"].asString();
        std::string userApp = entry["userApplication"].asString();
        configTypeCount[configType]++;
        uniqueUsers.insert(userApp);
    }

    // Print bridge activity summary
    std::cout << "🌉 Bridge Activity Summary:\n";

    std::cout << "🔢 Chain Event Counts:\n";
    for (const auto& [chainId, count] : chainEventCount) {
        std::cout << "  - Chain ID " << std::setw(4) << chainId << ": " << count << " updates\n";
    }

    std::cout << "⚙️ Config Type Updates:\n";
    for (const auto& [type, count] : configTypeCount) {
        std::string alert = (count > 2) ? " ⚠️" : "";
        std::cout << "  - Type " << type << ": " << count << " changes" << alert << "\n";
    }

    std::cout << "👥 Unique apps affected: " << uniqueUsers.size() << "\n";
}
