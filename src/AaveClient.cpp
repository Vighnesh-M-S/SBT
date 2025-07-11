#include "AaveClient.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <curl/curl.h>
#include <json/json.h>
#include <filesystem>
#include <chrono>
#include <iomanip>

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

    std::string query = "{ reserves(first: 5) { availableLiquidity } }";
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

        Json::Reader reader;
        Json::Value root;
        if (reader.parse(response, root)) {
            const auto& reserves = root["data"]["reserves"];
            totalAvailableLiquidity_ = 0.0;
            for (const auto& reserve : reserves) {
                double liquidity = std::stod(reserve["availableLiquidity"].asString());
                totalAvailableLiquidity_ += liquidity;
            }
            std::cout << "💧 Total Aave Available Liquidity = " << totalAvailableLiquidity_ << "\n";
        } else {
            std::cerr << "❌ Failed to parse TheGraph response\n";
        }
    }
}

double AaveClient::computeLiquidityRisk() const {
    // Assume higher risk if liquidity is very low
    if (totalAvailableLiquidity_ == 0.0) return 1.0;
    double risk = 1.0 / (1.0 + totalAvailableLiquidity_ / 1e8);
    std::cout << "⚠️ [Aave] Computed liquidity risk score = " << risk << "\n";
    return risk;
}

void AaveClient::updateLiquidityScoreCSV(const std::string& csvPath) const {
    double score = computeLiquidityRisk();
    const double MAX_LIQUIDITY = 1e24;
    double normalized = std::min(score / MAX_LIQUIDITY, 1.0);

    std::ifstream file(csvPath);
    if (!file) {
        std::cerr << "❌ Could not open file: " << csvPath << "\n";
        return;
    }

    std::vector<std::string> lines;
    std::string line;

    // Read header
    if (std::getline(file, line)) {
        lines.push_back(line); // header
    }

    // Read data lines
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    if (lines.size() <= 1) {
        std::cerr << "⚠️ CSV has no data rows to update.\n";
        return;
    }

    // Update last line
    std::string& lastLine = lines.back();
    std::stringstream ss(lastLine);
    std::vector<std::string> fields;
    std::string field;

    while (std::getline(ss, field, ',')) {
        fields.push_back(field);
    }

    if (fields.size() >= 6) {
        std::ostringstream scoreStream;
        scoreStream << std::fixed << std::setprecision(6) << normalized;
        fields[3] = scoreStream.str(); // liquidityRisk column

        std::ostringstream updatedLine;
        for (size_t i = 0; i < fields.size(); ++i) {
            updatedLine << fields[i];
            if (i < fields.size() - 1) updatedLine << ",";
        }
        lastLine = updatedLine.str();

        // Write back
        std::ofstream out(csvPath);
        for (const auto& l : lines) {
            out << l << "\n";
        }

        std::cout << "📈 [Aave] ✅ liquidityRisk updated to " << score << " in " << csvPath << "\n";
    } else {
        std::cerr << "❌ Unexpected CSV format (expected >=6 columns)\n";
    }
}
