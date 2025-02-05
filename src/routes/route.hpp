#ifndef ROUTE_HPP
#define ROUTE_HPP

#include "crow.h"

void route(crow::SimpleApp &app, const std::vector<std::string> &allowed_ips);

#endif