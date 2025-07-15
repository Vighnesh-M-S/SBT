#include "CurveClient.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include <filesystem>

static Json::Value lastCurveData;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

void CurveClient::fetchCurveTokens() {
    const char* apiKey = std::getenv("THEGRAPH_API_KEY");
    if (!apiKey) {
        std::cerr << "❌ THEGRAPH_API_KEY not set in environment." << std::endl;
        return;
    }

    std::string url = "https://gateway.thegraph.com/api/subgraphs/id/3fy93eAT56UJsRCEht8iFhfi6wjHWXtZ9dnnbQmvFopF";

    std::string query = R"({
        tokens(first: 5) {
            id
            name
            symbol
            decimals
        }
        rewardTokens(first: 5) {
            id
            token {
                id
            }
            type
        }
    })";

    Json::Value jsonRequest;
    jsonRequest["query"] = query;
    jsonRequest["operationName"] = "Subgraphs";
    jsonRequest["variables"] = Json::objectValue;

    Json::StreamWriterBuilder writer;
    std::string requestBody = Json::writeString(writer, jsonRequest);

    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        std::string authHeader = "Authorization: Bearer ";
        authHeader += apiKey;
        headers = curl_slist_append(headers, authHeader.c_str());

        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "❌ CURL error: " << curl_easy_strerror(res) << std::endl;
            return;
        }

        Json::Value root;
        Json::CharReaderBuilder reader;
        std::string errs;
        std::istringstream s(response);
        if (!Json::parseFromStream(reader, s, &root, &errs)) {
            std::cerr << "❌ Failed to parse JSON response from TheGraph: " << errs << std::endl;
            return;
        }

        if (root.isMember("errors")) {
            std::cerr << "❌ Error in GraphQL response: " << root["errors"] << std::endl;
            return;
        }
        lastCurveData = root["data"];  // Save for scoring
    }
}

double CurveClient::computeCurveRisk() {
    if (lastCurveData.isNull()) {
        std::cerr << "⚠️ No Curve data available to compute risk.\n";
        return 1.0;  // Max risk
    }

    const auto& tokens = lastCurveData["tokens"];
    const auto& rewards = lastCurveData["rewardTokens"];

    int tokenCount = tokens.isArray() ? tokens.size() : 0;
    int rewardCount = rewards.isArray() ? rewards.size() : 0;

    // Simple scoring logic: More tokens/rewards → less risk
    int maxPossible = 10;
    int total = tokenCount + rewardCount;


    return score;
}

void CurveClient::updateCurveScoreCSV(double score, const std::string& csvPath) {
    std::ifstream in(csvPath);
    if (!in) {
        std::cerr << "❌ [Curve] Failed to open CSV: " << csvPath << "\n";
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) lines.push_back(line);
    in.close();

    if (lines.size() < 2) {
        std::cerr << "⚠️ [Curve] Not enough data rows in CSV to update.\n";
        return;
    }

    std::stringstream ss(lines.back());
    std::string field;
    std::vector<std::string> fields;
    while (std::getline(ss, field, ',')) fields.push_back(field);

    if (fields.size() >= 6) {
        std::ostringstream scoreStream;
        scoreStream << std::fixed << std::setprecision(6) << score;
        fields[4] = scoreStream.str();  // curveRisk column

        std::ostringstream updatedLine;
        for (size_t i = 0; i < fields.size(); ++i) {
            updatedLine << fields[i];
            if (i < fields.size() - 1) updatedLine << ",";
        }
        lines.back() = updatedLine.str();

        std::ofstream out(csvPath);
        for (const auto& l : lines) out << l << "\n";
    }
}