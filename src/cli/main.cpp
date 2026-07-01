//
// Created by dkueh on 08.01.2026.
//

#include <iostream>
#include <fstream>

#include "StreamOptions.h"
#include "CliParser.h"
#include "TemplateEditor.h"

enum class DebugLevel { None, Verbose, MoreVerbose, Debug };

int main(int argc, char** argv) {
    StreamOptions opts{};
    int v_count = 0;

    int result = (argc == 1)
        ? template_editor::run_interactive_editor_flow()
        : cliparser::run_cli_flow(argc, argv, opts, v_count);

    if (result != 0) return result;

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