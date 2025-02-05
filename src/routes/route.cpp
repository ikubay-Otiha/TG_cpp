#include "route.hpp"
#include "../utils/ip_checker.hpp"
#include "crow.h"

void route(crow::SimpleApp &app, const std::vector<std::string> &allowed_ips) {
    app.route_dynamic("/hello").methods("GET"_method)([&allowed_ips](const crow::request &req) {
        std::string client_ip = req.remote_ip_address;
        crow::response res;

        if (!is_ip_allowed(client_ip, allowed_ips)) {
            res.code = 403;
            res.body = "Forbidden";
            return res;
        }
        return crow::response(200, "Hello World!");
    });
}