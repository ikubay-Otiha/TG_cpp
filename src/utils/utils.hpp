#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

bool is_ip_allowed(const std::string &ip,
                   const std::vector<std::string> &allowed_ips);

std::vector<std::string> load_allowed_ips();

std::unordered_map<std::string, std::string> load_env();

nlohmann::json load_config_file(const std::string &file_path);