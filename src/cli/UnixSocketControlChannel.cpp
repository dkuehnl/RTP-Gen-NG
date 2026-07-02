//
// Created by dkueh on 02.07.2026.
//

#include "UnixSocketControlChannel.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <iostream>

UnixSocketControlChannel::UnixSocketControlChannel(std::string socket_path)
    : m_socket_path(std::move(socket_path)) {
    m_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_sockfd < 0) throw std::runtime_error("socker() failed: " + std::string(strerror(errno)));

    unlink(m_socket_path.c_str());

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, m_socket_path.c_str(), sizeof(addr.sun_path) - 1);

    if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(m_sockfd);
        throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
    }

    if (::listen(m_sockfd, 1) < 0) {
        close(m_sockfd);
        throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));
    }
}

UnixSocketControlChannel::~UnixSocketControlChannel() {
    stop();
    close(m_sockfd);
    unlink(m_socket_path.c_str());
}

void UnixSocketControlChannel::listen() {
    m_active = true;
    pollfd pfd{m_sockfd, POLLIN, 0};

    while (m_active) {
        int ret = poll(&pfd, 1, 200);
        if (ret < 0) {
            if (errno == EINTR) continue;
            std::cerr << "poll() failed: " << strerror(errno) << "\n";
            break;
        }
        if (ret == 0) continue;

        int client_fd = accept(m_sockfd, nullptr, nullptr);
        if (client_fd < 0) {
            if (!m_active) break;
            std::cerr << "accept() failed: " << strerror(errno) << "\n";
            continue;
        }

        char buffer[512];
        ssize_t received_bytes = read(client_fd, buffer, sizeof(buffer) - 1);
        if (received_bytes > 0) {
            buffer[received_bytes] = '\0';
            dispatch(std::string(buffer));
        }
        close(client_fd);
    }
}

void UnixSocketControlChannel::stop() {
    m_active = false;
}

void UnixSocketControlChannel::dispatch(const std::string& raw_message) {
    if (raw_message.rfind("start_stream", 0) == 0 && m_start_handler) {
        m_start_handler(StartStreamMsg{});
    } else if (raw_message.rfind("stop_stream", 0) == 0 && m_end_handler) {
        std::cout << "stop_stream received" << std::endl;
        m_end_handler();
    } else if (raw_message.rfind("update_stream", 0) == 0 && m_update_handler) {

    }
}
