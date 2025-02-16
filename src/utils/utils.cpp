#include "utils.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using json = nlohmann::json;

// IPが許可されているかどうかを判定する
bool is_ip_allowed(const std::string &ip,
                   const std::vector<std::string> &allowed_ips) {
    // IP完全一致の場合にtrueを返す
    return std::find(allowed_ips.begin(), allowed_ips.end(), ip) !=
           allowed_ips.end();
}

// 許可されたIPリストをconfig.jsonから読み込む
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

// .envファイルからAPIキーとURLを読み込む
std::unordered_map<std::string, std::string> load_env() {
    std::unordered_map<std::string, std::string> env;
    std::ifstream env_file(".env");

    if (!env_file.is_open()) {
        std::cerr << "Failed to open .env file" << std::endl;
        return env;
    }

    std::string line; // １行分の文字列を格納する変数
    // .envファイルを1行ずつ読み込み, lineに格納
    while (std::getline(env_file, line)) {
        // =が見つかった場合, =の左側をkey, 右側をvalueとしてenvに格納
        // 読み込んだ行lineの中に'='文字があるかどうかを検索し、その位置をpos変数に格納
        // 見つからなかった場合はstd::string::nposが返る
        size_t pos = line.find("=");
        // =が見つかった場合
        if (pos != std::string::npos) {
            // line文字列の0番目からpos番目までの部分文字列を切り出す
            std::string key = line.substr(0, pos);
            // line文字列のpos+1番目から最後までの部分文字列を切り出す
            std::string value = line.substr(pos + 1);
            // keyとvalueをenvに格納
            env[key] = value;
        }
    }

    return env;
}

// configファイルを読み込む
json load_config_file(const std::string &file_path) {
    std::ifstream config_file(file_path);
    json config;

    if (!config_file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + file_path);
    }

    try {
        config_file >> config;

        // check if the port exists
        if (!config.contains("port")) {
            throw std::runtime_error("No port specified in config file");
        }
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }

    return config;
}