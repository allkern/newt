#pragma once

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

#include "Connection.hpp"

namespace newt {

enum {
    NET_OK = 0,
    NET_ERR_SOCK,
    NET_ERR_HOST,
    NET_ERR_BIND,
    NET_ERR_LISTEN,
    NET_ERR_ACCEPT
};

class Server {
    std::string m_host;
    int m_port;
    int m_sock;

    std::list <Connection*> m_connections;
    std::atomic <bool> m_listening;
    std::thread m_listen_thread;
    ConnectionRecvCallback m_recv_cb;
    void* m_udata;

    static void listen_thread_func(Server* server);

public:
    int open(std::string host, int port, int max_connections = 1, ConnectionRecvCallback recv_cb = nullptr, void* udata = nullptr);
    void close();

    Server() = default;
    Server(std::string host, int port, int max_connections = 1, ConnectionRecvCallback recv_cb = nullptr, void* udata = nullptr);

    ~Server();
};

}