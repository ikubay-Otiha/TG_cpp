#ifndef ROUTE_HPP
#define ROUTE_HPP

#include "crow.h"

void route(crow::SimpleApp &app, const std::vector<std::string> &allowed_ips);

// finction for each endpoint
crow::response handle_root(const crow::request &req,
                           const std::vector<std::string> &allowed_ips);

#endif