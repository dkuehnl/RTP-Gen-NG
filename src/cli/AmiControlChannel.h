//
// Created by dkueh on 09.07.2026.
//

#ifndef RTPGEN_NG_AMICONTROLCHANNEL_H
#define RTPGEN_NG_AMICONTROLCHANNEL_H
#include <atomic>
#include <map>

#include "IControlChannel.h"

class AmiControlChannel : public IControlChannel {
public:
    explicit AmiControlChannel(std::string host, uint16_t port, std::string user, std::string secret);
    ~AmiControlChannel() override;

    void on_start_stream(StartHandler handler) override { m_start_handler = std::move(handler); };
    void on_end_stream(EndHandler handler) override { m_end_handler = std::move(handler); };
    void on_update_dest(UpdateHandler handler) override { m_update_handler = std::move(handler); };

    void listen() override;
    void stop() override;

private:
    std::string m_host{};
    std::string m_user{}, m_secret{};
    uint16_t m_port{};
    int m_sockfd{-1};
    std::atomic_bool m_active{false};

    StartHandler m_start_handler;
    EndHandler m_end_handler;
    UpdateHandler m_update_handler;

    void connect_and_login() const;
    std::string read_message() const;
    std::map<std::string, std::string> parse_message(const std::string& raw);
    void dispatch(const std::map<std::string, std::string>& fields);
    std::string getvar(const std::string& channel, const std::string& var);
};


#endif //RTPGEN_NG_AMICONTROLCHANNEL_H