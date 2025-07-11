#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>

double computeScore(double p, double t, double l, double r, double b) {
    double w1 = 0.25, w2 = 0.25, w3 = 0.2, w4 = 0.15, w5 = 0.15;
    return std::min(1.0, w1*p + w2*t + w3*l + w4*r + w5*b);
}

std::vector<std::string> splitCSVLine(const std::string& line) {
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> result;
    while (std::getline(ss, item, ',')) result.push_back(item);
    return result;
}

void runDepegMonitor() {
    while (true) {
        std::ifstream file("model_scores.csv");
        std::string line, latestLine;

        // Skip header and find last non-empty line
        std::getline(file, line); // header
        while (std::getline(file, line)) {
            if (!line.empty()) latestLine = line;
        }

        if (!latestLine.empty()) {
            auto fields = splitCSVLine(latestLine);
            if (fields.size() < 6) {
                std::cerr << "❌ CSV row malformed.\n";
            } else {
                double p = std::stod(fields[1]);
                double t = std::stod(fields[2]);
                double l = std::stod(fields[3]);
                double r = std::stod(fields[4]);
                double b = std::stod(fields[5]);
                double score = computeScore(p, t, l, r, b);

                std::cout << "🕒 " << fields[0] << " | 🔮 Depeg Risk Score: " << score << "\n";

                // Optionally append to another file
                std::ofstream out("depeg_predictions.csv", std::ios::app);
                out << fields[0] << "," << score << "\n";
            }
        }

        std::this_thread::sleep_for(std::chrono::minutes(10));
    }
}
