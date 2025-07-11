// #include "RiskSnapshot.h"
// #include <fstream>
// #include <nlohmann/json.hpp> // or use Json::Value if using JsonCPP
// using json = nlohmann::json;

// void saveSnapshotToFile(const std::vector<RiskSnapshot>& snapshots, const std::string& filename = "risk_log.json") {
//     json output;

//     for (const auto& snap : snapshots) {
//         output.push_back({
//             {"coin", snap.coin},
//             {"price", snap.price},
//             {"riskLevel", snap.riskLevel},
//             {"trendingDown", snap.trendingDown},
//             {"volatility", snap.volatility},
//             {"socialMentions", snap.socialMentions},
//             {"uniswapTVL", snap.uniswapTVL},
//             {"aaveLiquidity", snap.aaveLiquidity},
//             {"bridgeUpdates", snap.bridgeUpdates},
//             {"timestamp", snap.timestamp}
//         });
//     }

//     std::ofstream outFile(filename);
//     outFile << output.dump(4); // Pretty print
//     outFile.close();
// }