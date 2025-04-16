#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <ctype.h>

#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <list>

#include "newt/Server.hpp"

namespace newt {

void Server::listen_thread_func(Server* server) {
    printf("server: Listening thread started\n");

    struct timeval timeout;
    fd_set set;

    while (server->m_listening) {
        // Initialize the file descriptor set.
        FD_ZERO(&set);
        FD_SET(server->m_sock, &set);

        // Set a 500ms timeout
        timeout.tv_sec = 0;
        timeout.tv_usec = 500;

        /* select returns 0 if timeout, 1 if input available, -1 if error. */
        int s = select(FD_SETSIZE, &set, NULL, NULL, &timeout);

        if (s == 0) {
            // Yield every ~500ms
            std::this_thread::yield();
        } else if (s == 1) {
            printf("server: Received connection\n");

            int fd = accept(server->m_sock, NULL, NULL);

            if (fd < 0) {
                printf("server: Couldn't accept connection\n");

                continue;
            }

            server->m_connections.push_back(new Connection(
                fd, server->m_recv_cb, server->m_udata
            ));
        } else {
            printf("server: Error! select returned %d, closing all connections\n", s);

            server->close();
        }
    }
}

int Server::open(std::string host, int port, int max_connections, ConnectionRecvCallback recv_cb, void* udata) {
    m_recv_cb = recv_cb;
    m_udata = udata;

    m_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Return socket open error when getting a negative file descriptor
    if (m_sock < 0) {
        return NET_ERR_SOCK;
    }

    // Initialize a socket address struct with the host:port data 
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    // Get a usable in_addr struct from the provided hostname
    struct hostent* info = gethostbyname(host.c_str());

    if (!info) {
        return NET_ERR_HOST;
    }

    addr.sin_addr = *(struct in_addr*)info->h_addr;

    // Try to bind the address to our socket
    if (bind(m_sock, (struct sockaddr*)&addr, sizeof(addr))) {
        return NET_ERR_BIND;
    }

    // Try to start listening for connections
    if (listen(m_sock, max_connections)) {
        return NET_ERR_LISTEN;
    }

    // Start listening thread
    m_listen_thread = std::thread(listen_thread_func, this);

    return NET_OK;
}

void Server::close() {
    // End all connections
    for (Connection* conn : m_connections) {
        conn->end();
    }

    m_listening = false;

    if (m_listen_thread.joinable())
        m_listen_thread.join();

    shutdown(m_sock, SHUT_RDWR);
}

Server::Server(std::string host, int port, int max_connections, ConnectionRecvCallback recv_cb, void* udata) : m_host(host), m_port(port) {
    open(host, port, max_connections, recv_cb, udata);
}

Server::~Server() {
    if (m_listening) close();
}

}