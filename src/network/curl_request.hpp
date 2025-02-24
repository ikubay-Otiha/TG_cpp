#ifndef CURL_REQUEST_HPP
#define CURL_REQUEST_HPP

#include <curl/curl.h>
#include <string>
#include <unordered_map>
#include <vector>

class CurlRequest {
public:
    CurlRequest(const std::string &url, const std::string &apiKey = "");
    ~CurlRequest();

    void addFilePart(
        const std::string &fieldName,
        const std::string &fileName,
        const std::vector<unsigned char> &fileData);

    void addTextPart(
        const std::string &fieldName,
        const std::string &value);

    std::string sendRequest();

private:
    CURL *curl;
    curl_mime *mime;
    struct curl_slist *headers;
    std::string response;

    static size_t WriteCallback(
        char *ptr,
        size_t size,
        size_t nmenb,
        void *userdata);
};

#endif