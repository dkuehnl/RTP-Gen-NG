//
// Created by dkueh on 27.06.2026.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Sender.h"

Sender::Sender(uint16_t src_port) : m_src_port(src_port) {
    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(src_port);

    bind(m_sockfd, reinterpret_cast<sockaddr*>(&local), sizeof(local));
}

void Sender::send(const std::vector<uint8_t>& packet, const StreamState& state) {
    if (state.current_src_port != m_src_port) {
        rebind_if_needed(state);
    }

    sockaddr_in dest{};

    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(state.current_dest_ip.c_str());
    dest.sin_port = htons(state.current_dest_port);

    sendto(m_sockfd, packet.data(), packet.size(), 0, reinterpret_cast<sockaddr*>(&dest), sizeof(dest));
}

void Sender::rebind_if_needed(const StreamState& state) {
    close(m_sockfd);

    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    if (state.use_new_random_src_port) {
        local.sin_port = 0;
    } else {
        local.sin_port = htons(state.current_src_port);
    }

    bind(m_sockfd, reinterpret_cast<sockaddr*>(&local), sizeof(local));
    m_src_port = state.current_src_port;
}
