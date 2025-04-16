#include <csignal>

#include "newt/HttpServer.hpp"

newt::HttpServer* g_server;

void handler(int s) {
    printf("ctrlc: Closing server\n");

    g_server->close();

    exit(0);
}

int main() {
    newt::HttpServer server;

    g_server = &server;

    signal(SIGINT, handler);

    while (server.open("localhost:1234")) {
        printf("main: Failed to open server, retrying in 1s...\n");

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    printf("main: Server started! main thread sleeping now\n");

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    server.close();
}