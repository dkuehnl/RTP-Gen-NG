//
// Created by dkueh on 01.07.2026.
//

#include "CliParser.h"

namespace cliparser {
    int run_cli_flow(int argc, char** argv, StreamOptions& opts, int& v_count) {
        CLI::App app{
            "With RTPGen NG you can easily generate a fully customizable RTP-stream.\n"
            "General configuration can be done via CLI-Args, but timed scenarios (eg switch codec after 10 seconds) "
            "have to be defined and provided by a yaml-file."
        };
        argv = app.ensure_utf8(argv);
        setup_argparser(app, opts, v_count);
        CLI11_PARSE(app, argc, argv);

        return 0;
    }

    void setup_argparser(CLI::App& app, StreamOptions& opts, int& v_count) {
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

        //Formatter
        app.footer("Beispiel: ./rtp-gen -i eth0 -p 5002");
        auto fmt = std::make_shared<CLI::Formatter>();
        fmt->column_width(50);
        app.formatter(fmt);
    }

}