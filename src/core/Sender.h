//
// Created by dkueh on 27.06.2026.
//

#ifndef RTPGEN_NG_SENDER_H
#define RTPGEN_NG_SENDER_H
#include "ScenarioEngine.h"


class Sender {
public:
    explicit Sender(uint16_t src_port = 0);
    ~Sender();
    void send(const std::vector<uint8_t>& packet, const StreamState& state);

private:
    int m_sockfd{-1};
    uint16_t m_src_port{};

    void rebind_if_needed(const StreamState& state);
};

#endif //RTPGEN_NG_SENDER_H