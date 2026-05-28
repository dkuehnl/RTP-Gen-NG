//
// Created by dkueh on 06.05.2026.
//

#ifndef RTPGEN_NG_YAMLPARSER_H
#define RTPGEN_NG_YAMLPARSER_H

#include <stdexcept>
#include <string>

#include "StreamOptions.h"

class YamlFileNotFound : public std::runtime_error {
public:
    explicit YamlFileNotFound (const std::string& msg)
        : std::runtime_error(msg) {}
};

class WrongFileFormat : public std::runtime_error {
public:
    explicit WrongFileFormat (const std::string& msg)
        : std::runtime_error(msg) {}
};

class YamlUnknownTriggerType : public std::runtime_error {
public:
    explicit YamlUnknownTriggerType (const std::string& msg)
        : std::runtime_error(msg) {}
};


namespace yaml {
    StreamOptions parse(const std::string& filepath);
};


#endif //RTPGEN_NG_YAMLPARSER_H