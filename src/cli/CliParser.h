//
// Created by dkueh on 01.07.2026.
//

#ifndef RTPGEN_NG_CLIPARSER_H
#define RTPGEN_NG_CLIPARSER_H

#include "CLI11.hpp"

#include "StreamOptions.h"

namespace cliparser {

    /**
     * @brief Parses CLI arguments into a StreamOptions instance.
     *
     * Builds a CLI11 app, registers all options via setup_argparser(), and
     * parses argv. On parse error, CLI11 prints its own usage/error message
     * and this function returns a non-zero exit code; the caller should
     * return that value directly from main() without further handling.
     *
     * @param argc Argument count, as passed to main().
     * @param argv Argument vector, as passed to main().
     * @param opts StreamOptions to populate from parsed CLI arguments.
     * @param v_count Verbosity counter, incremented once per -v flag.
     * @return 0 on success; non-zero if CLI11 reported a parse error.
     */
    int run_cli_flow(int argc, char** argv, StreamOptions& opts, int& v_count);

    /**
     * @brief Registers all CLI options and the help formatter on app.
     *
     * Only a reduced set of connection-level options is exposed via CLI
     * (dest-ip, dest-port, source-port, input file, verbosity). Stream-start
     * values and change-events are intentionally CLI-config-only via -f/--file
     * and not exposed as individual flags.
     *
     * @param app CLI11 app to configure; mutated in-place.
     * @param opts StreamOptions whose fields are bound to CLI options.
     * @param v_count Verbosity counter bound to the -v flag.
     */
    void setup_argparser(CLI::App& app, StreamOptions& opts, int& v_count);
};


#endif //RTPGEN_NG_CLIPARSER_H