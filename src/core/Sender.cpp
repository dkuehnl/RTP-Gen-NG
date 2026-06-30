//
// Created by dkueh on 27.06.2026.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <iostream>

#include "Sender.h"

Sender::Sender(uint16_t src_port) : m_src_port(src_port) {
    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sockfd < 0) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }

    sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(src_port);

    if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&local), sizeof(local)) < 0) {
        throw std::runtime_error("Failed to bind socket: " + std::string(strerror(errno)));
    }

}

void Sender::send(const std::vector<uint8_t>& packet, const StreamState& state) {
    if (state.current_src_port != m_src_port) {
        rebind_if_needed(state);
    }

    sockaddr_in dest{};

    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(state.current_dest_ip.c_str());
    dest.sin_port = htons(state.current_dest_port);

    auto bytes_send = sendto(m_sockfd, packet.data(), packet.size(), 0, reinterpret_cast<sockaddr*>(&dest), sizeof(dest));
    if (bytes_send < 0) {
        std::cerr << "Error while Sending packet: " << strerror(errno) << "\n";
    }
}

void Sender::rebind_if_needed(const StreamState& state) {
    int new_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (new_sockfd < 0) {
        std::cerr << "Rebind failed (socket creation): " << strerror(errno) << "\n";
        return;
    }

    sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(state.current_src_port);

    if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&local), sizeof(local)) < 0) {
        close(new_sockfd);
        std::cerr << "Rebind failed (bind to port: " << state.current_src_port << "): " << strerror(errno) << "\n";
        return;
    }

    close(m_sockfd);
    m_sockfd = new_sockfd;
    m_src_port = state.current_src_port;
}

Sender::~Sender() {
    close(m_sockfd);
}