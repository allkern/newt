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

#define CONN_BUFSIZE 0x10000

namespace newt {

class Connection;

typedef void (*ConnectionRecvCallback)(Connection*, void*);

class Connection {
    unsigned char m_buf[CONN_BUFSIZE];
    size_t m_size;
    int m_fd;

    std::atomic <bool> m_listening;
    std::thread m_listen_thread;
    ConnectionRecvCallback m_recv_cb;
    void* m_udata;

    static void listen_thread_func(Connection* conn);

public:
    void start(int fd);
    bool is_alive() const;
    void end();
    int send(void* data, size_t size);
    const void* data(size_t* size = nullptr) const;
    int fd() const;
    void set_recv_callback(ConnectionRecvCallback recv_cb, void* udata = nullptr);

    Connection() = default;
    Connection(int fd, ConnectionRecvCallback recv_cb, void* udata = nullptr);
    ~Connection();
};

}
