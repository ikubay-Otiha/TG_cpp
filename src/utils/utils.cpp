#include "utils.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using json = nlohmann::json;

// IPが許可されているかどうかを判定する
bool is_ip_allowed(const std::string &ip,
                   const std::vector<std::string> &allowed_ips) {
    // IP完全一致の場合にtrueを返す
    return std::find(allowed_ips.begin(), allowed_ips.end(), ip) !=
           allowed_ips.end();
}

// 許可されたIPリストをconfig.jsonから読み込む
std::vector<std::string> load_allowed_ips() {
    std::vector<std::string> allowed_ips;
    std::ifstream config_file("config/allowed_ip.json");

    if (!config_file.is_open()) {
        std::cerr << "Failed to open config file" << std::endl;
        return allowed_ips;
    }

    try {
        json config;
        config_file >> config;

        // check if the key exists
        if (config.contains("allowed_ips")) {
            allowed_ips = config["allowed_ips"].get<std::vector<std::string>>();
        } else {
            std::cerr << "No allowed_ips found in config" << std::endl;
        }
    } catch (const nlohmann::json::parse_error &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }

    return allowed_ips;
}

// リクエストヘッダーからクライアントIPを取得する
std::string get_client_ip(const crow::request& request) {
    // X-Forwarded-For ヘッダーをチェック
    auto forwardedFor = request.get_header_value("X-Forwarded-For");
    std::cout << "forwardedFor" << std::endl;
    std::cout << forwardedFor << std::endl;
    if (!forwardedFor.empty()) {
        // カンマ区切りの場合は最初のIPを使用
        size_t commaPos = forwardedFor.find(',');
        if (commaPos != std::string::npos) {
            return forwardedFor.substr(0, commaPos);
        }
        return forwardedFor;
    }
    
    // X-Real-IP ヘッダーをチェック
    auto realIP = request.get_header_value("X-Real-IP");
    std::cout << "realIP" << std::endl;
    std::cout << realIP << std::endl;
    if (!realIP.empty()) {
        return realIP;
    }
    
    // ヘッダーがない場合は直接のクライアントIPを使用
    return request.remote_ip_address;
}

// .envファイルからAPIキーとURLを読み込む
std::unordered_map<std::string, std::string> load_env() {
    std::unordered_map<std::string, std::string> env;
    std::ifstream env_file(".env");

    if (!env_file.is_open()) {
        std::cerr << "Failed to open .env file" << std::endl;
        return env;
    }

    std::string line; // １行分の文字列を格納する変数
    // .envファイルを1行ずつ読み込み, lineに格納
    while (std::getline(env_file, line)) {
        // =が見つかった場合, =の左側をkey, 右側をvalueとしてenvに格納
        // 読み込んだ行lineの中に'='文字があるかどうかを検索し、その位置をpos変数に格納
        // 見つからなかった場合はstd::string::nposが返る
        size_t pos = line.find("=");
        // =が見つかった場合
        if (pos != std::string::npos) {
            // line文字列の0番目からpos番目までの部分文字列を切り出す
            std::string key = line.substr(0, pos);
            // line文字列のpos+1番目から最後までの部分文字列を切り出す
            std::string value = line.substr(pos + 1);
            // keyとvalueをenvに格納
            env[key] = value;
        }
    }

    return env;
}

// configファイルを読み込む
json load_config_file(const std::string &file_path) {
    std::ifstream config_file(file_path);
    json config;

    if (!config_file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + file_path);
    }

    try {
        config_file >> config;

        // check if the port exists
        if (!config.contains("port")) {
            throw std::runtime_error("No port specified in config file");
        }
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }

    return config;
}

// multipart/form-dataのパラメータを解析する関数
std::string extra_value(const std::string &body, const std::string &boundary, const std::string &field_name) {
    std::string value;

    // フィールドの開始位置を検索
    std::string field_start_str = "Content-Disposition: form-data; name=\"" + field_name + "\"";
    size_t field_start = body.find(field_start_str);
    if (field_start == std::string::npos) {
        return ""; // フィールドが見つからない場合、空文字を返す
    }

    // データの開始位置を検索
    size_t data_start = body.find("\r\n\r\n", field_start) + 4;
    if (data_start == std::string::npos) {
        throw std::runtime_error("Invaild multiple format");
    }

    // 境界文字列を検索
    size_t field_end = body.find("--" + boundary, data_start);
    if (field_end == std::string::npos) {
        throw std::runtime_error("Invalid multipart ending");
    }

    // 値を検出
    value = body.substr(data_start, field_end - data_start - 2);

    return value;
}

