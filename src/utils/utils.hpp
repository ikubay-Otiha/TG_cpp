#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include "crow.h"

bool is_ip_allowed(const std::string &ip,
                   const std::vector<std::string> &allowed_ips);

std::string get_client_ip(const crow::request& request);

std::vector<std::string> load_allowed_ips();

std::unordered_map<std::string, std::string> load_env();

nlohmann::json load_config_file(const std::string &file_path);

std::string extra_value(
    const std::string &body,
    const std::string &boundary,
    const std::string &field_name);

std::vector<uint8_t> extra_audio_data(
    const std::string &body,
    const std::string &boundary);