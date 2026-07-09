//
// Created by dkueh on 08.01.2026.
//

#include <condition_variable>
#include <csignal>
#include <mutex>
#include <iostream>

#include "StreamOptions.h"
#include "CliParser.h"
#include "TemplateEditor.h"
#include "RtpEngine.h"

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
        RtpEngine engine(raw_opts, debug_level);
        engine.run();

        engine.on_shutdown([] {
            g_shutdown_requested = true;
            g_shutdown_cv.notify_all();
        });

        std::unique_lock lock(g_shutdown_mutex);
        g_shutdown_cv.wait(lock, [] { return g_shutdown_requested.load(); });

        engine.stop();
    } catch (const std::logic_error& e) {
        std::cerr << "Internal Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::runtime_error& e) {
        std::cerr << "Failed to start: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Something went completely wrong. You're fucked up!" << std::endl;
        return 1;
    }

    return 0;
}