// multipart/form-dataの音声データを解析する関数
// multipart/form-dataの音声データを解析する関数（大幅改善版）
std::vector<uint8_t> extra_audio_data(const std::string &body, const std::string &boundary) {
    std::vector<uint8_t> audio_data;

    // --boundaryで始まる各パートを探す
    std::string boundary_str = "--" + boundary;
    size_t pos = 0;
    std::string name_pattern = "name=\"audio\"";
    std::string filename_pattern = "filename=\"";
    bool found_audio_part = false;
    std::string detected_filename;
    std::string content_type;

    while ((pos = body.find(boundary_str, pos)) != std::string::npos) {
        // このパートの開始位置（境界文字列の後）
        size_t part_start = pos + boundary_str.length();
        
        // 次の境界文字列を探す
        size_t next_boundary = body.find(boundary_str, part_start);
        if (next_boundary == std::string::npos) {
            break;  // これ以上パートがない
        }
        
        // このパートのヘッダー部分を取得
        size_t header_end = body.find("\r\n\r\n", part_start);
        if (header_end == std::string::npos || header_end > next_boundary) {
            pos = next_boundary;
            continue;  // ヘッダーがない、または次の境界を超えている
        }
        
        // このパートのヘッダー
        std::string header = body.substr(part_start, header_end - part_start);
        
        // 'name="audio"'が含まれているか確認
        if (header.find(name_pattern) != std::string::npos) {
            found_audio_part = true;
            
            // ファイル名を抽出
            size_t filename_pos = header.find(filename_pattern);
            if (filename_pos != std::string::npos) {
                filename_pos += filename_pattern.length();
                size_t filename_end = header.find("\"", filename_pos);
                if (filename_end != std::string::npos) {
                    detected_filename = header.substr(filename_pos, filename_end - filename_pos);
                    std::cout << "Found filename in request: " << detected_filename << std::endl;
                }
            }
            
            // Content-Typeを抽出
            std::string content_type_pattern = "Content-Type: ";
            size_t content_type_pos = header.find(content_type_pattern);
            if (content_type_pos != std::string::npos) {
                content_type_pos += content_type_pattern.length();
                size_t content_type_end = header.find("\r\n", content_type_pos);
                if (content_type_end != std::string::npos) {
                    content_type = header.substr(content_type_pos, content_type_end - content_type_pos);
                    std::cout << "Content-Type: " << content_type << std::endl;
                }
            }
            
            // データ部分を抽出（ヘッダー終了後からパート終了まで）
            size_t data_start = header_end + 4;  // \r\n\r\n の後
            size_t data_end = next_boundary - 2;  // 次の境界の2バイト前（\r\n を除く）
            
            if (data_start < data_end && data_start < body.size() && data_end <= body.size()) {
                audio_data.assign(body.begin() + data_start, body.begin() + data_end);
                std::cout << "Extracted audio data: " << audio_data.size() << " bytes" << std::endl;
                
                // 最初の16バイトをデバッグ出力
                if (!audio_data.empty()) {
                    std::cout << "First 16 bytes: ";
                    for (size_t i = 0; i < std::min(audio_data.size(), size_t(16)); ++i) {
                        printf("%02x ", audio_data[i]);
                    }
                    std::cout << std::endl;
                }
                
                break;  // audioパートを見つけたので終了
            } else {
                std::cerr << "Invalid data range: start=" << data_start << ", end=" << data_end 
                          << ", body size=" << body.size() << std::endl;
            }
        }
        
        // 次のパートへ
        pos = next_boundary;
    }
    
    if (!found_audio_part) {
        std::cerr << "Audio part not found in request body" << std::endl;
    } else if (audio_data.empty()) {
        std::cerr << "Found audio part, but data is empty" << std::endl;
    } else if (audio_data.size() < 1000) {
        std::cerr << "Warning: Audio data is suspiciously small (" 
                  << audio_data.size() << " bytes)" << std::endl;
    }
    
    return audio_data;
}