#include <string>
#include <vector>

bool is_ip_allowed(const std::string &ip, const std::vector<std::string> &allowed_ips);

std::vector<std::string> load_allowed_ips();