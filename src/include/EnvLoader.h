#pragma once
#include <string>
#include <unordered_map>

class EnvLoader {
public:
    static void loadEnvFile(const std::string& path = ".env");
};