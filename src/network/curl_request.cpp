#include "curl_request.hpp"
#include <iostream>
#include <stdexcept>

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

    curl_mimepart *part = curl_mime_addpart(mime);
    curl_mime_name(part, fieldName.c_str());
    curl_mime_data(part, reinterpret_cast<const char *>(fileData.data()), fileData.size());

    // ファイル名をセット
    curl_mime_filename(part, fileName.c_str());

    // MIMEタイプをセット
    curl_mime_type(part, "audio/m4a");
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