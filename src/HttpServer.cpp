#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "newt/HttpServer.hpp"
#include "newt/HttpRequest.hpp"

namespace newt {

void http_recv(Connection* conn, void* udata) {
    char* buf = (char*)conn->data();

    HttpRequest req;

    req.parse(std::string(buf));

    std::string path, response;

    if (req.endpoint() == "/") {
        path = "content/index.html";
    } else {
        path = "content" + req.endpoint();
    }

    std::ifstream file(path, std::ios::binary);

    printf("path: %s\n", path.c_str());

    if (file.is_open()) {
        std::stringstream ss;
        std::string accept = req.field("Accept");

        ss << file.rdbuf();

        file.seekg(0, std::ios::end);
        unsigned int size = file.tellg();

        file.close();

        response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: ";

        response += accept.substr(0, accept.find(','));
        response +=
            "\nServer: Newt 0.1 (Linux)\r\n";

        response +=
            "Content-Length: ";
        response += std::to_string(size) + "\r\n";
        response += "\r\n";

        response += ss.str();
    } else {
        std::string accept = req.field("Accept");

        response = 
            "HTTP/1.1 404 Not found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 90\r\n"
            "Server: Newt 0.1 (Linux)\r\n";
            "\r\n"
            "<html>\n"
            "  <title>404 Not found</title>\n"
            "  <body>\n"
            "    <p>404 Not found</p>\n"
            "  </body>\n"
            "</html>\n";
    }

    printf("response=%s\n", response.c_str());

    conn->send((void*)response.c_str(), response.size());
    // conn->end();
}

int HttpServer::open(std::string host) {
    size_t p = host.find_first_of(':');

    std::string name = host.substr(0, p);
    std::string port = host.substr(p+1);

    return Server::open(name, stoul(port), 1, http_recv, this);
}

HttpServer::HttpServer(std::string host) {
    open(host);
}

}