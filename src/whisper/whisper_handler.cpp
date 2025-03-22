#include "../network/curl_request.hpp"
#include <boost/stacktrace.hpp>
#include <crow.h>
#include <curl/curl.h>
#include <future>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

std::future<std::string> processWhisper(const std::vector<uint8_t> audio_data,
    const std::string file_name,
    const std::unordered_map<std::string, std::string> env) {
return std::async(
std::launch::async,
[audio_data = std::move(audio_data),
file_name = std::move(file_name),
env = std::move(env)]() -> std::string {
try {
// データサイズの確認
if (audio_data.empty()) {
std::cerr << "Error: Empty audio data" << std::endl;
return "{\"error\": {\"message\": \"Empty audio data received\"}}";
}

if (audio_data.size() < 1000) {
std::cerr << "Warning: Audio data is suspiciously small: " << audio_data.size() << " bytes" << std::endl;
}

std::cout << "=== Processing Whisper Request ===" << std::endl;
std::cout << "Audio data size: " << audio_data.size() << " bytes" << std::endl;

// ファイル名の拡張子を確認
std::string actual_file_name = file_name;
if (file_name.find('.') == std::string::npos) {
// 拡張子がない場合、.m4aを追加
actual_file_name += ".m4a";
std::cout << "Added .m4a extension to filename: " << actual_file_name << std::endl;
}

// ファイルの最初の数バイトをデバッグ表示
std::cout << "First 16 bytes of audio data: ";
for (size_t i = 0; i < std::min(audio_data.size(), size_t(16)); ++i) {
printf("%02x ", audio_data[i]);
}
std::cout << std::endl;

// 一時ファイルにデータを保存して確認（オプション）
std::string temp_filename = "whisper_debug_" + actual_file_name;
std::ofstream temp_file(temp_filename, std::ios::binary);
if (temp_file) {
temp_file.write(reinterpret_cast<const char*>(audio_data.data()), audio_data.size());
temp_file.close();
std::cout << "Saved debug file to " << temp_filename << std::endl;
}

// set env configs
std::string endpoint = env.at("OPENAPI_WHISPER_URL");
std::string apiKey = env.at("OPENAI_API_KEY");

CurlRequest request(endpoint, apiKey);
request.addFilePart("file", actual_file_name, audio_data);
request.addTextPart("model", "whisper-1");

std::string response = request.sendRequest();
std::cout << "Response size: " << response.size() << " bytes" << std::endl;
std::cout << "Response content: " << response << std::endl;

// 一時ファイルを削除（オプション）
if (std::remove(temp_filename.c_str()) == 0) {
std::cout << "Removed debug file: " << temp_filename << std::endl;
}

return response;
} catch (const std::exception &e) {
std::stringstream ss;
ss << "\n=== ERROR OCCURRED ===" << std::endl;
ss << "Error: " << e.what() << "\n"
<< "Stack trace:\n"
<< boost::stacktrace::stacktrace();
std::cerr << ss.str() << std::endl;

// エラーメッセージをJSON形式で返す
return "{\"error\": {\"message\": \"" + std::string(e.what()) + "\"}}";
}
});
}