#include "curl_request.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cctype> 

CurlRequest::CurlRequest(const std::string &url, const std::string &apiKey)
    : curl(curl_easy_init()), mime(nullptr), headers(nullptr) {

    if (!curl) {
        throw std::runtime_error("Failed to init CURL");
    }

    mime = curl_mime_init(curl);

    // APIキーの確認
    if (!apiKey.empty()) {
        std::string authHeader = "Authorization: Bearer " + apiKey;
        headers = curl_slist_append(headers, authHeader.c_str());
    } else {
        throw std::runtime_error("APIKEY is not exist");
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // デバッグ出力追加
}

CurlRequest::~CurlRequest() {
    if (mime)
        curl_mime_free(mime);
    if (headers)
        curl_slist_free_all(headers);
    if (curl)
        curl_easy_cleanup(curl);
}

void CurlRequest::addFilePart(
    const std::string &fieldName,
    const std::string &fileName,
    const std::vector<unsigned char> &fileData) {
    if (fileData.empty()) {
        throw std::runtime_error("File data is empty. Cannot send empty file.");
    }
    
    std::cout << "Adding file part: " << fieldName << ", filename: " << fileName
              << ", size: " << fileData.size() << " bytes" << std::endl;
    
    // ファイル拡張子からMIMEタイプを決定
    std::string mimeType = "application/octet-stream";  // デフォルト
    
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string ext = fileName.substr(dotPos);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".mp3") mimeType = "audio/mpeg";
        else if (ext == ".m4a") mimeType = "audio/mp4";
        else if (ext == ".wav") mimeType = "audio/wav";
        else if (ext == ".flac") mimeType = "audio/flac";
        else if (ext == ".ogg" || ext == ".oga") mimeType = "audio/ogg";
        else if (ext == ".webm") mimeType = "audio/webm";
        else if (ext == ".mp4") mimeType = "audio/mp4";
        else if (ext == ".mpeg" || ext == ".mpga") mimeType = "audio/mpeg";
    }
    
    std::cout << "Using MIME type: " << mimeType << " for file: " << fileName << std::endl;
    
    // ファイルヘッダーの最初の数バイトをデバッグ表示
    if (fileData.size() >= 16) {
        std::cout << "First 16 bytes: ";
        for (size_t i = 0; i < 16; ++i) {
            printf("%02x ", fileData[i]);
        }
        std::cout << std::endl;
    }
    
    curl_mimepart *part = curl_mime_addpart(mime);
    curl_mime_name(part, fieldName.c_str());
    curl_mime_data(part, reinterpret_cast<const char *>(fileData.data()), fileData.size());
    curl_mime_filename(part, fileName.c_str());
    curl_mime_type(part, mimeType.c_str());
    
    std::cout << "File part added successfully" << std::endl;
}

void CurlRequest::addTextPart(
    const std::string &fieldName,
    const std::string &value) {

    curl_mimepart *part = curl_mime_addpart(mime);
    curl_mime_name(part, fieldName.c_str());
    curl_mime_data(part, value.c_str(), CURL_ZERO_TERMINATED);
}

std::string CurlRequest::sendRequest() {
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
    }
    return response;
}

size_t CurlRequest::WriteCallback(
    char *ptr,
    size_t size,
    size_t nmemb,
    void *userdata) {

    if (!ptr || !userdata) {
        return 0;
    }
    auto *response = static_cast<std::string *>(userdata);
    try {
        size_t total_size = size * nmemb;
        response->append(ptr, total_size);
        return total_size;
    } catch (const std::exception &e) {
        std::cerr << "Error in write callback: " << e.what() << std::endl;
        return 0;
    }
}