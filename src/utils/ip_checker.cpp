#include "ip_checker.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

bool is_ip_allowed(const std::string &ip, const std::vector<std::string> &allowed_ips) {
    // IP完全一致の場合にtrueを返す
    return std::find(allowed_ips.begin(), allowed_ips.end(), ip) != allowed_ips.end();
}

std::vector<std::string> load_allowed_ips() {
    std::vector<std::string> allowed_ips;
    std::ifstream config_file("config/allowed_ip.json");

    if (!config_file.is_open()) {
        std::cerr << "Failed to open config file" << std::endl;
        return allowed_ips;
    }

    try {
        json config;
        config_file >> config;

        // check if the key exists
        if (config.contains("allowed_ips")) {
            allowed_ips = config["allowed_ips"].get<std::vector<std::string>>();
        } else {
            std::cerr << "No allowed_ips found in config" << std::endl;
        }
    } catch (const nlohmann::json::parse_error &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }

    return allowed_ips;
}