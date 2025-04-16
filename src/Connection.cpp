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

#include "newt/Connection.hpp"

namespace newt {

void Connection::listen_thread_func(Connection* conn) {
    printf("server: Connection thread started\n");

    struct timeval timeout;
    fd_set set;

    while (conn->m_listening) {
        // Initialize the file descriptor set.
        FD_ZERO(&set);
        FD_SET(conn->m_fd, &set);

        // Set a 500ms timeout
        timeout.tv_sec = 0;
        timeout.tv_usec = 500;

        /* select returns 0 if timeout, 1 if input available, -1 if error. */
        int s = select(FD_SETSIZE, &set, NULL, NULL, &timeout);

        if (s == 0) {
            // Yield every ~500ms
            std::this_thread::yield();
        } else if (s == 1) {
            printf("connection: Data received\n");

            conn->m_size = recv(conn->m_fd, conn->m_buf, CONN_BUFSIZE, 0);

            if (conn->m_recv_cb)
                conn->m_recv_cb(conn, conn->m_udata);
        } else {
            printf("connection: Error! select returned %d, ending connection\n", s);

            conn->end();
        }
    }

    printf("connection: Listener thread shutting down!\n");
}

void Connection::start(int fd) {
    m_fd = fd;

    m_listening = true;
    m_listen_thread = std::thread(listen_thread_func, this);
}

bool Connection::is_alive() const {
    return m_listening;
}

void Connection::end() {
    m_listening = false;

    // printf("joinable=%d\n", m_listen_thread.joinable());

    // if (m_listen_thread.joinable())
    //     m_listen_thread.join();

    // printf("joinable=%d\n", m_listen_thread.joinable());

    shutdown(m_fd, SHUT_RDWR);
}

int Connection::send(void* data, size_t size) {
    struct timeval timeout;
    fd_set set;

    FD_ZERO(&set);
    FD_SET(m_fd, &set);

    // Set a 10ms timeout
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    /* select returns 0 if timeout, 1 if input available, -1 if error. */
    int s = select(FD_SETSIZE, NULL, &set, NULL, &timeout);

    if (s == 0) {
        printf("connection: Client timed out\n");
    } else if (s == 1) {
        printf("connection: Sending response to client...\n");

        int r = ::send(m_fd, data, size, 0);

        printf("connection: Sent %d bytes\n", r);

        // if (::send(m_fd, data, size, 0) <= 0) {
        //     printf("connection: Couldn't send response\n");
        // } else {
        //     printf("connection: Done\n");
        // }
    } else {
        printf("connection: Error! select returned %d, ending connection\n", s);

        end();
    }

    return s;
}

const void* Connection::data(size_t* size) const {
    if (size) *size = m_size;

    return m_buf;
}

int Connection::fd() const {
    return m_fd;
}

void Connection::set_recv_callback(ConnectionRecvCallback recv_cb, void* udata) {
    m_recv_cb = recv_cb;
    m_udata = udata;
}

Connection::Connection(int fd, ConnectionRecvCallback recv_cb, void* udata) {
    set_recv_callback(recv_cb, udata);
    start(fd);
}

Connection::~Connection() {
    if (m_listening) end();
}

}
