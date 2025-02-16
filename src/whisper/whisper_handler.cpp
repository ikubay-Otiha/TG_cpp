#include <boost/stacktrace.hpp>
#include <crow.h>
#include <curl/curl.h>
#include <future>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

std::future<std::string> processWhisper(const std::vector<uint8_t> &audio_data,
                                        const std::string &file_name,
                                        const std::unordered_map<std::string, std::string> &env) {
    return std::async(std::launch::async, [&audio_data, file_name, &env]() -> std::string {
        try {
            std::cout << "processWhisper started" << std::endl; // 開始ログ

            // set env configs
            std::string endpoint = env.at("OPENAPI_WHISPER_URL");
            std::string apiKey = env.at("OPENAI_API_KEY");

            std::cout << "Endpoint: " << endpoint << std::endl;
            std::cout << "apiKey: " << apiKey << std::endl;

            // init CURL
            CURL *curl = curl_easy_init();
            if (!curl) {
                std::cerr << "curl_easy_init() failed" << std::endl;
                throw std::runtime_error("Failed to init CURL");
            }

            std::cout << "curl initialized" << std::endl; // curl初期化ログ

            // 詳細ログ出力設定
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            curl_mime *mime = curl_mime_init(curl);
            curl_mimepart *part;

            std::cout << "file part added" << std::endl; // mime初期化ログ

            // ファイルデータ（メモリ上バイナリ）をmultipart/form-dataに追加
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "file");
            const char *audio_data_ptr = reinterpret_cast<const char *>(audio_data.data()); // 型変換
            curl_mime_data(part, audio_data_ptr, audio_data.size());
            curl_mime_filename(part, file_name.c_str());

            std::cout << "file part added" << std::endl;

            // Content-Typeヘッダーを手動設定
            struct curl_slist *headers = NULL;
            std::string auth_header = "Authorization: Bearer " + apiKey;
            headers = curl_slist_append(headers, auth_header.c_str());

            std::string content_type_header = "Content-Type: multipart/form-data";
            headers = curl_slist_append(headers, content_type_header.c_str());

            std::cout << "contetn type header added" << std::endl;

            // モデルパート
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "model");
            curl_mime_data(part, "whisper-1", CURL_ZERO_TERMINATED);

            std::cout << "model part added" << std::endl;

            std::string response;
            curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
            // ヘッダーリストをcurlに設定
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](char *ptr, size_t size, size_t nmemb, std::string *userdata) -> size_t {
                userdata->append(ptr, size * nmemb);
                return size * nmemb;
            });
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            std::cout << "curl options set" << std::endl;

            CURLcode res = curl_easy_perform(curl);

            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

            // レスポンスを出力
            std::cout << "HTTP status code: " << http_code << std::endl;
            std::cout << "Response: " << response << std::endl;

            std::cout << "curl_easy_perform finished" << std::endl;

            curl_mime_free(mime);
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);

            std::cout << "curl cleanup finished" << std::endl;

            if (res != CURLE_OK) {
                std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
                throw std::runtime_error("CRUL request failed: " + std::string(curl_easy_strerror(res)));
            }

            std::cout << "response received" << std::endl;

            return response;
        } catch (const std::exception &e) {
            std::stringstream ss;
            ss << "Error: " << e.what() << "\n"
               << "Stack trace:\n"
               << boost::stacktrace::stacktrace();
            std::cerr << ss.str() << std::endl;

            throw std::runtime_error("Exception: " + std::string(e.what()));
        }
    });
}