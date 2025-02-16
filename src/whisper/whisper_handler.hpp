#include <future>
#include <string>
#include <unordered_map>
#include <vector>

std::future<std::string> processWhisper(const std::vector<uint8_t> &audio_data,
                                        const std::string &file_name,
                                        const std::unordered_map<std::string, std::string> &env);