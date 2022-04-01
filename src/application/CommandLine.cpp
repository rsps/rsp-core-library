/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <application/CommandLine.h>
#include <algorithm>
#include <filesystem>

namespace rsp::application {

CommandLine::CommandLine(int argc, const char **argv)
{
    if (argc > 0) {
        auto p = std::filesystem::path(argv[0]);
        mAppName = p.stem();
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            mOptions.push_back(std::string(argv[i]));
        } else {
            mCommands.push_back(std::string(argv[i]));
        }
    }
}

bool CommandLine::HasOption(const std::string aStr)
{
    return (std::find(mOptions.begin(), mOptions.end(), aStr) != mOptions.end());
}

bool CommandLine::GetOptionValue(const std::string aKey, std::string &arValue)
{
    for (auto o : mOptions) {
        if (o.find(aKey) != std::string::npos) {
            arValue = o.substr(aKey.length());
            return true;
        }
    }
    return false;
}

} /* namespace rsp::application */
