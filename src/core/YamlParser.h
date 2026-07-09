//
// Created by dkueh on 06.05.2026.
//

#ifndef RTPGEN_NG_YAMLPARSER_H
#define RTPGEN_NG_YAMLPARSER_H

#include <stdexcept>
#include <string>

#include "StreamOptions.h"

/// @brief Thrown if the specified file does not exist or is not a regular file.
class YamlFileNotFound : public std::runtime_error {
public:
    explicit YamlFileNotFound (const std::string& msg)
        : std::runtime_error(msg) {}
};

/// @brief Thrown if the file extension is not .yaml or .yml.
class WrongFileFormat : public std::runtime_error {
public:
    explicit WrongFileFormat (const std::string& msg)
        : std::runtime_error(msg) {}
};

/// @brief Thrown if a change event's trigger type string is not recognized.
class YamlUnknownTriggerType : public std::runtime_error {
public:
    explicit YamlUnknownTriggerType (const std::string& msg)
        : std::runtime_error(msg) {}
};

/// @brief Thrown if a change event's type string is not recognized.
class YamlUnknownChangeEvent : public std::runtime_error {
public:
    explicit YamlUnknownChangeEvent (const std::string& msg)
        : std::runtime_error(msg) {}
};

class YamlUnknownChannelType : public  std::runtime_error {
public:
    explicit YamlUnknownChannelType (const std::string& msg)
        : std::runtime_error(msg) {}
};


namespace yaml {

    /**
     * @brief Parses a YAML config file into a StreamOptions instance.
     *
     * Only connectionDetails.destinationIP and connectionDetails.destinationPort
     * are mandatory. All other fields are optional and left as std::nullopt if
     * absent — defaults are applied later by check_configuration().
     *
     * @param filepath Path to a .yaml or .yml file.
     * @return Populated StreamOptions struct.
     * @throws YamlFileNotFound        If the file does not exist.
     * @throws WrongFileFormat         If the extension is not .yaml/.yml.
     * @throws YamlUnknownTriggerType  If a trigger type string is not recognized.
     * @throws YamlUnknownChangeEvent  If a change event type string is not recognized.
     * @throws YAML::Exception         On malformed YAML or missing mandatory keys.
     */
    StreamOptions parse(const std::string& filepath);

    /**
     * @brief Parses a YAML config content into a StreamOptions instance.
     *
     * Only connectionDetails.destinationIP and connectionDetails.destinationPort
     * are mandatory. All other fields are optional and left as std::nullopt if
     * absent — defaults are applied later by check_configuration().
     *
     * @param content represented yaml-code as String.
     * @return Populated StreamOptions struct.
     * @throws YamlFileNotFound        If the file does not exist.
     * @throws WrongFileFormat         If the extension is not .yaml/.yml.
     * @throws YamlUnknownTriggerType  If a trigger type string is not recognized.
     * @throws YamlUnknownChangeEvent  If a change event type string is not recognized.
     * @throws YAML::Exception         On malformed YAML or missing mandatory keys.
     */
    StreamOptions parse_from_string(const std::string& content);
};


#endif //RTPGEN_NG_YAMLPARSER_H