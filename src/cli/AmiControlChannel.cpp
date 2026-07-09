//
// Created by dkueh on 09.07.2026.
//

#include "AmiControlChannel.h"

#include <utility>
#include <sys/socket.h>
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <bits/valarray_after.h>


AmiControlChannel::AmiControlChannel(std::string host, const uint16_t port, std::string user, std::string secret)
    : m_host(std::move(host)), m_user(std::move(user)), m_secret(std::move(secret)), m_port(port)
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);

    if (inet_pton(AF_INET, m_host.c_str(), &addr.sin_addr) <= 0) {
        close(m_sockfd);
        throw std::runtime_error("inet_pton() failed: invalid host " + m_host);
    }

    if (connect(m_sockfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(m_sockfd);
        throw std::runtime_error("connect() failed: " + std::string(strerror(errno)));
    }

    connect_and_login();
}

void AmiControlChannel::connect_and_login() const {
    //this is used to skip the Asterisk specific Welcome-Banner
    char banner[265];
    read(m_sockfd, banner, sizeof(banner));

    const std::string login_req =
        "Action: Login\r\n"
        "Username: " + m_user + "\r\n"
        "Secret: " + m_secret + "\r\n"
        "Events: on\r\n"
        "\r\n";

    if (const ssize_t sent = write(m_sockfd, login_req.c_str(), login_req.size()); sent < 0) {
        close(m_sockfd);
        throw std::runtime_error("write() failed: " + std::string(strerror(errno)));
    }

    std::string response = read_message();
    if (response.find("Response: Success") == std::string::npos) {
        close(m_sockfd);
        throw std::runtime_error("AMI login failed: " + response);
    }
}

std::string AmiControlChannel::read_message() const {
    std::string result{};
    char buffer[512];

    while (result.find("\r\n\r\n") == std::string::npos) {
        const ssize_t received = read(m_sockfd, buffer, sizeof(buffer));
        if (received <= 0) {
            throw std::runtime_error("read() failed or connection closed: " + std::string(strerror(errno)));
        }
        buffer[received] = '\0';
        result.append(buffer, received);
    }

    return result;
}

void AmiControlChannel::listen() {
    m_active = true;
    pollfd pfd{m_sockfd, POLLIN, 0};

    while (m_active) {
        int has_event = poll(&pfd, 1, 200);
        if (has_event < 0) {
            if (errno == EINTR) continue;
            std::cerr << "poll() failed: " << strerror(errno) << "\n";
            break;
        }

        if (has_event == 0) continue;

        std::string raw{};
        try {
            raw = read_message();
        } catch (const std::runtime_error& e) {
            std::cerr << "AMI read failed: " << e.what() << "\n";
            break;
        }

        auto fields = parse_message(raw);
        dispatch(fields);
    }
}

std::map<std::string, std::string> AmiControlChannel::parse_message(const std::string& raw) {
    std::map<std::string, std::string> fields;
    std::istringstream stream(raw);
    std::string line{};

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        const auto colons_pos = line.find(':');
        if (colons_pos == std::string::npos) continue;

        std::string key = line.substr(0, colons_pos);
        std::string value = line.substr(colons_pos + 1);

        if (!value.empty() && value.front() == ' ') value.erase(0, 1);

        fields[key] = value;
    }

    return fields;
}

void AmiControlChannel::dispatch(const std::map<std::string, std::string>& fields) {
    auto event_iterator = fields.find("Event");
    if (event_iterator == fields.end()) return;

    const std::string& event = event_iterator->second;

    if (event == "Newstate") {
        if (
            const auto state_iterator = fields.find("ChannelStateDesc");
            state_iterator == fields.end()
            || state_iterator->second != "Up"
            ) return;

        const auto chan_iterator = fields.find("Channel");
        if (chan_iterator == fields.end()) return;

        const std::string dest = getvar(chan_iterator->second, "CHANNEL(rtp,dest)");
        if (dest.empty()) return;

        const auto colon_pos = dest.find(':');
        if (colon_pos == std::string::npos) return;

        StartStreamMsg msg;
        msg.dest_ip = dest.substr(0, colon_pos);
        msg.dest_port = static_cast<uint16_t>(std::stoi(dest.substr(colon_pos + 1)));

        if (m_start_handler) m_start_handler(msg);

    } else if (event == "Hangup" || event == "DialEnd") {
        if (m_end_handler) m_end_handler();
    }
}

std::string AmiControlChannel::getvar(const std::string& channel, const std::string& var) {
    const std::string request =
        "Action: Getvar\r\n"
        "Channel: " + channel + "\r\n"
        "Variable: " + var + "\r\n"
        "\r\n";

    const ssize_t sent = write(m_sockfd, request.c_str(), request.size());
    if (sent < 0) return "";

    std::string raw{};
    try {
        raw = read_message();
    } catch (const std::runtime_error&) {
        return "";
    }

    auto fields = parse_message(raw);
    const auto iterator = fields.find("Value");
    return (iterator != fields.end()) ? iterator->second : "";
}

void AmiControlChannel::stop() {
    m_active = false;
}

AmiControlChannel::~AmiControlChannel() {
    stop();

    const std::string logoff = "Action: Logoff\r\n\r\n";
    write(m_sockfd, logoff.c_str(), logoff.size());
    close(m_sockfd);
}
