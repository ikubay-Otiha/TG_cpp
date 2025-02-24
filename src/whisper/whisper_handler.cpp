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
                std::cout << "=== Processing Whisper Request ===" << std::endl;
                // set env configs
                std::string endpoint = env.at("OPENAPI_WHISPER_URL");
                std::string apiKey = env.at("OPENAI_API_KEY");

                CurlRequest request(endpoint, apiKey);

                request.addFilePart("file", file_name, audio_data);
                request.addTextPart("model", "whisper-1");

                std::string response = request.sendRequest();

                std::cout << "Response size: " << response.size() << " bytes" << std::endl;
                std::cout << "Response content: " << response << std::endl;

                return response;
                // // 開始ログ
                // std::cout << "=== Stage 1: Initialization ===" << std::endl;
                // std::cout << "processWhisper started" << std::endl;
                // std::cout << "Audio data size: " << audio_data.size() << " bytes" << std::endl;
                // std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;

                // // init CURL
                // std::cout << "\n=== Stage 2: CURL Setup ===" << std::endl;
                // CURL *curl = curl_easy_init();
                // if (!curl) {
                //     std::cerr << "curl_easy_init() failed" << std::endl;
                //     throw std::runtime_error("Failed to init CURL");
                // }

                // std::cout << "curl initialized" << std::endl; // curl初期化ログ

                // // RAII for cleanup
                // struct CurlCleanup {
                //     CURL *curl;
                //     curl_mime *mime;
                //     struct curl_slist *headers;

                //     CurlCleanup(CURL *c) : curl(c), mime(nullptr), headers(nullptr) {
                //         if (!curl) {
                //             throw std::runtime_error("Failed to init CURL");
                //         }
                //     }

                //     ~CurlCleanup() {
                //         std::cout << "Starting CURL cleanup..." << std::endl;
                //         cleanup();
                //     }

                //     void cleanup() {
                //         if (mime) {
                //             curl_mime_free(mime);
                //             mime = nullptr;
                //             std::cout << "MIME freed" << std::endl;
                //         }
                //         if (headers) {
                //             curl_slist_free_all(headers);
                //             headers = nullptr;
                //             std::cout << "Headers freed" << std::endl;
                //         }
                //         if (curl) {
                //             curl_easy_cleanup(curl);
                //             curl = nullptr;
                //             std::cout << "CURL cleaned up" << std::endl;
                //         }
                //     }

                //     // コピー禁止
                //     CurlCleanup(const CurlCleanup &) = delete;
                //     CurlCleanup &operator=(const CurlCleanup &) = delete;
                // };

                // std::cout << "\n=== Stage 3: MIME Setup ===" << std::endl;
                // curl_mime *mime = curl_mime_init(curl);

                // // // 詳細ログ出力設定
                // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                // // curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, curl_debug_callback);

                // // ファイルデータ（メモリ上バイナリ）をmultipart/form-dataに追加
                // curl_mimepart *part = curl_mime_addpart(mime);
                // curl_mime_name(part, "file");
                // const char *audio_data_ptr = reinterpret_cast<const char *>(audio_data.data()); // 型変換
                // //
                // if (audio_data.empty()) {
                //     throw std::runtime_error("Audio data is empty. Cannot send empty file.");
                // }
                // //
                // curl_mime_data(part, audio_data_ptr, audio_data.size()); // 74行目
                // curl_mime_filename(part, file_name.c_str());
                // std::cout << "file part added" << std::endl;

                // // Content-Typeヘッダーを手動設定
                // std::cout << "\n=== Stage 4: Headers Setup ===" << std::endl;
                // struct curl_slist *headers = NULL;
                // std::string auth_header = "Authorization: Bearer " + apiKey;
                // headers = curl_slist_append(headers, auth_header.c_str());

                // std::string content_type_header = "Content-Type: multipart/form-data";
                // headers = curl_slist_append(headers, content_type_header.c_str());
                // // cleanup.headers = headers;
                // std::cout << "Headers added" << std::endl;

                // // モデルパート
                // part = curl_mime_addpart(mime);
                // curl_mime_name(part, "model");
                // curl_mime_data(part, "whisper-1", CURL_ZERO_TERMINATED);
                // std::cout << "model part added" << std::endl;

                // std::cout << "\n=== Stage 5: CURL Options Setup ===" << std::endl;
                // std::string response;
                // curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
                // // ヘッダーリストをcurlに設定
                // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                // curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
                // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                // //  [](char *ptr, size_t size, size_t nmemb, std::string *userdata) -> size_t {
                // //      try {
                // //          if (!ptr || !userdata) {
                // //              std::cerr << "Invalid callback parameters" << std::endl;
                // //              return 0;
                // //          }
                // //          size_t total_size = size * nmemb;
                // //          std::cout << "Receiving chunk of size: " << total_size << std::endl;
                // //          userdata->append(ptr, total_size);
                // //          return total_size;
                // //      } catch (const std::exception &e) {
                // //          std::cerr << "Error in write callback: " << e.what() << std::endl;
                // //          return 0;
                // //      }
                // //  });
                // curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                // std::cout << "curl options set" << std::endl;

                // std::cout << "\n=== Stage 6: Performing Request ===" << std::endl;
                // CURLcode res = curl_easy_perform(curl);

                // long http_code = 0;
                // curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

                // // レスポンスを出力
                // std::cout << "HTTP status code: " << http_code << std::endl;

                // if (res != CURLE_OK) {
                //     std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
                //     throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
                // }

                // std::cout << "\n=== Stage 7: Processing Response ===" << std::endl;
                // std::cout << "Response size: " << response.size() << " bytes" << std::endl;
                // std::cout << "Response content: " << response << std::endl;
                // std::cout << "curl_easy_perform finished" << std::endl;

                // // JSON　parse
                // try {
                //     auto json_response = nlohmann::json::parse(response);
                //     std::cout << "JSON parsed successfully" << std::endl;
                // } catch (const nlohmann::json::exception &e) {
                //     std::cerr << "JSON parse error: " << e.what() << std::endl;
                // }

                // return response;

            } catch (const std::exception &e) {
                std::stringstream ss;
                ss << "\n=== ERROR OCCURRED ===" << std::endl;
                ss << "Error: " << e.what() << "\n"
                   << "Stack trace:\n"
                   << boost::stacktrace::stacktrace();
                std::cerr << ss.str() << std::endl;
                throw;
            }
        });
}