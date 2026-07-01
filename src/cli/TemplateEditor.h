//
// Created by dkueh on 01.07.2026.
//

#ifndef RTPGEN_NG_TEMPLATEEDITOR_H
#define RTPGEN_NG_TEMPLATEEDITOR_H

#include <string>


namespace template_editor {
    /**
     * @brief Runs the interactive "no-args" workflow: opens a YAML template
     *        in the user's editor and prints the edited content.
     *
     * Writes YAML_TEMPLATE to a temp file, blocks on the user's editor via
     * launch_editor_blocking(), then reads back and prints the edited content.
     * The temp file is always unlinked before returning, on every path.
     *
     * @return 0 on success; 1 if the editor exited with a non-zero status or
     *         an exception was thrown during the flow.
     */
    int run_interactive_editor_flow();

    /**
     * @brief Reads the full contents of a file into a string.
     * @param path Path to the file to read.
     * @return File contents.
     * @throws std::runtime_error If the file cannot be opened.
     */
    std::string read_file(const std::string& path);

    /**
     * @brief Launches editor as a blocking child process on the given file.
     *
     * Forks, execvp's editor with path as its sole argument, and waits for
     * the child to exit. Use WIFEXITED/WEXITSTATUS on the returned status to
     * check the outcome.
     *
     * @param editor Editor binary to launch (from choose_editor()).
     * @param path Path passed as the file argument to the editor.
     * @return Raw wait status as returned by waitpid(); not a plain exit code.
     * @throws std::runtime_error If fork() or waitpid() fails.
     */
    int launch_editor_blocking(const std::string& editor, const std::string& path);

    /**
     * @brief Writes tpl to a new uniquely-named temp file with a .yaml extension.
     * @param tpl Template content to write.
     * @param out_path Set to the created file's path on success.
     * @return The created file's path (same as out_path).
     * @throws std::runtime_error If mkstemps(), write(), or close() fails.
     *         On write/close failure, the partially-written temp file is unlinked.
     */
    std::string create_tmp_template_file(const std::string& tpl, std::string& out_path);

    /**
     * @brief Determines which editor binary to launch.
     * @return $VISUAL if set and non-empty, else $EDITOR if set and non-empty, else "vi".
     */
    std::string choose_editor();
};


#endif //RTPGEN_NG_TEMPLATEEDITOR_H