//
// Created by dkueh on 27.06.2026.
//

#ifndef RTPGEN_NG_SCHEDULER_H
#define RTPGEN_NG_SCHEDULER_H

#include <atomic>

#include "ScenarioEngine.h"
#include "Sender.h"

class Scheduler {
public:
    explicit Scheduler(ScenarioEngine& engine, const Sender& sender, uint64_t start_interval);
    void start_stream();
    void end_stream();

private:
    ScenarioEngine m_engine;
    Sender m_sender;

    std::atomic_bool m_active{false};
    uint64_t m_ptime_btw_packets{};
};


#endif //RTPGEN_NG_SCHEDULER_H