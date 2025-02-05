#include "crow.h"
#include "routes/route.hpp"
#include "utils/ip_checker.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    std::cout << "Current path: " << std::filesystem::current_path() << std::endl;

    crow::SimpleApp app;

    // load allowed IPlist
    std::vector<std::string> allowed_ips = load_allowed_ips();

    // load config file
    std::ifstream config_file("config/allowed_ip.json");
    json config;

    try {
        config_file >> config;

        // check if the port exists
        if (!config.contains("port")) {
            std::cerr << "No port specified in config" << std::endl;
            return -1;
        }
    } catch (const nlohmann::json::parse_error &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return -1;
    }

    app.route_dynamic("/")([&allowed_ips](const crow::request &req) {
        std::string client_ip = req.remote_ip_address;
        crow::response res;

        if (!is_ip_allowed(client_ip, allowed_ips)) {
            res.code = 403;
            res.body = "Forbidden";
            return res;
        }

        res.code = 200;
        res.body = "Access granted!";
        return res;
    });

    // set routing
    route(app, allowed_ips);

    // launch server
    int port = config["port"].get<int>();
    app.port(port).multithreaded().run();

    return 0;
}