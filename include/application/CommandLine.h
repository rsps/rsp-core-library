/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_APPLICATION_COMMAND_LINE_H
#define RSP_CORE_LIB_APPLICATION_COMMAND_LINE_H

#include <string>
#include <vector>

namespace rsp::application {

/**
 * \class CommandLine
 * \brief Encapsulation of the command line arguments.
 */
class CommandLine
{
public:
    /**
     * Constructs this object with content from the command line.
     *
     * \param argc
     * \param argv
     */
    CommandLine(int argc, const char **argv);

    /**
     * Get the decoded command line options.
     *
     * \return List of strings
     */
    std::vector<std::string>& GetOptions() { return mOptions; }

    /**
     * Check if a given option exist on the command line.
     *
     * \param aStr
     * \return True if the option exists
     */
    bool HasOption(const std::string &arStr);
    bool HasOption(const char *apStr) { return HasOption(std::string(apStr)); }

    /**
     * Get the value assigned to an extended option, e.g. '--name=value'
     *
     * \param aOption Name of option
     * \param arValue Reference to string that is loaded with value.
     * \return bool True if the option exists
     */
    bool GetOptionValue(const std::string &arOption, std::string &arValue);
    bool GetOptionValue(const char *apOption, std::string &arValue) { return GetOptionValue(std::string(apOption), arValue); }

    /**
     * Get a list of all commands on the command line, except the one invoking this application.
     * E.g. everything not starting with a '-'.
     *
     * \return List of strings
     */
    std::vector<std::string>& GetCommands() { return mCommands; }

    /**
     * \brief Get the application name.
     *
     * The name is extracted from the invoking command line string.
     *
     * \return string
     */
    const std::string& GetAppName() { return mAppName; }

protected:
    std::string mAppName{};
    std::vector<std::string> mOptions{};
    std::vector<std::string> mCommands{};
};

} /* namespace rsp::application */

#endif // RSP_CORE_LIB_APPLICATION_COMMAND_LINE_H
