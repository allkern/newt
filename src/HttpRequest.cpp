#include <algorithm>
#include <sstream>

#include "newt/HttpRequest.hpp"

namespace newt {

HttpMethod get_method(std::string str) {
    if (str == "GET") return HTTP_GET;
    else if (str == "HEAD") return HTTP_HEAD;
    else if (str == "OPTIONS") return HTTP_OPTIONS;
    else if (str == "TRACE") return HTTP_TRACE;
    else if (str == "PUT") return HTTP_PUT;
    else if (str == "DELETE") return HTTP_DELETE;
    else if (str == "POST") return HTTP_POST;
    else if (str == "PATCH") return HTTP_PATCH;
    else if (str == "CONNECT") return HTTP_CONNECT;

    return HTTP_UNKNOWN;
}

std::string get_line(std::istringstream& ss) {
    std::string str;

    std::getline(ss, str);

    return str;
}

std::string trim(const std::string& source) {
    std::string s(source);

    s.erase(0,s.find_first_not_of(" \n\r\t"));
    s.erase(s.find_last_not_of(" \n\r\t")+1);

    return s;
}

HttpRequest::HttpRequest(std::string str) : m_str(str) {
    parse(str);
}

int HttpRequest::parse(std::string str) {
    std::istringstream ss(str);

    std::string start = get_line(ss);
    std::string kv = get_line(ss);

    while (kv.size()) {
        std::string key = trim(kv.substr(0, kv.find_first_of(':')));
        std::string value = trim(kv.substr(kv.find_first_of(':') + 1));

        m_header.insert({ key, value });

        kv = get_line(ss);
    }

    // Whatever's left is our request body
    char c = ss.get();

    while (c && !ss.eof()) {
        m_body.push_back(c);

        c = ss.get();
    }

    // Parse start line (<method> <endpoint> <version>)
    const char* p = start.c_str();

    std::string method;

    while (!isspace(*p))
        method.push_back(*p++);

    m_method = get_method(method);

    while (isspace(*p)) ++p;

    while (!isspace(*p))
        m_endpoint.push_back(*p++);

    while (isspace(*p)) ++p;

    while (*p)
        m_version.push_back(*p++);

    m_version = trim(m_version);

    m_valid = true;

    return 0;
}

bool HttpRequest::is_valid() const {
    return m_valid;
}

HttpMethod HttpRequest::method() const {
    return m_method;
}

std::string HttpRequest::field(std::string field) {
    return m_header[field];
}

std::string HttpRequest::path() const {
    return m_endpoint;
}

std::string HttpRequest::endpoint() const {
    return m_endpoint;
}

std::string HttpRequest::body() const {
    return m_body;
}

std::string HttpRequest::version() const {
    return m_version;
}

}