/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <map>
#include <iostream>
#include <logging/LogWriter.h>

namespace rsp::logging {

static const std::map<LogLevel, std::string> cLevelToText = {
    { LogLevel::Emergency, "emergency" },
    { LogLevel::Alert, "alert" },
    { LogLevel::Critical, "critical" },
    { LogLevel::Error, "error" },
    { LogLevel::Warning, "warning" },
    { LogLevel::Notice, "notice" },
    { LogLevel::Info, "info" },
    { LogLevel::Debug, "debug" }
};

LogLevel ToLogLevel(std::string aLevelString)
{
    for (auto m : cLevelToText) {
        if (m.second == aLevelString) {
            return m.first;
        }
    }

    return cDefautLogLevel;
}

std::string ToString(LogLevel aLevel)
{
    return cLevelToText.at(aLevel);
}

} /* namespace logging */

