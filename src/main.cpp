#include "crow.h"
#include "routes/route.hpp"
#include "utils/utils.hpp"
#include "whisper/whisper_handler.hpp"
#include <boost/stacktrace.hpp>
#include <fstream>
#include <future>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

int main() {
    crow::SimpleApp app;

    // load allowed IPlist
    std::vector<std::string> allowed_ips = load_allowed_ips();

    // load .env file
    std::unordered_map<std::string, std::string> env = load_env();
    std::string config_path = env["CONFIG_PATH"];
    int port_number;

    // load config file
    try {
        nlohmann::json config = load_config_file(config_path);
        port_number = config["port"].get<int>(); // int型で取得
    } catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    // add Whisper API endpoint
    CROW_ROUTE(app, "/transcription")
        .methods("POST"_method)([&env, &allowed_ips](const crow::request &req) {
            // check if the IP is allowed
            if (!is_ip_allowed(req.remote_ip_address, allowed_ips)) {
                return crow::response(403, "Forbidden");
            }

            try {
                // headerのContent-typeを確認
                std::string content_type = req.get_header_value("Content-Type");
                if (content_type.find("multipart/form-data") == std::string::npos) {
                    return crow::response(400, "Invalid content type");
                }

                //  境界文字列を取得
                const std::string boundary_prefix = "boundary=";
                size_t boundary_pos = content_type.find(boundary_prefix);
                if (boundary_pos == std::string::npos) {
                    return crow::response(400, "Invalid multipart boundary");
                }
                // boundary=の後ろに続く文字列を取得する
                std::string boundary = content_type.substr(boundary_pos + boundary_prefix.length());

                // multipart/form-dataから各種パラメータを抽出
                std::string meeting_title = extra_value(req.body, boundary, "title");
                std::cout << "meeting_title: " + meeting_title << std::endl;

                std::string meeting_datetime = extra_value(req.body, boundary, "datetime");
                std::cout << "meeting_datetime: " + meeting_datetime << std::endl;

                if (meeting_title.empty()) {
                    return crow::response(400, "Missing meeting title");
                }

                if (meeting_datetime.empty()) {
                    return crow::response(400, "Missing meeting datetime");
                }

                // multipart/form-dataから音声データを抽出
                std::vector<uint8_t> audio_data = extra_audio_data(req.body, boundary);

                // call WhisperAPI in async process
                std::future<std::string> future_transcription = processWhisper(audio_data, "sample.m4a", env);

                // timeout within 30 seconds
                if (future_transcription.wait_for(std::chrono::seconds(30)) == std::future_status::ready) {
                    std::string transcription = future_transcription.get();

                    nlohmann::json response_json = {
                        {"title", meeting_title},
                        {"datetime", meeting_datetime},
                        {"transcription", transcription}};

                    return crow::response(200, response_json.dump());
                } else {
                    std::cerr << "Timeout: whipser API took too long to respond" << std::endl;
                    return crow::response(504, "Timeout");
                }

            } catch (const std::exception &e) {
                std::stringstream ss;
                ss << "Error: " << e.what() << "\n"
                   << "Stack trace:\n"
                   << boost::stacktrace::stacktrace();
                std::cerr << ss.str() << std::endl;
                return crow::response(500, "Internal Server Error");
            }
        });

    // set routing for all paths
    app.route_dynamic("/")([&allowed_ips](const crow::request &req) {
        return handle_root(req, allowed_ips);
    });

    // set routing
    route(app, allowed_ips);

    // launch server
    // app.loglevel(crow::LogLevel::Debug);
    app.port(port_number).multithreaded().run();

    return 0;
}