//
// Created by dkueh on 08.01.2026.
//

#include <condition_variable>
#include <csignal>
#include <mutex>
#include <iostream>
#include <fstream>

#include "StreamOptions.h"
#include "CliParser.h"
#include "TemplateEditor.h"
#include "RtpEngine.h"
#include "UnixSocketControlChannel.h"

namespace {
    std::condition_variable g_shutdown_cv;
    std::mutex g_shutdown_mutex;
    std::atomic_bool g_shutdown_requested{false};

    void handle_signal(int) {
        g_shutdown_requested = true;
        g_shutdown_cv.notify_all();
    }
}


int main(int argc, char** argv) {
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    StreamOptions raw_opts{};
    int v_count = 0;

    int result = (argc == 1)
        ? template_editor::run_interactive_editor_flow(raw_opts)
        : cliparser::run_cli_flow(argc, argv, raw_opts, v_count);

    if (result != 0) return result;

    DebugLevel debug_level = DebugLevel::None;
    if (v_count == 1) {
        debug_level = DebugLevel::Verbose;
    } else if (v_count == 2) {
        debug_level = DebugLevel::MoreVerbose;
    } else if (v_count >= 3) {
        debug_level = DebugLevel::Debug;
    }

    try {
        UnixSocketControlChannel control_channel("/tmp/rtpgen.sock");
        RtpEngine engine(raw_opts, debug_level, control_channel);
        engine.run();

        std::unique_lock lock(g_shutdown_mutex);
        g_shutdown_cv.wait(lock, [] { return g_shutdown_requested.load(); });

        engine.stop();
    } catch (...) {
        std::cerr << "Something went completly wrong. You're fucked up!" << std::endl;
        return 1;
    }

    return 0;
}