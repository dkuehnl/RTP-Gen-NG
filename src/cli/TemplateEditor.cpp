//
// Created by dkueh on 01.07.2026.
//

#include "TemplateEditor.h"

#include <iostream>
#include <cerrno>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>

#include "yaml_template.h"

namespace template_editor {
    int run_interactive_editor_flow(StreamOptions& opts) {
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

            opts.input_content = read_file(path);

            unlink(path.c_str());
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            if (!path.empty()) unlink(path.c_str());
            return 1;
        }

        return 0;
    }

    std::string read_file(const std::string& path) {
        std::ifstream ifs(path, std::ios::in |std::ios::binary);
        if (!ifs) {
            throw std::runtime_error(std::string("failed to open temp file or reading."));
        }
        std::ostringstream oss;
        oss << ifs.rdbuf();
        return oss.str();
    }

    int launch_editor_blocking(const std::string& editor, const std::string& path) {
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

    std::string create_tmp_template_file(const std::string& tpl, std::string& out_path) {
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

    std::string choose_editor() {
        const char* visual = std::getenv("VISUAL");
        if (visual && *visual) return visual;
        const char* editor = std::getenv("EDITOR");
        if (editor && *editor) return editor;
        return "vi";
    }
}