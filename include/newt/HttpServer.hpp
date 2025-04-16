#pragma once

#include <unordered_map>

#include "Server.hpp"

namespace newt {

class HttpServer : public Server {
    std::unordered_map <std::string, bool> m_endpoints;

public:
    int open(std::string host);

    HttpServer() = default;
    HttpServer(std::string host);
};

}