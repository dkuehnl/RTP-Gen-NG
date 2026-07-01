//
// Created by dkueh on 01.07.2026.
//

#ifndef RTPGEN_NG_CLIPARSER_H
#define RTPGEN_NG_CLIPARSER_H

#include "CLI11.hpp"

#include "StreamOptions.h"

namespace cliparser {
    int run_cli_flow(int argc, char** argv, StreamOptions& opts, int& v_count);
    void setup_argparser(CLI::App& app, StreamOptions& opts, int& v_count);
};


#endif //RTPGEN_NG_CLIPARSER_H