#include "EnvLoader.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>

void EnvLoader::loadEnvFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "❌ Failed to open .env file at " << path << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                setenv(key.c_str(), value.c_str(), 1);  // 1 = overwrite
                // std::cout << "🔧 Loaded " << key << " from .env\n";
            }

        }
        
    }
    std::cout << "🔧 Loaded .env file\n";
}
