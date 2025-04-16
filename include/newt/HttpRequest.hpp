#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <regex>

namespace newt {

enum HttpMethod {
    HTTP_GET,
    HTTP_HEAD,
    HTTP_OPTIONS,
    HTTP_TRACE,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_POST,
    HTTP_PATCH,
    HTTP_CONNECT,
    HTTP_UNKNOWN
};

class HttpRequest {
    bool m_valid;
    std::string m_str;
    HttpMethod m_method;
    std::string m_endpoint;
    std::string m_body;
    std::string m_version;
    std::unordered_map <std::string, std::string> m_header;
    std::regex m_start_regex;
    std::regex m_header_regex;

public:
    HttpRequest() = default;
    HttpRequest(std::string str);
    ~HttpRequest() = default;

    int parse(std::string str);
    bool is_valid() const;
    HttpMethod method() const;
    std::string field(std::string field);
    std::string path() const;
    std::string endpoint() const;
    std::string body() const;
    std::string version() const;
};

}