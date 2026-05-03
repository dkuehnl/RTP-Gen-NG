//
// Created by dkueh on 08.01.2026.
//

#include <iostream>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "CLI11.hpp"

#include "StreamOptions.h"
#include "yaml_template.h"

enum class DebugLevel { None, Verbose, MoreVerbose, Debug };

static std::string choose_editor() {
    const char* visual = std::getenv("VISUAL");
    if (visual && *visual) return visual;
    const char* editor = std::getenv("EDITOR");
    if (editor && *editor) return editor;
    return "vi";
}

static std::string create_tmp_template_file(const std::string& tpl, std::string& out_path) {
    char tmpl[] = "/tmp/rtpgen_XXXXXX.yaml";
    int fd = mkstemps(tmpl, 5);
    if (fd == -1) {
        throw std::runtime_error(std::string("mkstemp failed: ") + std::strerror(errno));
    }

    out_path = tmpl;

    ssize_t total = 0;
    const char* data = tpl.data();
    ssize_t len = static_cast<ssize_t>(tpl.size());
    while (total < len) {
        ssize_t n = write(fd, data + total, len - total);
        if (n < 0) {
            close(fd);
            unlink(out_path.c_str());
            throw std::runtime_error(std::string("write failed: ") + std::strerror(errno));
        }
        total += n;
    }

    fsync(fd);
    if (close(fd) != 0) {
        unlink(out_path.c_str());
        throw std::runtime_error(std::string("close failed: ") + std::strerror(errno));
    }

    return out_path;
}

static int launch_editor_blocking(const std::string& editor, const std::string& path) {
    pid_t pid = fork();
    if (pid < 0) {
        throw std::runtime_error(std::string("fork failed: ") + std::strerror(errno));
    }

    if (pid == 0) {
        std::vector<char*> args;
        args.push_back(const_cast<char*>(editor.c_str()));
        args.push_back(const_cast<char*>(path.c_str()));
        args.push_back(nullptr);

        execvp(editor.c_str(), args.data());
        _exit(127);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        throw std::runtime_error(std::string("waitpid failed: ") + std::strerror(errno));
    }

    return status;
}

static std::string read_file(const std::string& path) {
    std::ifstream ifs(path, std::ios::in |std::ios::binary);
    if (!ifs) {
        throw std::runtime_error(std::string("failed to open temp file or reading."));
    }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

void setup_argparser(CLI::App& app, StreamOptions& opts, int& v_count) {
    //Start-Values
    app.add_option(
        "-f,--file",
        opts.input_file,
        "File for Stream-Configuration, all other CLI-Args will be ignored"
        );
    app.add_flag(
        "-v",
        v_count,
        "Verbose mode, Increasable from -v up to -vvv"
        );
    app.add_option(
        "--dest-ip",
        opts.dest_ip,
        "Destination-IP address for the generated RTP-stream"
        );
    app.add_option(
        "--source-port",
        opts.source_port,
        "Set custom source port for the RTP packets, not mandatory"
        );
    app.add_option(
        "--dest-port",
        opts.dest_port,
        "Destionation port for the generated RTP-stream"
        );
    app.add_flag(
        "--use-tcp",
        opts.use_tcp,
        "Switch to TCP, not recommended for RTP"
        );
    app.add_option(
        "--start-ssrc",
        opts.start_ssrc,
        "Set custom start-ssrc, can be overwritten by configured events"
        )->default_str("0x112233");
    app.add_option(
        "--start-codec",
        opts.start_codec,
        "Set custom start-codec, can be overwritten by configured events"
        )->default_val(8);
    app.add_option(
        "--start_seq",
        opts.start_seq,
        "Set custom start-sequence-number, will be increased by seq-steps"
        )->default_val(0);
    app.add_option(
        "--seq-step",
        opts.seq_steps,
        "Sequence number will be increased by this value with every packet, can be overwritten by configured events"
        )->default_val(1);
    app.add_option(
        "--start-timestamp",
        opts.start_timestamp,
        "Set starting value for timestamp-field, will be increased by step-size"
        )->default_val(0);
    app.add_option(
        "--start-timestamp-step-size",
        opts.timestamp_step_size,
        "Timestamp will be increased by this value with every packet, can be overwritten by configured events"
        )->default_val(150);
    app.add_option(
        "--start-clockrate",
        opts.start_clockrate,
        "Sets clockrate for samples in one packet, important for timestamp-step calculation"
    );
    app.add_option(
        "--ptime",
        opts.ptime_in_packet,
        "Set the size in ms of payload contained in every rtp packet"
        )->default_val(20);
    app.add_option(
        "--ptime-btw",
        opts.ptime_btw_packet,
        "Set the time in ms between every single rtp packet. Should be same as ptime, but can be different"
        )->default_val(20);

    //Formatter
    app.footer("Beispiel: ./rtp-gen -i eth0 -p 5002");
    auto fmt = std::make_shared<CLI::Formatter>();
    fmt->column_width(50);
    app.formatter(fmt);

}

int main(int argc, char** argv) {
    StreamOptions opts{};
    int v_count = 0;
    if (argc == 1) {

        std::string path;
        try {
            create_tmp_template_file(YAML_TEMPLATE, path);

            const std::string editor = choose_editor();
            int status = launch_editor_blocking(editor, path);

            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                std::cerr << "Editor exited with error" << std::endl;
                unlink(path.c_str());
                return 1;
            }

            std::string content = read_file(path);

            std::cout << "Content: " << std::endl << content << std::endl;
            unlink(path.c_str());
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            if (!path.empty()) unlink(path.c_str());
            return 1;
        }

    } else {
        CLI::App app{
            "With RTPGen NG you can easily generate a fully customizable RTP-stream.\n"
            "General configuration can be done via CLI-Args, but timed scenarios (eg switch codec after 10 seconds) "
            "have to be defined and provided by a yaml-file."
        };
        argv = app.ensure_utf8(argv);
        setup_argparser(app, opts, v_count);
        CLI11_PARSE(app, argc, argv);
    }
    DebugLevel debug_level = DebugLevel::None;
    if (v_count == 1) {
        debug_level = DebugLevel::Verbose;
    } else if (v_count == 2) {
        debug_level = DebugLevel::MoreVerbose;
    } else if (v_count >= 3) {
        debug_level = DebugLevel::Debug;
    }

    std::cout << "Wir können jetzt hier weiter machen" << std::endl;
    return 0;
}