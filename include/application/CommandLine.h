/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_APPLICATION_COMMANDLINE_H_
#define INCLUDE_APPLICATION_COMMANDLINE_H_

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
    CommandLine(int argc, char **argv);

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
    bool HasOption(const std::string aStr);
    /**
     * Get the value assigned to an extended option, e.g. '--name=value'
     *
     * \param aOption Name of option
     * \param arValue Reference to string that is loaded with value.
     * \return bool True if the option exists
     */
    bool GetOptionValue(const std::string aOption, std::string &arValue);

    /**
     * Get a list of all commands on the command line. E.g. everything not starting with a '-'.
     * \return List of strings
     */
    std::vector<std::string>& GetCommands() { return mCommands; }

protected:
    std::vector<std::string> mOptions;
    std::vector<std::string> mCommands;
};

} /* namespace rsp::application */

#endif /* INCLUDE_APPLICATION_COMMANDLINE_H_ */
